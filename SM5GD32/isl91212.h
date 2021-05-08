#ifndef __ISL91212_H__
#define __ISL91212_H__

int isl91212_init(void);
void isl91212_destroy(void);
int isl91212_buck_on(unsigned int buck);
void isl91212_buck_off(unsigned int buck);
int isl91212_voltage_config(unsigned int buck, unsigned int voltage);
unsigned long isl91212_output_current(unsigned int buck);
unsigned long isl91212_output_voltage(unsigned int buck);

#endif
