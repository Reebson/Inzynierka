/*
 * GccApplication2.c
 *
 * Created: 18.01.2022 10:36:54
 * Author : piotr
 */ 

#define F_CPU 16000000UL

#include <stdint.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <stdlib.h>
#include <string.h>

#include "ff.h"
#include "diskio.h"
#include "i2c.h"
#include "LCD_HD44780_IIC.h"
#include "bme280.h"
#include "Menu.h"
#include "USART.h"
#include "watchdog.h"
#include "rtc.h"

//---------------------------------------------------------------------------------------//
//----------------------------DEFINITIONS------------------------------------------------//
//---------------------------------------------------------------------------------------//

#define VREF 5
#define PRESKALER 128 // wartosc preskalera adc
#define DIODE (1<<PD4)

#define keyLeft (1<<PB0)
#define keyRight (1<<PB1)
#define keyUp (1<<PD5)
#define keyDown (1<<PD6)
#define keyEnter (1<<PD7)

#define Out_1 (1<<PC0)
#define Out_2 (1<<PC1)
#define Out_3 (1<<PD3)
#define Out_4 (1<<PC3)

//---------------------------------------------------------------------------------------//
//----------------------------FUNCTIONS--------------------------------------------------//
//---------------------------------------------------------------------------------------//

void sensorReading();
void ADC_init();
void dataSave();
uint16_t adc_read(uint8_t ch);
int convertToPercentages(int x);
void removeFile(FIL *Fil, FRESULT *fr);
void saveData(FIL *Fil, FRESULT *fr, UINT *bw, rtct *rtc);
void writeData(char* pdata, char* parameter);
void sendDataToPhone();
int convertADCtoLux(int adc);

void buttonDown();
void buttonUp();
void buttonEnter();
void buttonLeft();
void buttonRight();
void timerDisplay(rtct *rtc);

void outputEnable_1(int hysteresis, int adc);		// sterowanie pompa
void outputEnable_2(int hysteresis, int adc);		// sterowanie oswietleniem
void outputEnable_3(rtct *rtc, int timeToActivationOut_1);						// sterowanie silownikiem i wentylatorem

uint16_t adc_result0, adc_result1;
uint16_t result;
volatile int lightFlag = 0;
volatile char usartChar;


FATFS FatFs;		/* FatFs work area needed for each volume */
FIL Fil;			/* File object needed for each open file */

typedef char string[20];


//---------------------------------------------------------------------------------------//
//----------------------------------------MAIN-------------------------------------------//
//---------------------------------------------------------------------------------------//

int main(void)
{
	rtct rtc;
	UINT bw;
	//UINT br;
	FRESULT fr;
	char txt[10];
	memset(txt, '\0', 10);
	//FSIZE_t size = 0;
	
	DDRD |= (1<<PD4); //wyjscie dodatkowe
	
	DDRC |= Out_1; //wyjscie 230V			wiatrak
	DDRC |= Out_2; //wyjscie 230V			oswietlenie
	DDRD |= Out_3; //wyjscie 12V dolne		pompa wody
	DDRC |= Out_4; //wyjscie 12V gorne		silownik

	PORTB |= keyLeft;
	PORTB |= keyRight;
	PORTD |= keyUp;
	PORTD |= keyDown;
	PORTD |= keyEnter;

	//SENSORS
	readingParameters.temp = 0; //przypisac do czujnikow
	readingParameters.hum = 0;
	readingParameters.press = 0;
	readingParameters.wg = 0;
	readingParameters.foto = 0;

	thresholdsParameters.temp = 25; //progi
	thresholdsParameters.wg = 50;
	thresholdsParameters.foto = 50;
	thresholdsParameters.timeToActivationOut_1 = 30;
	thresholdsParameters.hysteresis = 3;
	
	ADC_init();
	USART_Init();
	LCDinit();
	RTCinit();
	LCDhome();
	
	//RTC
	rtc.hour = 0x09; //  10:40:20 am
	rtc.min =  0x27;
	rtc.sec =  0x00;

	rtc.date = 0x04; //1st Jan 2016
	rtc.month = 0x08;
	rtc.year = 0x2;
	rtc.weekDay = 4; // Friday
	//RTC_SetDateTime(&rtc);  //  10:40:20 am, 1st Jan 2016
	rtct new_rtc = rtc;
	
	sei();

	//displayStartInformation();
	displayMenu(MENU_START,UpDownFlag, 0);	//start menu, indicator=1*///
	
	f_mount(&FatFs, "", 0);		/* Give a work area to the default drive */
	
	while (1) {
		buttonDown();
		buttonUp();
		buttonEnter();
		buttonLeft();
		buttonRight();
		sensorReading();
		
		if(menuFlag == 0) {
			timerDisplay(&rtc);
		}
		
		adc_result0 = adc_read(6);		// read adc value at PE2
		adc_result1 = adc_read(7);		// read adc value at PE3
		readingParameters.wg = convertToPercentages(adc_result0);
		readingParameters.foto = convertADCtoLux(adc_result0);
		
		outputEnable_1(thresholdsParameters.hysteresis, readingParameters.foto);
		outputEnable_2(thresholdsParameters.hysteresis, readingParameters.wg);
		outputEnable_3(&rtc, thresholdsParameters.timeToActivationOut_1);	

		RTC_GetDateTime(&rtc);
		if(new_rtc.hour != rtc.hour){
			saveData(&Fil, &fr, &bw, &rtc);
			new_rtc = rtc;
		}
		
		sendDataToPhone();
    }
}

