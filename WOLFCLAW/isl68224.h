#ifndef __ISL68224_H__
#define __ISL68224_H__

void isl68224_get_nvm_slot_num(void);
void isl68224_get_device_id(void);
void isl68224_get_reversion_id(void);
int isl68224_proof(char *name);
int isl68224_program(char *name);
int isl68224_program_from_mcuflash(uint32_t file_addr);
#endif