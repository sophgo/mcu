#include <dvfs.h>
#include <freq.h>
#include <math.h>
#include <mon.h>
#include <flash.h>
#include <system.h>
#include <power.h>
#include <timer.h>
#include <tick.h>
#include <mcu.h>
#include <isl68224.h>
#include <adc.h>
#include <common.h>
#include <debug.h>

#define TPU_MPLL		2
#define PARENT_FREQ		25 * MHZ
#define CRITICAL_TEMP		105
#define TEMP_DETECT_INTERVAL	500
#define mW			1000UL

struct dvfs_pair freq_volt_pair[] = {
	{1000 * MHZ, 800},
	{950 * MHZ, 780},
	{900 * MHZ, 780},
	{850 * MHZ, 760},
	{800 * MHZ, 760}, // 4
	{750 * MHZ, 740},
	{700 * MHZ, 740},
	{650 * MHZ, 720},
	{600 * MHZ, 720},//8
	{550 * MHZ, 700},
	{500 * MHZ, 700},
	{50 * MHZ, 680},//11
};

struct dvfs_trip dvfs_trips[] = {
	{85, 5,  &freq_volt_pair[4], &freq_volt_pair[0]},
	{95, 5,  &freq_volt_pair[11], &freq_volt_pair[4]},
};

static struct dvfs_sensor dvfs_sensor[SOC_NUM];
static struct dvfs_power dvfs_power;
static int over_temp_count[SOC_NUM];
static struct dvfs_pair last_pair[SOC_NUM];
static unsigned long last_time_temp;
uint8_t dvfs_p_enable = 0;
uint8_t atx_300W = 0;
static uint8_t dvfs_p_init_flag;

void dvfs_p_init(void);

static struct dvfs_pair* dvfs_get_tpu_target_freq_volt(uint8_t idx,
				    	  	       enum dvfs_freq_scaling_caller caller,
				    		       struct dvfs_pair *target_pair)
{
	uint64_t temp_freq_target = 0;
	uint64_t power_freq_target = 0;

	if (!target_pair || idx >= SOC_NUM) {
		debug("dvfs get tpu target freq volt fail!\n");
        	return NULL;
   	}

	if (caller == FREQ_CALLER_TEMP) {
		dvfs_sensor[idx].temp_target_pair = target_pair;
	} else if (caller == FREQ_CALLER_POWER) {
		dvfs_power.power_target_pair = target_pair;
	}
	temp_freq_target = dvfs_sensor[idx].temp_target_pair->freq;
	power_freq_target = dvfs_power.power_target_pair->freq;

	if (temp_freq_target < power_freq_target)
		return dvfs_sensor[idx].temp_target_pair;

	return &freq_volt_pair[dvfs_power.dvfs_level];

}

static void __dvfs_set_freq_volt(int idx,
				 int mode,
			         struct dvfs_pair *target_pair)
{
	int ret = 0;

	if (mode == DVFS_DOWN_MODE) {
		ret = sg2044_clk_pll_set_rate(idx, TPU_MPLL, target_pair->freq, PARENT_FREQ);
		if (ret != 0) {
			debug("device chip tpu freq cfg fail, %d\n", ret);
			return;
		}
		timer_udelay(20);
		isl68224_set_out_voltage(idx, 0, target_pair->volt);
	} else if (mode == DVFS_UP_MODE) {
			isl68224_set_out_voltage(idx, 0, target_pair->volt);
			timer_udelay(20);
			ret = sg2044_clk_pll_set_rate(idx, TPU_MPLL, target_pair->freq, PARENT_FREQ);
			if (ret != 0) {
				debug("device chip tpu freq cfg fail, %d\n", ret);
				return;
			}
	}
}

static void dvfs_set_freq_volt(int idx,
			       enum dvfs_freq_scaling_caller caller,
			       struct dvfs_pair *input_pair)
{
	int mode;
	struct dvfs_pair *target_pair;

	target_pair = dvfs_get_tpu_target_freq_volt(idx, caller, input_pair);
	if (target_pair == NULL)
		return;
	if (target_pair->freq < last_pair[idx].freq)
		mode = DVFS_DOWN_MODE;
	else if (target_pair->freq > last_pair[idx].freq)
		mode = DVFS_UP_MODE;
	else
		return;

