/*
 * watchdog.h
 *
 * Created: 30.05.2022 13:58:20
 *  Author: piotr
 */ 


#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define wdr() __asm__ __volatile__("wdr")

typedef enum{
	wdt_timeout_16ms,
	wdt_timeout_32ms,
	wdt_timeout_64ms,
	wdt_timeout_125ms,
	wdt_timeout_250ms,
	wdt_timeout_500ms,
	wdt_timeout_1s,
	wdt_timeout_2s,
	wdt_timeout_4s,
	wdt_timeout_8s,
}wdt_timeout_e;

void WDT_off(uint8_t int_enable);
void WDT_enable();
void WDT_Prescaler_Change(uint8_t int_enable, wdt_timeout_e timeout);

#endif /* WATCHDOG_H_ */