void removeFile(FIL *Fil, FRESULT *fr) {
	f_unlink("temp.txt");
	*fr = f_open(Fil, "temp.txt", FA_CREATE_NEW);	/* Create a file */
	*fr = f_close(Fil);							/* Close the file */
}

void saveData(FIL *Fil, FRESULT *fr, UINT *bw, rtct *rtc) {
	FSIZE_t size = 0;
	char data[50];
	char* pdata = data;
	char parameter[10];
	rtct nrtc = *rtc;
	
	*fr = f_open(Fil, "temp.txt", FA_WRITE);
	
	if (*fr == FR_OK) {
		itoa(readingParameters.temp, parameter, 10);			//zapis temp
		strcpy(pdata, parameter);
		while(*pdata != '\0') {
			pdata++;
		}
		*pdata = ' ';
		pdata++;
		itoa(readingParameters.hum, parameter, 10);				//zapis wilgotnosci
		strcpy(pdata, parameter);
		while(*pdata != '\0') {
			pdata++;
		}
		*pdata = ' ';
		pdata++;
		itoa(readingParameters.press, parameter, 10);			//zapis cisnienia
		strcpy(pdata, parameter);
		while(*pdata != '\0') {
			pdata++;
		}
		*pdata = ' ';
		pdata++;
		itoa(readingParameters.wg, parameter, 10);				//zapis wilgotnosci gleby
		strcpy(pdata, parameter);
		while(*pdata != '\0') {
			pdata++;
		}
		*pdata = ' ';
		pdata++;
		itoa(readingParameters.foto, parameter, 10);			//zapis naslonecznienia
		strcpy(pdata, parameter);
		while(*pdata != '\0') {
			pdata++;
		}
		*pdata = ' ';
		pdata++;
		itoa(nrtc.hour, parameter, 10);							//zapis godzina
		strcpy(pdata, parameter);
		while(*pdata != '\0') {
			pdata++;
		}
		*pdata = ' ';
		pdata++;
		itoa(nrtc.weekDay, parameter, 10);						//zapis dzien
		strcpy(pdata, parameter);
		while(*pdata != '\0') {
			pdata++;
		}
		*pdata = ' ';
		pdata++;
		
		
		
		*pdata = '\0';
		*(pdata-1) = '\n';
		
		pdata = data;
		size = f_size(Fil);
		f_lseek (Fil, size);
		f_write(Fil, pdata, strlen(pdata), bw);	
		*fr = f_close(Fil);							
	}
}

/*void saveData(FIL *Fil, FRESULT *fr, UINT *bw) {
	FSIZE_t size = 0;
	char data[50];
	char* pdata = data;
	char parameter[10];
	int tab[] = {readingParameters.temp, readingParameters.hum, readingParameters.press, readingParameters.foto, readingParameters.wg};

	*fr = f_open(Fil, "temp.txt", "a");	
	
	if (*fr == FR_OK) {
		int i;
		for(i = 0; i < (sizeof(tab)/sizeof(tab[0])); i++) {
			itoa(tab[i], parameter, 10);
			strcpy(pdata, parameter);
			while(*pdata != '\0') {
				pdata++;
			}
			*pdata = ' ';
			pdata++;
		}
		
		*pdata = '\0';
		*(pdata-1) = '\n';
		
		pdata = data;
		size = f_size(Fil);
		f_lseek (Fil, size);
		f_write(Fil, pdata, strlen(pdata), bw);
		*fr = f_close(Fil);							
	}
}*/

