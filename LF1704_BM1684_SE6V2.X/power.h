/* 
 * File:   power.h
 * Author: darren
 *
 * Created on February 23, 2023, 3:18 PM
 */

#ifndef POWER_H
#define	POWER_H

#ifdef	__cplusplus
extern "C" {
#endif

void power_on(void);
void power_off(void);
void power_ctrl(void);
//void reset_ctrl(void);

#ifdef	__cplusplus
}
#endif

#endif	/* POWER_H */