	__dvfs_set_freq_volt(idx, mode, target_pair);
	last_pair[idx] = *target_pair;
}

static void temp_thermal_trip(const struct dvfs_trip *trips,
					    struct dvfs_sensor *sensor,
					    int trips_size, int idx)
{

	for (int i = 0; i < trips_size; i++) {
		//debug("chip %d temp%d \n",idx, sensor->temp);
		//debug("trip_bak:%d flag:%d\n", sensor->trip_back,sensor->hysteresis_flag);
		if (sensor->last_temp < trips[i].trip_temp &&
		    sensor->temp >= trips[i].trip_temp) {
			sensor->hysteresis_flag = 0;
			dvfs_set_freq_volt(idx, FREQ_CALLER_TEMP, trips[i].temp_up_pair);
			debug("FREQ DOWN: chip%d  set tpu freq is %lu\n",
				   idx, trips[i].temp_up_pair->freq);
		}

		if (sensor->last_temp >= trips[i].trip_temp &&
		    sensor->temp < trips[i].trip_temp) {
			sensor->hysteresis_flag = 1;
			sensor->trip_back = i;
			//debug("into down,trip:%d\n",i);
		}

		if (sensor->temp < (trips[sensor->trip_back].trip_temp - trips[sensor->trip_back].hysteresis) &&
		    sensor->hysteresis_flag == 1) {
			sensor->hysteresis_flag = 0;
			dvfs_set_freq_volt(idx, FREQ_CALLER_TEMP, trips[sensor->trip_back].temp_down_pair);
			debug("FREQ UP:chip%d, set tpu freq is %lu\n",
				   idx, trips[sensor->trip_back].temp_down_pair->freq);
		}
	}

	// critical detect
	if (sensor->temp >= CRITICAL_TEMP) {
		over_temp_count[idx]++;
		if (over_temp_count[idx] == 5) {
			power_off();
			eeprom_write_byte(EEPROM_POWER_OFF_REASON_OFFSET, POWER_OFF_REASON_OVER_HEAT);
			debug("chip%d current temperature is over %d celsius degree, power off!\n",
				   idx, CRITICAL_TEMP);
		}
	}
}

int power_thermal_trip(int idx)
{
	struct dvfs_pair *power_pair;

	if (dvfs_power.power_average >= dvfs_power.power_upper_threshold) {
		if (dvfs_power.dvfs_level < dvfs_power.dvfs_max_level - 1){
			dvfs_power.dvfs_level++;
			power_pair = &freq_volt_pair[dvfs_power.dvfs_level];
			//debug("chip %d, VDFS-P DOWN\n", idx);
		} else
			return 0;
	} else if (dvfs_power.power_average <= dvfs_power.power_lower_threshold) {
		if (dvfs_power.dvfs_level > 0){
			dvfs_power.dvfs_level--;
			power_pair = &freq_volt_pair[dvfs_power.dvfs_level];
			//debug("chip %d, VDFS-P UP\n", idx);
		} else
			return 0;
	} else {
		return 0;
	}

	for (int i = 0; i < SOC_NUM; i++) {
		dvfs_set_freq_volt(i, FREQ_CALLER_POWER, power_pair);
	}

	return 0;
}

void dvfs_process(void)
{
	unsigned long current_time;

	current_time = tick_get();
	if (current_time - last_time_temp > TEMP_DETECT_INTERVAL) {
		for(int i = 0; i < SOC_NUM; i++ ) {
			dvfs_sensor[i].last_temp = dvfs_sensor[i].temp;
			dvfs_sensor[i].temp = get_soc_temp(i);
			temp_thermal_trip(dvfs_trips, &dvfs_sensor[i], ARRAY_SIZE(dvfs_trips), i);
			if(dvfs_p_enable) {
				dvfs_power.power_average = get_12v_power();
				debug("power_average: %u\n",dvfs_power.power_average);
				debug("dvfs_level: %d\n",dvfs_power.dvfs_level);
				power_thermal_trip(i);
				debug("\n");
				dvfs_p_init_flag = 1;
			} else if(dvfs_p_enable == 0 && dvfs_p_init_flag == 1) {
				dvfs_p_init();
				dvfs_p_init_flag = 0;
			}
		}
		last_time_temp = current_time;
	}
}