/*void writeData(char* pdata, char* parameter) {
	strcpy(pdata, parameter);
	while(*pdata != '\0') {
		pdata++;
	}
	*pdata = ' ';
	pdata++;
}*/


void sensorReading() {
	Bme280CalibrationData calibrationData;
	result = bme280ReadCalibrationData(&calibrationData);

	if (result == BME280_OK) {
		Bme280Data data;
		result = bme280ReadData(BME280_OSS_1, BME280_OSS_1, BME280_OSS_1, &data, &calibrationData);

		if (result == BME280_OK) {
			readingParameters.temp = (int)data.temperatureC;
			readingParameters.hum = (int)data.humidityPercent;
			readingParameters.press = data.pressurePa/100; //readingParameters.press = (int)data.pressurePa/100; ???
		}
	}
}

void ADC_init() {
	ADCSRA |= 1<<ADEN;  //Enables ADC
	ADCSRA |= 1<<ADPS2 | 1<<ADPS1| 1<<ADPS0;  //Sets ADC Prescalar as 128, i.e. my ADC frequency is 125KHz
	ADCSRA |= 1<<ADSC ;  //Enables ADC interupt and Start the conversion //1<<ADIE
	ADMUX |= (1<<REFS0);
}

uint16_t adc_read(uint8_t ch) {
	// select the corresponding channel 0~7
	// ANDing with ’7? will always keep the value
	// of ‘ch’ between 0 and 7
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
	
	// start single convertion
	// write ’1? to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes ’0? again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	return (ADC);
}

ISR(USART0_RX_vect) {
	usartChar = UDR0;
}

int convertToPercentages(int Value) {
	return Value*0.09766; //przeliczenie wartosci WG na procenty (100/1024) = 0.09766
}

void timerDisplay(rtct *rtc)
{
	rtct nrtc = *rtc;
	RTC_GetDateTime(&nrtc);
	char s[3];
	
	//hour
	if(nrtc.hour <= 9)
	{
		lcd_locate(3,3);
		LCDstring("0", 1);
		LCDstring(itoa(nrtc.hour, s, 16), 1);
	}
	else
	{
		lcd_locate(3,3);
		LCDstring(itoa(nrtc.hour, s, 16), 2);
	}
	lcd_locate(5,3);
	LCDstring(":", 1);
	
	//min
	if(nrtc.min <= 9)
	{
		lcd_locate(6,3);
		LCDstring("0", 1);
		LCDstring(itoa(nrtc.min, s, 16), 1);
	}
	else
	{
		lcd_locate(6,3);
		LCDstring(itoa(nrtc.min, s, 16), 2);
	}
	
	lcd_locate(8,3);
	LCDstring(":", 1);
	
	//sec
	if(nrtc.sec <= 9)
	{
		lcd_locate(9,3);
		LCDstring("0", 1);
		LCDstring(itoa(nrtc.sec, s, 16), 1);
	}
	else
	{
		lcd_locate(9,3);
		LCDstring(itoa(nrtc.sec, s, 16), 2);
	}
}

void buttonDown() {
	if(bit_is_clear(PINB,0)) //keyDown (1<<PB0)
	{
		_delay_ms(120);
		UpDownFlag++;
		if(UpDownFlag > 3 && (menuFlag == MENU_START || menuFlag == MENU_OPTIONS 
							|| menuFlag == MENU_BUZZER || menuFlag == MENU_LANGUAGES))
		UpDownFlag = 1;
		if(UpDownFlag > 6 && (menuFlag == MENU_READING)) //6 wierszy
		UpDownFlag = 1;
		if(UpDownFlag > 6 && (menuFlag == MENU_THRESHOLDS)) //7 wierszy
		UpDownFlag = 1;

		displayMenu(menuFlag,UpDownFlag, 0);
	}
}

void buttonUp() {
	if(bit_is_clear(PIND,6)) // keyUp (1<<PD6)
	{
		_delay_ms(120);
		UpDownFlag--;
		if(UpDownFlag < 1 && (menuFlag == MENU_START || menuFlag == MENU_OPTIONS
								|| menuFlag == MENU_BUZZER || menuFlag == MENU_LANGUAGES))
		UpDownFlag = 3;
		if(UpDownFlag < 1 && (menuFlag == MENU_READING)) //6 wierszy
		UpDownFlag = 6;
		if(UpDownFlag < 1 && (menuFlag == MENU_THRESHOLDS)) //7 wierszy
		UpDownFlag = 6;

		displayMenu(menuFlag,UpDownFlag, 0);
	}
}

