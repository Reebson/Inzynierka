/*
 * Menu.h
 *
 * Created: 21.05.2022 19:13:51
 *  Author: piotr
 */ 

#define MENU_START		0
#define MENU_READING	1
#define MENU_THRESHOLDS	2
#define MENU_OPTIONS	3
#define MENU_BUZZER		4
#define MENU_LANGUAGES	5

#define TIME			4
#define ERROR			-1

#define MINUS			-1
#define PLUS			 1

#define ON				2
#define OFF				3

#define POL				4
#define ENG				5

void displayMenuStart();
void displayMenuSetup();
void displayStartInformation(void);
void displayReadingMenu(int indicator);
void displayMenu(int menuflag, int indicator, int change);
void displayThresholdsMenu(int indicator, int change);
void displayMenuBuzzer(int change);
void displayMenuLanguages(int change);

volatile int menuFlag = MENU_START;
volatile int UpDownFlag = 1;
volatile int leftRightFlag = 0;

int languagesFlag = POL;

typedef struct Parameters{
	int temp;
	int hum;
	int press;
	int wg;
	int foto;
	int hysteresis;
	int timeToActivationOut_1;
} parameters;

volatile parameters readingParameters;
volatile parameters thresholdsParameters;


void displayMenu(int flag, int indicator, int change) {

	int indicator_2;

	indicator--;
	indicator_2 = indicator;

	if(indicator > 3) {
		indicator = 3;
	}

	LCDclr();
	lcd_locate(0,indicator);
	LCDstring("-> ", 3); //3

	switch(flag) {
		case MENU_START:	//menuflag = 0
		{
			displayMenuStart();
		}
		break;
		case MENU_READING:	//menuflag = 1
		{
			displayReadingMenu(indicator_2);
		}
		break;
		case MENU_THRESHOLDS: //menuflag = 2
		{
			displayThresholdsMenu(indicator_2, change);
		}
		break;
		case MENU_OPTIONS: //menuflag = 3
		{
			displayMenuSetup();
		}
		break;
		case MENU_BUZZER: //menuflag = 4
		{
			displayMenuBuzzer(change);
		}
		break;
		
		case MENU_LANGUAGES: //menuflag = 5
		{
			displayMenuLanguages(change);
		}
		break;
		
		default:	//menuflag != 0,1,2,3
		{
			//ERROR -> in 2 second back to start menu
			LCDclr();
			lcd_locate(0,0);
			LCDstring("ERROR", 5);
			_delay_ms(500);
			lcd_locate(0,7);
			LCDstring(".", 1);
			_delay_ms(500);
			lcd_locate(0,8);
			LCDstring(".", 1);
			_delay_ms(500);
			lcd_locate(0,9);
			LCDstring(".", 1);
			_delay_ms(500);

			//flagi "wyzerowac"
			menuFlag = MENU_START;
			//UpDownFlag = 1;
			indicator = 1;
			displayMenu(menuFlag,indicator, 0);
		}
	}
}

void displayMenuStart() {
	if(languagesFlag == POL) {
		lcd_locate(3,0);
		LCDstring("Odczyt pomiarow", 15);	//3 - 17 bit LCD
		lcd_locate(3,1);
		LCDstring("Progi", 5);				//3 - 7 bit LCD
		lcd_locate(3,2);
		LCDstring("Ustawienia", 10);		//3 - 12 bit LCD
		lcd_locate(3,0);
	}
	
	else if(languagesFlag == ENG) {
		lcd_locate(3,0);
		LCDstring("Sensors reading", 15);	//3 - 17 bit LCD
		lcd_locate(3,1);
		LCDstring("Tresholds", 9);				//3 - 7 bit LCD
		lcd_locate(3,2);
		LCDstring("Setup", 5);		//3 - 12 bit LCD
		lcd_locate(3,0);
	}
}

