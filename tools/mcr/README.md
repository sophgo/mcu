MCU UART Console Reference Code for SC5PRO PCIE Driver
======================================================

Purpose
-------
SC5PRO implemented an UART based console. This console is used for human or TPU
chip to control MCU, or get information from MCU. It is friendly for humanbeing,
but it maybe a bit hard for a program.

This code provides a reference design for PCIE device driver to involke commands
on MCU console. It is compiled on linux based host and runs on it. But it should
easily port to PCIE driver. The principles of this code are:  
1. Easy to use  
2. Easy to port  

Files
-----
console.c console.h: MCU uart console logic implementation  
xmodem.c xmodem.h: xmodem transmitter implementation
uart.c uart.h: linux based serial interface porting  
timer.c timer.h: linux based simple timer implementation  
log.h: output functions  
main.c: test program  

Porting Guidence
----------------
1. console.c console.h shouldnot be changed except for header files included.  
1. xmodem.c xmodem.h same with console.c and console.h
2. uart.c uart.h should be replaced by implementer. Implementer can change the
definition of data structer uart\_ctx but it shouldnot change the external
function declaration in uart.h.  
3. timer.h and timer.c should follow the same rule with uart.c and uart.h. The
simple timer is implemented as mostly software timers. The only function that
cannot cross between different platforms is current\_time. This function get
a counter value with a precision of milliseconds. It is not necessary to get
a real time value. For PCIE driver which compiled with linux kernel, it may
replace function gettimeofday with ktime\_get\_ns.  
4. log.h implements some output method, including debug message, error message
and information message. In linux userland, I used function fprintf. For PCIE
driver, fprintf can be replaced with dev\_err, dev\_info or dev\_dbg.  
5. main.c is not needed by PCIE device dirver.  

