/*
 * rtc.c
 *
 * Created: 03.06.2022 19:10:26
 *  Author: piotr
 */ 
#include "i2c.h"
#include "rtc.h"

void RTCinit() {
	i2cInit();
	i2cStart();
	i2cSendData(Device_Write_address);
	i2cSendData(C_Ds1307ControlRegAddress_U8);
	i2cSendData(C_Ds1307SecondRegAddress_U8 );
	i2cStop();
}

void RTC_SetDateTime(rtct *rtc) {
	i2cStart();
	i2cSendData(Device_Write_address);
	i2cSendData(C_Ds1307SecondRegAddress_U8 );
	i2cSendData(rtc->sec);
	i2cSendData(rtc->min);
	i2cSendData(rtc->hour);
	i2cSendData(rtc->weekDay);
	i2cSendData(rtc->date);
	i2cSendData(rtc->month);
	i2cSendData(rtc->year);
	
	
	i2cStop();
}

void RTC_GetDateTime(rtct *rtc) {
	i2cStart();
	i2cSendData(Device_Write_address);
	i2cSendData(C_Ds1307SecondRegAddress_U8 );
	i2cStop();
	i2cStart();
	i2cSendData(Device_Read_address);
	
	rtc->sec = i2cReadDataAck();
	rtc->min = i2cReadDataAck();
	rtc->hour = i2cReadDataAck();
	rtc->weekDay = i2cReadDataAck();
	rtc->date = i2cReadDataAck();
	rtc->month = i2cReadDataAck();
	rtc->year = i2cReadDataNotAck();
	
	i2cStop();
}