void displayMenuSetup() {
	if(languagesFlag == POL) {
		lcd_locate(3,0);
		LCDstring("Buzzer", 6);		//3 - 7 bit LCD
		lcd_locate(3,1);
		LCDstring("Jezyk", 5);		//3 - 7 bit LCD
		lcd_locate(3,2);
		LCDstring("Cofnij", 6);		//3 - 8 bit LCD
	}
	
	else if(languagesFlag == ENG) {
		lcd_locate(3,0);
		LCDstring("Buzzer", 6);		//3 - 7 bit LCD
		lcd_locate(3,1);
		LCDstring("Languages", 9);		//3 - 7 bit LCD
		lcd_locate(3,2);
		LCDstring("Back", 4);		//3 - 8 bit LCD
	}
}

void displayReadingMenu(int indicator) {
	char parametertab[5]; //nie wiem jaka wielkosc tablicy, przyjalem wartosc 10 to 3 elementowa bo jeszcze znak konca lini '\0'

	switch(indicator) {
		case 0: case 1: case 2: case 3:
		{
			if(languagesFlag == POL){
				lcd_locate(2,0);
				LCDstring("Temperatura: ", 13);		//3 - 7 bit LCD
				itoa(readingParameters.temp, parametertab, 10);
				lcd_locate(15, 0);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(18,0);
				LCDstring("*C", 2);

				lcd_locate(2,1);
				LCDstring("Wilgotnosc: ", 12);		//3 - 7 bit LCD
				itoa(readingParameters.hum, parametertab, 10);
				lcd_locate(15,1);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(18,1);
				LCDstring("%", 1);

				lcd_locate(2,2);
				LCDstring("Cisnienie: ", 11);		//3 - 7 bit LCD
				itoa(readingParameters.press, parametertab, 10);
				lcd_locate(13,2);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(17,2);
				LCDstring("hPa", 3);

				lcd_locate(2,3);
				LCDstring("W. gleby:  ", 10);		//3 - 7 bit LCD
				itoa(readingParameters.wg, parametertab, 10);
				lcd_locate(15,3);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(18,3);
				LCDstring("%", 1);
			}
			
			else if(languagesFlag == ENG) {
				lcd_locate(2,0);
				LCDstring("Temperature: ", 13);		//3 - 7 bit LCD
				itoa(readingParameters.temp, parametertab, 10);
				lcd_locate(15, 0);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(18,0);
				LCDstring("*C", 2);

				lcd_locate(2,1);
				LCDstring("Humidity: ", 10);		//3 - 7 bit LCD
				itoa(readingParameters.hum, parametertab, 10);
				lcd_locate(15,1);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(18,1);
				LCDstring("%", 1);

				lcd_locate(2,2);
				LCDstring("Pressure: ", 10);		//3 - 7 bit LCD
				itoa(readingParameters.press, parametertab, 10);
				lcd_locate(13,2);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(17,2);
				LCDstring("hPa", 3);

				lcd_locate(2,3);
				LCDstring("Soil moisture:  ", 15);		//3 - 7 bit LCD
				itoa(readingParameters.wg, parametertab, 10);
				lcd_locate(16,3);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(19,3);
				LCDstring("%", 1);
			}
		}
		break;
		case 4:
		{
			if(languagesFlag == POL) {
				lcd_locate(2,0);
				LCDstring("Wilgotnosc: ", 12);		//3 - 7 bit LCD
				itoa(readingParameters.hum, parametertab, 10);
				lcd_locate(15,0);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(18,0);
				LCDstring("%", 0);

				lcd_locate(2,1);
				LCDstring("Cisnienie: ", 11);		//3 - 7 bit LCD
				itoa(readingParameters.press, parametertab, 10);
				lcd_locate(13,1);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(17,1);
				LCDstring("hPa", 3);
			
				lcd_locate(2,2);
				LCDstring("W. gleby:  ", 10);		//3 - 7 bit LCD
				itoa(readingParameters.wg, parametertab, 10);
				lcd_locate(15,2);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(18,2);
				LCDstring("%", 1);

				lcd_locate(2,3);
				LCDstring("Swiatlo: ", 9);		//3 - 7 bit LCD
				itoa(readingParameters.foto, parametertab, 10);
				lcd_locate(13,3);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(17,3);
				LCDstring("Lux", 3);
			}
			
			else if(languagesFlag == ENG){
				lcd_locate(2,0);
				LCDstring("Humidity: ", 10);		//3 - 7 bit LCD
				itoa(readingParameters.hum, parametertab, 10);
				lcd_locate(15,0);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(18,0);
				LCDstring("%", 1);

				lcd_locate(2,1);
				LCDstring("Pressure: ", 10);		//3 - 7 bit LCD
				itoa(readingParameters.press, parametertab, 10);
				lcd_locate(13,1);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(17,1);
				LCDstring("hPa", 3);

				lcd_locate(2,2);
				LCDstring("Soil moisture:  ", 15);		//3 - 7 bit LCD
				itoa(readingParameters.wg, parametertab, 10);
				lcd_locate(16,2);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(19,2);
				LCDstring("%", 1);
			
				lcd_locate(2,3);
				LCDstring("Lighting: ", 10);		//3 - 7 bit LCD
				itoa(readingParameters.foto, parametertab, 10);
				lcd_locate(13,3);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(17,3);
				LCDstring("Lux", 3);
			}
		}
		break;
		case 5:
		{
			if(languagesFlag == POL) {
				lcd_locate(2,0);
				LCDstring("Cisnienie: ", 11);		//3 - 7 bit LCD
				itoa(readingParameters.press, parametertab, 10);
				lcd_locate(13,0);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(17,0);
				LCDstring("hPa", 3);

				lcd_locate(2,1);
				LCDstring("W. gleby:  ", 10);		//3 - 7 bit LCD
				itoa(readingParameters.wg, parametertab, 10);
				lcd_locate(15,1);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(18,1);
				LCDstring("%", 1);

				lcd_locate(2,2);
				LCDstring("Swiatlo: ", 9);		//3 - 7 bit LCD
				itoa(readingParameters.foto, parametertab, 10);
				lcd_locate(13,2);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(17,2);
				LCDstring("Lux", 3);

				lcd_locate(2,3);
				LCDstring("Cofnij", 6);		//3 - 8 bit LCD
			}
			
			else if(languagesFlag == ENG){
				lcd_locate(2,0);
				LCDstring("Pressure: ", 10);		//3 - 7 bit LCD
				itoa(readingParameters.press, parametertab, 10);
				lcd_locate(13,0);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(17,0);
				LCDstring("hPa", 3);

				lcd_locate(2,1);
				LCDstring("Soil moisture:  ", 15);		//3 - 7 bit LCD
				itoa(readingParameters.wg, parametertab, 10);
				lcd_locate(16,1);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(19,1);
				LCDstring("%", 1);
			
				lcd_locate(2,2);
				LCDstring("Lighting: ", 10);		//3 - 7 bit LCD
				itoa(readingParameters.foto, parametertab, 10);
				lcd_locate(13,2);			//9 - 11
				LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
				lcd_locate(17,2);
				LCDstring("Lux", 3);
			
				lcd_locate(2,3);
				LCDstring("Back", 4);		//3 - 8 bit LCD
			}
		}
		break;
		default:
		// co w przypadku bledu
		break;
	}
}

