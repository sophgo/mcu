/*
 * soc_eeprom.h
 *
 *  Created on: Jul 22, 2019
 *      Author: weic
 */

#ifndef __SOC_EEPROM__
#define __SOC_EEPROM__

void eeprom_init(void);
void eeprom_log_power_off_reason(int reason);

#define EEPROM_POWER_OFF_REASON_POWER_OFF	0x80
#define EEPROM_POWER_OFF_REASON_RESET		0x81
#define EEPROM_POWER_OFF_REASON_REBOOT		0x82
#define EEPROM_POWER_OFF_REASON_OVER_HEAT	0x83
#define EEPROM_POWER_OFF_REASON_WATCHDOG	0x84

#endif /* DS1307_H_ */
