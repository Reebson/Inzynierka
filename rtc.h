/*
 * rtc.h
 *
 * Created: 03.06.2022 19:11:28
 *  Author: piotr
 */ 
#include <avr/io.h>

#ifndef RTC_H_
#define RTC_H_

#define Device_Write_address	0xD0	/* Define RTC DS1307 slave write address */
#define Device_Read_address	0xD1	/* Make LSB bit high of slave address for read */
#define C_Ds1307SecondRegAddress_U8   0x00u   // Address to access Ds1307 SEC register
#define C_Ds1307DateRegAddress_U8     0x04u   // Address to access Ds1307 DATE register
#define C_Ds1307ControlRegAddress_U8  0x07u   // Address to access Ds1307 CONTROL register

typedef struct
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t weekDay;
	uint8_t date;
	uint8_t month;
	uint8_t year;
}rtct;

void RTCinit();
void RTC_SetDateTime(rtct *rtc);
void RTC_GetDateTime(rtct *rtc);



#endif /* RTC_H_ */