void displayThresholdsMenu(int indicator, int change) {
	char parametertab[10]; //nie wiem jaka wielkosc tablicy, przyjolem wartosc 10 to 3 elementowa bo jeszcze znak konca lini '\0'

	//display_indicator(indicator);

	switch(indicator)
	{
		case 0: case 1: case 2: case 3:
		{
			if(languagesFlag == POL) {
				lcd_locate(3,0);
				LCDstring("Pompa wody: ", 12);		//3 - 7 bit LCD

				lcd_locate(3,1);
				LCDstring("Silowik: ", 9);		//3 - 7 bit LCD

				lcd_locate(3,2);
				LCDstring("Oswietlenie: ", 13);		//3 - 7 bit LCD

				lcd_locate(3,3);
				LCDstring("Wentylator: ", 12);		//3 - 7 bit LCD
			}
			
			else if(languagesFlag == ENG) {
				lcd_locate(3,0);
				LCDstring("Water pump: ", 12);		//3 - 7 bit LCD

				lcd_locate(3,1);
				LCDstring("Actuator: ", 10);		//3 - 7 bit LCD

				lcd_locate(3,2);
				LCDstring("Lighting: ", 10);		//3 - 7 bit LCD

				lcd_locate(3,3);
				LCDstring("Ventilator: ", 12);		//3 - 7 bit LCD
			
		}
		break;
		case 4:
		{
			if(languagesFlag == POL){
				lcd_locate(3,0);
				LCDstring("Silownik: ", 9);		//3 - 7 bit LCD

				lcd_locate(3,1);
				LCDstring("Oswietlenie: ", 13);		//3 - 7 bit LCD

				lcd_locate(3,2);
				LCDstring("Wentylator: ", 12);		//3 - 7 bit LCD
			
				lcd_locate(3,3);
				LCDstring("Histereza: ", 10);		//3 - 7 bit LCD
			}
			
			else if(languagesFlag == ENG){
				lcd_locate(3,0);
				LCDstring("Actuator: ", 10);		//3 - 7 bit LCD

				lcd_locate(3,1);
				LCDstring("Lighting: ", 10);		//3 - 7 bit LCD

				lcd_locate(3,2);
				LCDstring("Ventilator: ", 12);		//3 - 7 bit LCD
			
				lcd_locate(3,3);
				LCDstring("Histeresis: ", 12);		//3 - 7 bit LCD
			}
		}
		break;
		case 5:
		{
			if(languagesFlag == POL){
				lcd_locate(3,0);
				LCDstring("Oswietlenie: ", 13);		//3 - 7 bit LCD

				lcd_locate(3,1);
				LCDstring("Wentylator: ", 12);		//3 - 7 bit LCD
			
				lcd_locate(3,2);
				LCDstring("Histereza: ", 10);		//3 - 7 bit LCD

				lcd_locate(3,3);
				LCDstring("Cofnij: ", 8);		//3 - 7 bit LCD
			}
			
			else if(languagesFlag == ENG){
				lcd_locate(3,0);
				LCDstring("Lighting: ", 10);		//3 - 7 bit LCD

				lcd_locate(3,1);
				LCDstring("Ventilator: ", 12);		//3 - 7 bit LCD
			
				lcd_locate(3,2);
				LCDstring("Histeresis: ", 12);		//3 - 7 bit LCD
			
				lcd_locate(3,3);
				LCDstring("Back: ", 6);		//3 - 7 bit LCD
			}
		}
		break;
		default:
		// co w przypadku bledu
		break;
	}
	}

	switch(indicator)
	{
		case 0:
		{
			thresholdsParameters.wg = thresholdsParameters.wg + change;
			itoa(thresholdsParameters.wg, parametertab, 10);
			lcd_locate(17,0);
			LCDstring(parametertab, strlen(parametertab));		//8 - 10 bit LCD
		}break;
		
		case 1:
		{
			thresholdsParameters.temp = thresholdsParameters.temp + change;
			itoa(thresholdsParameters.temp, parametertab, 10);
			lcd_locate(17,1);
			LCDstring(parametertab, strlen(parametertab));		//8 - 10 bit LCD
		}break;
		
		
		case 2:
		{
			thresholdsParameters.foto = thresholdsParameters.foto + change;
			itoa(thresholdsParameters.foto, parametertab, 10);
			lcd_locate(17,2);
			LCDstring(parametertab, strlen(parametertab));		//8 - 10 bit LCD
		}break;	
		
		case 3:
		{
			thresholdsParameters.timeToActivationOut_1 = thresholdsParameters.timeToActivationOut_1 + change;
			itoa(thresholdsParameters.timeToActivationOut_1, parametertab, 10);
			lcd_locate(17,3);
			LCDstring(parametertab, strlen(parametertab));		//8 - 10 bit LCD
		}break;
		
		case 4:
		{
			thresholdsParameters.hysteresis = thresholdsParameters.hysteresis + change;
			itoa(thresholdsParameters.hysteresis, parametertab, 10);
			lcd_locate(17,3);
			LCDstring(parametertab, strlen(parametertab));		//8 - 10 bit LCD
		}break;
		
		case 5:
		{
			//nic nie robic bo tu jest cofnij
		}break;

		default:
		//co w przypadku bledu
		break;
	}
}