/* In some versions, the maximum power  is equal to 350W 
 * and needs to be determined */
void dvfs_p_threshold()
{		
	if (get_pcb_version() != 0) {
		if (atx_300W == 1) {
			dvfs_power.power_upper_threshold = 300;
			dvfs_power.power_lower_threshold = 300;
		} else if (atx_300W == 0) {
			dvfs_power.power_upper_threshold = 350;
			dvfs_power.power_lower_threshold = 350;
		} else {
			dvfs_power.power_upper_threshold = 350;
			dvfs_power.power_lower_threshold = 350;
		}
	} else {
		dvfs_power.power_upper_threshold = 300;
		dvfs_power.power_lower_threshold = 300;
	}

	debug("power_upper_threshold: %uW\n",dvfs_power.power_upper_threshold);
}

/* atx_300W: 1, max power 300w；*
 * atx_300W: 0, max power 350w；*/
void dvfs_p_init(void)
{
	dvfs_power.dvfs_max_level = ARRAY_SIZE(freq_volt_pair);
	dvfs_power.power_average = get_12v_power();
	dvfs_power.power_target_pair = &freq_volt_pair[0];
	dvfs_power.dvfs_level = 0;
}

void dvfs_t_init(void)
{
	for(int i = 0; i < SOC_NUM; i++ ) {
		dvfs_sensor[i].last_temp = dvfs_sensor[i].temp =  get_soc_temp(i);
		dvfs_sensor[i].temp_target_pair = &freq_volt_pair[0];
		dvfs_sensor[i].trip_back = dvfs_sensor[i].hysteresis_flag = 0;
	}
}

void dvfs_init(void)
{
	//int dvfs_mode;
	last_time_temp = tick_get();
#if 0
	dvfs_mode = eeprom_read_byte(EEPROM_DVFS_MODE_OFFSET);
	switch (dvfs_mode)
	{
	case MODE_DVFS_350W:
		dvfs_p_enable = 1;
		atx_300W = 0; //350W
		debug("MODE_DVFS_350W\n");
		break;
	case MODE_DVFS_300W:
		dvfs_p_enable = 1;
		atx_300W = 1;//300W
		debug("MODE_DVFS_300W\n");
		break;
	case NO_DVFS:
		atx_300W = 0;
		dvfs_p_enable = 0;
		debug("NO_DVFS\n");
		break;
	default:
		atx_300W = 0;
		dvfs_p_enable = 0;
		debug("default:NO_DVFS\n");
		break;
	}
#endif

	if (gpio_get(PWR_SENSE0_PORT, PWR_SENSE0_PIN) == 0 &&
	    gpio_get(PWR_SENSE1_PORT, PWR_SENSE1_PIN) == 0) {
		dvfs_power.power_upper_threshold = 350;
		dvfs_power.power_lower_threshold = 350;
		atx_300W = 0;
	} else if (gpio_get(PWR_SENSE0_PORT, PWR_SENSE0_PIN) == 1 &&
		   gpio_get(PWR_SENSE1_PORT, PWR_SENSE1_PIN) == 0) {
		dvfs_power.power_upper_threshold = 350;
		dvfs_power.power_lower_threshold = 350;
		atx_300W = 0;
	} else if (gpio_get(PWR_SENSE0_PORT, PWR_SENSE0_PIN) == 0 &&
		   gpio_get(PWR_SENSE1_PORT, PWR_SENSE1_PIN) == 1) {
		dvfs_power.power_upper_threshold = 300;
		dvfs_power.power_lower_threshold = 300;
		atx_300W = 1;
	} else {
		dvfs_power.power_upper_threshold = 300;
		dvfs_power.power_lower_threshold = 300;
		atx_300W = 1;
	}

	/* In some versions, the maximum power  is equal to 350W 
	* and needs to be determined */
	if (get_pcb_version() == 0 && dvfs_power.power_upper_threshold == 350) {
		dvfs_power.power_upper_threshold = 300;
		dvfs_power.power_lower_threshold = 300;	
		atx_300W = 1;
	}

	debug("power_upper_threshold: %uW\n",dvfs_power.power_upper_threshold);

	dvfs_t_init();
	dvfs_p_init();
}





