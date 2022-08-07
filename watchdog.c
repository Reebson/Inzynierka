/*
 * watchdog.c
 *
 * Created: 30.05.2022 13:58:08
 *  Author: piotr
 */ 
#include "watchdog.h"

void WDT_off(uint8_t int_enable) {
	
	if(int_enable != 0) {
		cli();
	}
	
	wdr();
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1<<WDRF);
	/* Write logical one to WDCE and WDE */
	/* Keep old prescaler setting to prevent unintentional time-out */
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	/* Turn off WDT */
	WDTCSR = 0x00;
	
	if(int_enable != 0) {
		sei();
	}
}

void WDT_enable() {
	wdr();
	MCUSR &= ~(1<<WDRF);
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR |= (1<<WDE);
}

void WDT_Prescaler_Change(uint8_t int_enable, wdt_timeout_e timeout)
{
	if(int_enable != 0) {
		cli();
	}
	
	wdr();
	
	/* Start timed sequence */
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	/* Set new prescaler(time-out) value = 64K cycles (~0.5 s) */
	WDTCSR = (timeout << WDP0) | (1<<WDE);
	
	if(int_enable != 0) {
		sei();
	}
}