void displayMenuBuzzer(int change) {
	
	if(change == ON)
		 PORTD |= (1<<PD4);		//funkcja
	 if(change == OFF)			//funkcja
		 PORTD &= ~(1<<PD4);
	 
	lcd_locate(3,0);
	LCDstring("ON", 2);		//3 - 7 bit LCD
	lcd_locate(3,1);
	LCDstring("OFF", 3);		//3 - 7 bit LCD
	lcd_locate(3,2);
	LCDstring("Cofnij", 6);		//3 - 8 bit LCD
}

void displayMenuLanguages(int change) {
	
	if(change == POL)
		languagesFlag = POL;
	if(change == ENG)
		languagesFlag = ENG;
	
	if(languagesFlag == POL) {
		lcd_locate(3,0);
		LCDstring("POL", 3);		//3 - 7 bit LCD
		lcd_locate(3,1);
		LCDstring("ENG", 3);		//3 - 7 bit LCD
		lcd_locate(3,2);
		LCDstring("Cofnij", 6);		//3 - 8 bit LCD
	}
	
	else if(languagesFlag == ENG) {
		lcd_locate(3,0);
		LCDstring("POL", 3);		//3 - 7 bit LCD
		lcd_locate(3,1);
		LCDstring("ENG", 3);		//3 - 7 bit LCD
		lcd_locate(3,2);
		LCDstring("Back", 4);		//3 - 8 bit LCD
	}
}

