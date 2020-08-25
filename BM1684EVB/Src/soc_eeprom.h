/*
 * soc_eeprom.h
 *
 *  Created on: Jul 22, 2019
 *      Author: weic
 */

#ifndef __SOC_EEPROM__
#define __SOC_EEPROM__

#include <stdint.h>
extern void eeprom_init(void);

int eeprom_get_lock_status(void);
void eeprom_lock_code(uint8_t code);
void eeprom_write_byte_protected(uint16_t offset, uint8_t data);
#endif /* DS1307_H_ */
