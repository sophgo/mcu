# MCU Firmware Release Notes

## SC5 (EVB)

Version 9:

    1. sys reset pin not pull down for 30ms

Version 10:

    1. add data eeprom lock

## SC5H

Version 4:

    1. fixbug, soc reset not working as expected
    2. power tune

Version 5:

    1. fixbug, pcb version and bom version are reversed

Version 6:

    1. add data eeprom lock

## SC5PLUS

Version 6:

    1. TPU power supply from 0.792V to 0.774V

Version 7:

    1. add data eeprom lock

## SC5PRO

Version 0:

    1. Initial release

Version 1:

    1. Change baudrate from 921600 to 115200

Version 2:

    1. Add clear mode for mon collection

## SA5/SM5/SE5

Version 34:

    1. sm5 soc mode and sm5 pcie mode auto adapt
    2. sm5 soc mode if tca6416 is on board, otherwise sm5 pcie mode is selected.

Version 35:

    1. se5 support motherboard power off reboot

Version 36:

    1. based on v36, but power off threshold changed from
    2. soc temperature 95C and board temperature 85C to soc temperature 100C and board temperature 90C

Version 37:

    1. support V3 version SE5 motherboard
    2. support tmp451 extended mode(temperature under zero)

Version 38:

    1. fixbug, mcu may loss host pcie ep reset
    2. new adaptive logic

Version 39:

    1. SE5 over temperature alert support (error led flicker at 0.5Hz)
    2. fixbug, some core board may failed to detect base board
    3. speedup MCU clock to 32MHz

Version 40:

    1. fixbug, test i2c port read may failed.

Version41:

    1. fixbug hds cannot power-on on v3 motherboard
    2. se5 smbus alert pin config wrong
    3. se5 under temperature control error
    4. remove 90C error led flicker

Version42:

    1. fixbug: tmp451 temperature wrong when under zero

Version44:

    1. fixbug: cannot write eeprom just after upgrade
    2. add data eeprom lock

Version 45:

    1. se5 pic support(i2c transaction pass through)

Version 46:

    1. new feature sm5 reset button support

Version 47:

    1. workaround: retry at most 10 times if pic not ack address

Version 48:

    1. Wide temperature operating core board adaption

## SE5 Motherboard V3 or Later (PCB v1.2)

Version 0:

    1. SE5 motherboard V3(PCB V1.2) support
    2. support power button and reset button on linux
    3. support HDS heater control

Version 1:

    1. fixbug: se5 motherboard v3 heater control, it cannot switch heater on when temperature is too low.

Version 2:

    1. fixbug: power off key for system level power off cannot work.

Version 3:

    1. Add power acquisition.

Version 4:

    1. Fixbug: thermal led flicker just after power plug.