void buttonEnter() {
	if(bit_is_clear(PIND,7)) //keyEnter (1<<PD7)
	{
		_delay_ms(120);
		if(menuFlag == MENU_START)
		{
			menuFlag = UpDownFlag;
			UpDownFlag = 1;
			displayMenu(menuFlag,UpDownFlag, 0);
		}
		else if(menuFlag == MENU_READING)
		{
			if(UpDownFlag == 6)	// back to start menu
			{
				menuFlag = MENU_START;
				UpDownFlag = 1;
				displayMenu(menuFlag,UpDownFlag, 0);
			}
		}
		else if(menuFlag == MENU_THRESHOLDS)
		{
			if(UpDownFlag == 6)	// back to start menu
			{
				menuFlag = MENU_START;
				UpDownFlag = 1;
				displayMenu(menuFlag,UpDownFlag, 0);
			}
		}
		else if(menuFlag == MENU_OPTIONS)
		{
			if(UpDownFlag == 1) {
				menuFlag = MENU_BUZZER;
				UpDownFlag = 1;
				displayMenu(menuFlag, UpDownFlag, 0);
			}
			if(UpDownFlag == 2) {
				menuFlag = MENU_LANGUAGES;
				UpDownFlag = 1;
				displayMenu(menuFlag, UpDownFlag, 0);
			}
			if(UpDownFlag == 3)	//back to start menu
			{
				menuFlag = MENU_START;
				UpDownFlag = 1;
				displayMenu(menuFlag,UpDownFlag, 0);
			}
		}
		else if(menuFlag == MENU_BUZZER) {
			if(UpDownFlag == 1) {
				displayMenu(menuFlag, UpDownFlag, ON);
			}
			if(UpDownFlag == 2) {
				displayMenu(menuFlag, UpDownFlag, OFF);
			}
			if(UpDownFlag == 3)	//back to start menu
			{
				menuFlag = MENU_OPTIONS;
				UpDownFlag = 1;
				displayMenu(menuFlag,UpDownFlag, 0);
			}
		}
		else if(menuFlag == MENU_LANGUAGES) {
			if(UpDownFlag == 1) {
				displayMenu(menuFlag, UpDownFlag, POL);
			}
			if(UpDownFlag == 2) {
				displayMenu(menuFlag, UpDownFlag, ENG);
			}
			if(UpDownFlag == 3)	//back to start menu
			{
				menuFlag = MENU_OPTIONS;
				UpDownFlag = 1;
				displayMenu(menuFlag,UpDownFlag, 0);
			}
		}
	}
}

void buttonLeft() {
	if(bit_is_clear(PIND,5)) //keyLeft (1<<PD5)
	{
		_delay_ms(120);
		if(menuFlag == MENU_THRESHOLDS)
		displayMenu(menuFlag,UpDownFlag,MINUS);
	}
}

void buttonRight() {
	if(bit_is_clear(PINB,1)) //keyRight (1<<PB1)
	{
		_delay_ms(120);
		if(menuFlag == MENU_THRESHOLDS)
		displayMenu(menuFlag,UpDownFlag,PLUS);
	}
}


void outputEnable_1(int hysteresis, int luxValue) {				// sterowanie oswietleniem PC0 //wczesniej bylo int adc i w warunku adc < tresholds
	
	if(lightFlag == 0 ) {								//flaga = 0, wchodzimy w if
		if(luxValue < thresholdsParameters.foto) {			//jezeli przypisana wartosc jest mniejsza lub rowna ustawionemu progowi 
			lightFlag = 1;									//zmien wartosc flagi
			PORTD |= Out_2;									//zapal swiatlo
			//_delay_ms(100);
		}
	}
	else { //lightFlag = 1									//jezeli swiatlo juz sie pali
		if(luxValue > thresholdsParameters.foto + hysteresis) {	//jezeli aktualna wartosc foto jest wieksza od progu + histereza
			lightFlag = 0;									//zeruj flage
			PORTD &= ~Out_2;								//zgas swiatlo
		}
	}
} 

void outputEnable_2(int hysteresis, int adc) {		    	// sterowanie pompa wody
	
	if(adc > thresholdsParameters.wg + hysteresis) {	//jezeli odczytana wartosc jest mniejsza niz ustawiona - histereza
		PORTD |= Out_3;									//zalacz pompe
	}
	else if (adc < thresholdsParameters.wg - hysteresis) {	//lej wode dopoki odczytana wartosc bedzie rowna ustawionej + histereza
		PORTD &= ~Out_3;
	}
}

