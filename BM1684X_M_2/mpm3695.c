#include <i2c_master.h>
#include <mpm3695.h>
#include <debug.h>

#define I2C I2C2
#define MPM3695_SLAVE_ADDR 0x30 //0x30
#define VOUT_COMMAND_REG 0x21
#define VOUT_SCALE_LOOP_REG 0x29
#define MFR_CTRL_OPS 0xD2

#define VOUT_COMMAND_LSB 2 // 2mV/LSB
#define VOUT_SCALE_LOOP_LSB 0.001 // 0.001/LSB
#define VOL_DIVIDER_RESISTOR (30.0/31) //(30/31)
#define UNITY_VOUT_SCALE_LOOP 0.333

static inline int mpm3695_write_word(unsigned char cmd, uint16_t data) {
    while (i2c_master_smbus_write_word(I2C, MPM3695_SLAVE_ADDR, 1, cmd, data));
    return 0;
}

static inline uint16_t mpm3695_read_word(unsigned char cmd) {
    uint16_t tmp;
    while (i2c_master_smbus_read_word(I2C, MPM3695_SLAVE_ADDR, 1, cmd, &tmp));
    return tmp;
}

static inline int mpm3695_write_byte(unsigned char cmd,
				    unsigned char data)
{
	while (i2c_master_smbus_write_byte(I2C, MPM3695_SLAVE_ADDR, 1, cmd, data));
		;
	return 0;
}

static inline int mpm3695_read_byte(unsigned char cmd)
{
	unsigned char tmp;
	while (i2c_master_smbus_read_byte(I2C, MPM3695_SLAVE_ADDR,
					  1, cmd, &tmp));
		;
	return tmp;
}

void set_voltage(int voltage_mV) {
    float vout_command_float = (float)voltage_mV / (VOUT_COMMAND_LSB * UNITY_VOUT_SCALE_LOOP * VOL_DIVIDER_RESISTOR);
    uint16_t vout_command = (uint16_t)vout_command_float; 
    debug("Setting voltage: %d mV, VOUT_COMMAND: %f\n", voltage_mV, vout_command_float);
    mpm3695_write_word(VOUT_COMMAND_REG, vout_command);
}

int get_voltage_mV(void) {
    debug("Entering get_voltage_mV\n");
    uint16_t vout_command = mpm3695_read_word(VOUT_COMMAND_REG);
    debug("Read VOUT_COMMAND: 0x%04X\n", vout_command);
    float voltage_mV = (float)(vout_command * VOUT_COMMAND_LSB * UNITY_VOUT_SCALE_LOOP);
    debug("Calculated Voltage before adjustment: %f mV\n", voltage_mV);
    int voltage_mV_int = (int)(voltage_mV * VOL_DIVIDER_RESISTOR);
    debug("Final Calculated Voltage after adjustment: %d mV\n", voltage_mV_int);
    return voltage_mV_int;
}

 void init_mpm3695(void){
    debug("Entering init_3695\n");
    mpm3695_write_byte(MFR_CTRL_OPS,0x07);
    // 0000 0111

    uint16_t vout_command = mpm3695_read_byte(MFR_CTRL_OPS);
    debug("Read MFR_CTRL_OPS: 0x%02X\n", vout_command);
}