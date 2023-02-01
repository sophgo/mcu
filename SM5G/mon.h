#ifndef __MON_H__
#define __MON_H__

void mon_init(void);
uint8_t get_pcb_version(void);
uint8_t get_bom_version(void);
uint8_t get_hardware_version(void);
unsigned long get_current(void);

#endif
