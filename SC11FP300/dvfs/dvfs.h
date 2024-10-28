#ifndef __DVFS_H__
#define __DVFS_H__

#include <stdint.h>
#include <common.h>

enum DVFS_MODE {
	MODE_DVFS_350W = 0,
	MODE_DVFS_300W,
	NO_DVFS,
	DVFS_MODE_BUTT
};

enum dvfs_scaling_mode {
	DVFS_UP_MODE = 0,
	DVFS_DOWN_MODE,
	DVFS_INIT_MODE,
	DVFS_SCAL_BUTT
};

enum dvfs_freq_scaling_caller {
	FREQ_CALLER_TEMP = 0,
	FREQ_CALLER_POWER,
	FREQ_CALLER_BUTT
};

struct dvfs_pair {
	uint64_t freq;
	uint32_t volt;
};

struct dvfs_trip {
	int trip_temp;
	int hysteresis;
	struct dvfs_pair *temp_up_pair;
	struct dvfs_pair *temp_down_pair;
};

struct dvfs_sensor {
	int temp;
	int last_temp;
	int hysteresis_flag;
	int trip_back;
	struct dvfs_pair *temp_target_pair;
};

struct dvfs_power {
	uint32_t power_average;
	uint32_t power_upper_threshold;
	uint32_t power_lower_threshold;
	uint32_t dvfs_level;
	uint32_t dvfs_max_level;
	struct dvfs_pair *power_target_pair;
};

int dvfs_init(void);
int dvfs_process(void);
void dvfs_p_threshold(void);

#endif