/*void soilMoistureCalibration(int change) {
	
	if(change == DONE)
		readingParameters.wg = 100;
	if(change == NOT_DONE)
		
	
	lcd_locate(0,0);
	LCDstring("Podlej rosline", 18);
	lcd_locate(3,1);
	LCDstring("Podlana", 7);
	lcd_locate(3,2);
	LCDstring("Nie podlana", 11);
}*/

void displayStartInformation(void)
{
	if(languagesFlag == POL) {
		lcd_locate(4,0);
		LCDstring("Sterownik do", 12);
		lcd_locate(4,1);
		LCDstring("monitorowania", 13);
		lcd_locate(2,2);
		LCDstring("warunkow wzrostu", 16);
		lcd_locate(7,3);
		LCDstring("roslin", 6);
		_delay_ms(3000);
		LCDclr();
		lcd_locate(4,0);
		LCDstring("Wykonal:", 8);
		lcd_locate(4,1);
		LCDstring("Piotr Karpik", 12);
		_delay_ms(2000);
		LCDclr();
	}
	
	else if(languagesFlag == ENG) {
		lcd_locate(1,0);
		LCDstring("Electronis system", 17);
		lcd_locate(2,1);
		LCDstring("for controlling", 15);
		lcd_locate(4,2);
		LCDstring("plant growth", 12);
		lcd_locate(4,3);
		LCDstring("conditions", 10);
		_delay_ms(3000);
		LCDclr();
		lcd_locate(4,0);
		LCDstring("Author:", 7);
		lcd_locate(4,1);
		LCDstring("Piotr Karpik", 12);
		_delay_ms(2000);
		LCDclr();
	}
}