void outputEnable_3(rtct *rtc, int timeToActivationOut_1) {
	int temp = 0;
	int minutes = rtc->min; //static
	
	if(readingParameters.temp > thresholdsParameters.temp) {
		PORTC |= Out_4;															//zalacz silownik
		temp = 1;																//jezeli silownik dziala ustaw flage na 1														//minuta wlaczenia silownika jest przypisana do zmiennej
		if(rtc->min == minutes + timeToActivationOut_1 && temp == 1 && readingParameters.temp > thresholdsParameters.temp) {	
			//jezeli silownik dziala, temperatura dalej jest wieksza niz prog i aktualna minuta jest rowna poprzedniej zwiekszonej o ustawiony czas zadzialania
			PORTC |= Out_1;														//zalacz wentylator
			minutes = rtc->min;
			if(minutes >= 50)
				minutes = minutes - 60;
		}
	}
	else 																		//jezeli temperatura jest mniejsza niz prog															
		temp = 0;																//flaga ustawiona na 0
}

void sendDataToPhone() {
	char buff[4];
	
	if(languagesFlag == POL) {
		if(usartChar == '3') {
			UART_SendString("Dane z czujnikow\n");
			UART_SendString("Temperatura: " );
			UART_SendString(itoa(readingParameters.temp, buff, 10));
			UART_SendString(" *C");
			UART_TxChar('\n');
			UART_SendString("Wilgotnosc: ");
			UART_SendString(itoa(readingParameters.hum, buff, 10));
			UART_SendString(" %");
			UART_TxChar('\n');
			UART_SendString("Cisnienie: ");
			UART_SendString(itoa(readingParameters.press, buff, 10));
			UART_SendString(" hPa");
			UART_TxChar('\n');
			UART_SendString("Wilgotnosc_gleby: ");
			UART_SendString(itoa(readingParameters.wg, buff, 10));
			UART_SendString(" %");
			UART_TxChar('\n');
			UART_SendString("Naslonecznienie: ");
			UART_SendString(itoa(readingParameters.foto, buff, 10));
			UART_SendString(" %");
			UART_TxChar('\n');
		}
	}
	
	else if(languagesFlag == ENG) {
		if(usartChar == '3') {
			UART_SendString("Sensors reading\n");
			UART_SendString("Temperature: " );
			UART_SendString(itoa(readingParameters.temp, buff, 10));
			UART_SendString(" *C ");
			UART_TxChar('\n');
			UART_SendString("Humidity: ");
			UART_SendString(itoa(readingParameters.hum, buff, 10));
			UART_SendString(" %");
			UART_TxChar('\n');
			UART_SendString("Pressure: ");
			UART_SendString(itoa(readingParameters.press, buff, 10));
			UART_SendString(" hPa ");
			UART_TxChar('\n');
			UART_SendString("Soil moisture: ");
			UART_SendString(itoa(readingParameters.wg, buff, 10));
			UART_SendString(" %");
			UART_TxChar('\n');
			UART_SendString("Lighting: ");
			UART_SendString(itoa(readingParameters.foto, buff, 10));
			UART_SendString(" Lux");
			UART_TxChar('\n');
		}
	}
}


int convertADCtoLux(int adc) {
	
	int luxValue;
	
	if(adc >= 0 && adc <= 99)
		return luxValue = 1;
	else if(adc >= 100 && adc <= 199)
		return luxValue = 3;
	else if(adc >= 200 && adc <= 299)
		return luxValue = 5;
	else if(adc >= 300 && adc <= 399)
		return luxValue = 9;
	else if(adc >= 400 && adc <= 499)
		return luxValue = 11;
	else if(adc >= 500 && adc <= 599)
		return luxValue = 12;
	else if(adc >= 600 && adc <= 699)
		return luxValue = 13;
	else if(adc >= 700 && adc <= 799)
		return luxValue = 14;
	else if(adc >= 800 && adc <= 849)
		return luxValue = 18;
	else if(adc >= 850 && adc <= 899)
		return luxValue = 30;
	else if(adc >= 900 && adc <= 949)
		return luxValue = 40;
	else if(adc >= 950 && adc <= 999)
		return luxValue = 50;
	else if(adc >= 1000 && adc <= 1010)
		return luxValue = 60;
	else if(adc >= 1011 && adc <= 1024)
		return luxValue = 70;
}
