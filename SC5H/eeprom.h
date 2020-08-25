#ifndef __EEPROM__
#define __EEPROM__

void eeprom_init(void);
int eeprom_get_lock_status(void);
void eeprom_lock_code(uint8_t code);

#endif
