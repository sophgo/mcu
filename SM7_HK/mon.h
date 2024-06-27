#ifndef __MON_H__
#define __MON_H__

void mon_init(void);
void mon_process(void);
uint16_t get_current(void);
uint8_t get_pcb_version(void);
uint8_t get_bom_version(void);
uint8_t get_hardware_version(void);
int get_ntc_tmp(void);
void set_hardware_version(unsigned int pcb_ver_val, unsigned int bom_ver_val);
#endif
