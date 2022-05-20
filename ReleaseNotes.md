# MCU Firmware Release Notes

## SC5 (EVB)

Version 9:

    1. sys reset pin not pull down for 30ms

Version 10:

    1. add data eeprom lock

Version 11:

    1. Add board type in firmware

version 12:

    1. reset watchdog on system startup
    2. fixbug, pcie mode check failed

## SC5H

Version 4:

    1. fixbug, soc reset not working as expected
    2. power tune

Version 5:

    1. fixbug, pcb version and bom version are reversed

Version 6:

    1. add data eeprom lock

Version 7:

    1. Add board type in firmware

## SC5PLUS

Version 6:

    1. TPU power supply from 0.792V to 0.774V

Version 7:

    1. add data eeprom lock

Version 8:

    1. Add board type in firmware

## SC5PRO

Version 0:

    1. Initial release

Version 1:

    1. Change baudrate from 921600 to 115200

Version 2:

    1. Add clear mode for mon collection

Version 3:

    1. Fixbug, MCU uart console may hangup

Version 4:

    1. fixbug, sc5pro smbus cannot working

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

Version 41:

    1. fixbug hds cannot power-on on v3 motherboard
    2. se5 smbus alert pin config wrong
    3. se5 under temperature control error
    4. remove 90C error led flicker

Version 42:

    1. fixbug: tmp451 temperature wrong when under zero

Version 44:

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

Version 49:

    1. Add board type in firmware

version 50:

    1. reset watchdog on system startup

Version 51:

    1. new feature, se5 motherboard re-power just after upgrade firmware
    2. fixbug, power-on power-off reboot failed, on wide temperature operation board

Version 52:

    1. fixbug: i2c upgrader cannot work

Version 53:

    1. sa5: auto power on when core board powered up, for some customers' private design

Version 54:

    1. fixbug: se5 error led may not on during boot

## SE5 LITE

Version 0:

    1. Bringup, all functions are supported, except WOL
    2. Heater control is addded but not tested
    3. WOL is not supported yet

Version 1:

    1. WOL support
    2. power consumption support

Version 2:
    1. fixbug, heater not powered on when temperature under zero

Version 3:

    1. tune power on sequence

Version 4:

    1. fixbug, hardware version adc get wrong value.

Version 5:

    1. fixbug, reboot may fail, system power off but cannot power on
    2. fixbug, power off may fail, system may perform a reboot instead of power off
    3. add 100us delay after asm3142 vdd33 on

## SM5 GD32 Alternative

Version 0:

    1. GD32 Alternative full function support for SM5
    2. Online upgrade tool should upgrade to v2.0 or later

## SM5 MINI

Version 0:

    1. first release

Version 1:

    1. fixbug: pcie reboot failed

Version 3:

    1. Add board type in firmware
    2. Fixbug, not power off 1.8v on startup, on wide temperature operation board

version 4:

    1. reset watchdog on system startup
    2. fixbug, power-on power-off reboot failed

Version 5:

    1. fixbug: i2c upgrader cannot work

Version 6:

    1. Factory test support

Version 7:

    1. fixbug pcie mode cannot work on server

Version 8:

    1. update version table

Version 9:

    1. fixbug, current acquire value not accurate enough
    2. add power consumption read

Version 11:

    1. add board type : SM5SE6M
    2. se6 ctrl bord auto detect(eeprom detect)
    3. add bord id/ip
    4. se6 ctrl  upgrade type
    5. se6  key  power on/off、fact reset
    6. aiu reset (pciee reset)
    7. se6 ctrl eeprom  (NVMEM)  read/write

## SM5G

Version 0:

    1. SM5G initial release

Version 1:

    1. SM5G initial release for 1st pcb improvement

Version 2:

    1. fixbug, sm5g does not send ack as iic slave

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

Version 5:

    1. new feature: 128 bytes program flash based none volatile memory

## SE5 Motherboard V2

Version 0:

    1. se5 support motherboard power off reboot

Version 1:

    1. se5 fixbug, power button keep pressed down after board powered down will cause board power on.

