//DCF-Uhr mit LCD20x4
//ATmega16 @ 16 MHz
//Autor: Johannes Maslowski
//Version 2010-02-27-02-52
//AVR-Studio 4.18 Build 692, WinAVR 20100110

//Header Dateien
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "lcd-routines.h"

/////////////
//Variablen//
/////////////
//date
uint8_t week_day;
uint8_t day;
uint8_t month;
uint16_t year;

//time
uint16_t millisecond;
uint8_t second;
uint8_t minute;
uint8_t hour;
uint8_t timezone;

//Speicherbereich für DCF-Pegel
uint8_t dataPC;

//Pegel-Auswertung
uint16_t counter_high;
uint16_t counter_low;
uint8_t bit_number;

//dcf_date
uint8_t dcf_week_day;
uint8_t dcf_day;
uint8_t dcf_month;
uint8_t dcf_year;
uint8_t dcf_date_parity;

//dcf_time
uint8_t dcf_second;
uint8_t dcf_minute;
uint8_t dcf_minute_parity;
uint8_t dcf_hour;
uint8_t dcf_hour_parity;
uint8_t dcf_timezone;

//Puffer für die int --> char Umwandlung
char itoa_buffer[8];

//Array für die DCF-Datenbits
//uint8_t minute_frame[60];	//wird noch nicht genutzt

//Schaltjahr
uint8_t leapyear;

////////////////////
//Reset-Funktionen//
////////////////////
void counter_reset(void)
{
	counter_high=0;
	counter_low=0;
	bit_number=0;
}

void dcf_date_reset(void)
{
	dcf_week_day=0;
	dcf_day=0;
	dcf_month=0;
	dcf_year=0;
	dcf_date_parity=0;
}

void dcf_time_reset(void)
{
	dcf_minute=0;
	dcf_minute_parity=0;
	dcf_hour=0;
	dcf_hour_parity=0;
	dcf_timezone=0;
}

/////////////
//Sonstiges//
/////////////
//Eine führende 0 ergänzen bei Zahlen kleiner/gleich 9
void leading_0(uint8_t digit)
{
	if(digit<=9)
	{
		lcd_data('0');
	}
}

////////////////
//LCD-Ausgaben//
////////////////
//Zusammengeführtes Datum
void show_matched_date(uint8_t column,uint8_t row)	//Übergabe der Cursorposition (Spalte,Zeile)
{
	set_cursor(column,row);

	//LCD-Ausgabe: Wochentag
	switch(week_day)
	{
		case 1: lcd_string("Mo ");break;	//LCD-Ausgabe: Montag
		case 2: lcd_string("Di ");break;	//LCD-Ausgabe: Dienstag
		case 3: lcd_string("Mi ");break;	//LCD-Ausgabe: Mittwoch
		case 4: lcd_string("Do ");break;	//LCD-Ausgabe: Donnerstag
		case 5: lcd_string("Fr ");break;	//LCD-Ausgabe: Freitag
		case 6: lcd_string("Sa ");break;	//LCD-Ausgabe: Samstag
		case 7: lcd_string("So ");break;	//LCD-Ausgabe: Sonntag
		default: lcd_string("   ");break;
	}

	//LCD-Ausgabe: Tag
	leading_0(day);
	itoa(day,itoa_buffer,10);
	lcd_string(itoa_buffer);
	lcd_data('.');

	//LCD-Ausgabe: Monat
	leading_0(month);
 	itoa(month,itoa_buffer,10);
	lcd_string(itoa_buffer);
	lcd_string(".");

	//LCD-Ausgabe: Jahreszahl
	leading_0(year);
 	itoa(year,itoa_buffer,10);
	lcd_string(itoa_buffer);
}

//Zusammengeführte Zeit
void show_matched_time(uint8_t column,uint8_t row)	//Übergabe der Cursorposition (Spalte,Zeile)
{
	set_cursor(column,row);

	//LCD-Ausgabe: Stunde
	leading_0(hour);
	itoa(hour,itoa_buffer,10);
	lcd_string(itoa_buffer);
	lcd_data(':');

	//LCD-Ausgabe: Minute
	leading_0(minute);
 	itoa(minute,itoa_buffer,10);
	lcd_string(itoa_buffer);
	lcd_data(':');

	//LCD-Ausgabe: Sekunde
	leading_0(second);
 	itoa(second,itoa_buffer,10);
	lcd_string(itoa_buffer);
	lcd_data(' ');

	//LCD-Ausgabe: Zeitzone
	switch(timezone)
	{
		case 0: lcd_string(" MEZ ");break;	//Ausgabe: Mitteleuropäische Zeit
		case 2: lcd_string(" MESZ");break;	//Ausgabe: Mitteleuropäische Sommerzeit
		default: lcd_string(" NOTZ");break;	//"No timezone"(NOTZ): Bitfehler bei der Auswertung der Zeitzone
	}
}

///////////////////////
//DCF-Synchronisation//
///////////////////////
//High-Pegel muß für 1700-1900ms anliegen
//Danach muß ein Low-Pegel folgen
void dcf_sync(void)
{
//	show_time(0,2);
	show_matched_date(0,1);
	show_matched_time(0,2);

	//LCD-Ausgabe: Synchronisation läuft gerade
	set_cursor(19,1);
	lcd_data('?');

	if(!(dataPC & (1<<PINC1)))
	{
		if(counter_high>=1750 && counter_high<=1950)
		{
			//LCD-Ausgabe: Synchronisation erfolgreich
			set_cursor(19,1);
			lcd_data('!');

			counter_reset();
			dcf_date_reset();
			dcf_time_reset();
		}
		else
		{
			counter_high=0;
			dcf_sync();
		}
	}
	else
	{
		dcf_sync();
	}
}

//////////////////
//DCF-Auswertung//
//////////////////
void dcf_0(void)
{
	switch(bit_number)
	{
		case 0: ;break;	//Start einer neuen Minuten, muß immer 0 sein
		case 1: ;break;	//Wetterdaten
		case 2: ;break;	//Wetterdaten
		case 3: ;break;	//Wetterdaten
		case 4: ;break;	//Wetterdaten
		case 5: ;break;	//Wetterdaten
		case 6: ;break;	//Wetterdaten
		case 7: ;break;	//Wetterdaten
		case 8: ;break;	//Wetterdaten
		case 9: ;break;	//Wetterdaten
		case 10: ;break;	//Wetterdaten
		case 11: ;break;	//Wetterdaten
		case 12: ;break;	//Wetterdaten
		case 13: ;break;	//Wetterdaten
		case 14: ;break;	//Wetterdaten
		case 15: ;break;	//Rufbit für PTB
		case 16: ;break; //Keine Änderung der Zeitzone
		case 17: ;break;	//Zeitzone: dcf_timezone=dcf_timezone
		case 18: dcf_timezone+=1;break;	//Zeitzone: 
		case 19: ;break;	//keine Schaltsekunde
		case 20: dcf_sync();break;	//Beginn der Zeitinformation (immer 1)
		case 21: ;break;
		case 22: ;break;
		case 23: ;break;
		case 24: ;break;
		case 25: ;break;
		case 26: ;break;
		case 27: ;break;
		case 28: ;break;	//Parität: Minute
		case 29: ;break;
		case 30: ;break;
		case 31: ;break;
		case 32: ;break;
		case 33: ;break;
		case 34: ;break;
		case 35: ;break;	//Parität: Stunde
		case 36: ;break;
		case 37: ;break;
		case 38: ;break;
		case 39: ;break;
		case 40: ;break;
		case 41: ;break;
		case 42: ;break;
		case 43: ;break;
		case 44: ;break;
		case 45: ;break;
		case 46: ;break;
		case 47: ;break;
		case 48: ;break;
		case 49: ;break;
		case 50: ;break;
		case 51: ;break;
		case 52: ;break;
		case 53: ;break;
		case 54: ;break;
		case 55: ;break;
		case 56: ;break;
		case 57: ;break;
		case 58: ;break;	//Parität: Datum
		case 59: dcf_sync();break;	//keine Absenkung --> neue Minute
		default: dcf_sync();break;
	}
}

void dcf_1(void)
{
	switch(bit_number)
	{
		case 0: dcf_sync();break;	//Start einer neuen Minute, muß immer 0 sein
		case 1: ;break;	//Wetterdaten
		case 2: ;break;	//Wetterdaten
		case 3: ;break;	//Wetterdaten
		case 4: ;break;	//Wetterdaten
		case 5: ;break;	//Wetterdaten
		case 6: ;break;	//Wetterdaten
		case 7: ;break;	//Wetterdaten
		case 8: ;break;	//Wetterdaten
		case 9: ;break;	//Wetterdaten
		case 10: ;break;	//Wetterdaten
		case 11: ;break;	//Wetterdaten
		case 12: ;break;	//Wetterdaten
		case 13: ;break;	//Wetterdaten
		case 14: ;break;	//Wetterdaten
		case 15: ;break;	//Rufbit für PTB
		case 16: ;break; //Am Ende dieser Stunde wird auf MEZ/MESZ umgestellt
		case 17: dcf_timezone+=1;break;	//Zeitzone: 
		case 18: ;break;	//Zeitzone: dcf_timezone=dcf_timezone
		case 19: ;break;	//Am Ende diese Stunde wird eine Schaltsekunde eingefügt.
		case 20: ;break;	//Beginn der Zeitinformation (immer 1)
		case 21: dcf_minute+=1;dcf_minute_parity+=1;break;
		case 22: dcf_minute+=2;dcf_minute_parity+=1;break;
		case 23: dcf_minute+=4;dcf_minute_parity+=1;break;
		case 24: dcf_minute+=8;dcf_minute_parity+=1;break;
		case 25: dcf_minute+=10;dcf_minute_parity+=1;break;
		case 26: dcf_minute+=20;dcf_minute_parity+=1;break;
		case 27: dcf_minute+=40;dcf_minute_parity+=1;break;
		case 28: dcf_minute_parity+=1;break;	//Parität: Minute
		case 29: dcf_hour+=1;dcf_hour_parity+=1;break;
		case 30: dcf_hour+=2;dcf_hour_parity+=1;break;
		case 31: dcf_hour+=4;dcf_hour_parity+=1;break;
		case 32: dcf_hour+=8;dcf_hour_parity+=1;break;
		case 33: dcf_hour+=10;dcf_hour_parity+=1;break;
		case 34: dcf_hour+=20;dcf_hour_parity+=1;break;
		case 35: dcf_hour_parity+=1;break;	//Parität: Stunde
		case 36: dcf_day+=1;dcf_date_parity+=1;break;
		case 37: dcf_day+=2;dcf_date_parity+=1;break;
		case 38: dcf_day+=4;dcf_date_parity+=1;break;
		case 39: dcf_day+=8;dcf_date_parity+=1;break;
		case 40: dcf_day+=10;dcf_date_parity+=1;break;
		case 41: dcf_day+=20;dcf_date_parity+=1;break;
		case 42: dcf_week_day+=1;dcf_date_parity+=1;break;
		case 43: dcf_week_day+=2;dcf_date_parity+=1;break;
		case 44: dcf_week_day+=4;dcf_date_parity+=1;break;
		case 45: dcf_month+=1;dcf_date_parity+=1;break;
		case 46: dcf_month+=2;dcf_date_parity+=1;break;
		case 47: dcf_month+=4;dcf_date_parity+=1;break;
		case 48: dcf_month+=8;dcf_date_parity+=1;break;
		case 49: dcf_month+=10;dcf_date_parity+=1;break;
		case 50: dcf_year+=1;dcf_date_parity+=1;break;
		case 51: dcf_year+=2;dcf_date_parity+=1;break;
		case 52: dcf_year+=4;dcf_date_parity+=1;break;
		case 53: dcf_year+=8;dcf_date_parity+=1;break;
		case 54: dcf_year+=10;dcf_date_parity+=1;break;
		case 55: dcf_year+=20;dcf_date_parity+=1;break;
		case 56: dcf_year+=40;dcf_date_parity+=1;break;
		case 57: dcf_year+=80;dcf_date_parity+=1;break;
		case 58: dcf_date_parity+=1;break;	//Parität: Datum
		case 59: dcf_sync();break;	//keine Absenkung --> neue Minute
		default: dcf_sync();break;
	}
}
/////////////////////////
//Berechnung Schaltjahr//
/////////////////////////
void leap_year(void)
{
	if(year%4==0 && year%100!=0)
	{
		leapyear=1;	//Schaltjahr, da durch 4 teilbar aber nicht durch 100
	}
	else
	{
		if(year%4!=0)
		{
			leapyear=0;//Kein Schaltjahr, da nicht durch 4 teilbar
		}
		else
		{
			if(year%400==0)
			{
				leapyear=1;//Schaltjahre, da durch 400 teilbar
			}
			else
			{
				leapyear=0;//Kein Schaltjahr, da durch 100 teilbar aber nicht durch 400
			}
		}
	}
}

/////////////////////////////
//Interrupt Service Routine//
/////////////////////////////
//Der Compare Interrupt Handler wird aufgerufen, wenn TCNT0 = OCR0 = 250-1 ist (250 Schritte), d.h. genau alle 1 ms
ISR (TIMER0_COMP_vect)
{
	millisecond++;	//Counter für die Millisekunden
	dataPC = PINC;	//Der aktuelle Pegel vom DCF-Modul wird eingelesen und in dataPC den anderen Funktionen zur Verfügung gestellt
	counter_high++;	//Counter für die Bit-Auswertung (Dauer des High-Pegels), Rücksetzen muß in den Funktionen geschehen
	counter_low++;	//Counter für die Bit-Auswertung (Dauer des Low-Pegels), Rücksetzen muß in den Funktionen geschehen
	if(millisecond==1000)	//"selbstlaufende" Uhr
	{
		second++;
		millisecond=0;
		if(second==60)
		{
			minute++;
			second=0;
		}
		if(minute==60)
		{
			hour++;
			minute=0;
		}
		if(hour==24)
		{
			day++;
			week_day++;
			hour=0;
		}
		if(week_day==8)
		{
			week_day=1;
		}
		switch(day)
		{
			case 28:
			{
				leap_year();
				if(month==2 && leapyear==0)
				{
					month++;
					day=0;
				}
			}
			break;

			case 29:
			{
				leap_year();
				if(month==2 && leapyear==1)
				{
					month++;
					day=0;
				}
			}
			break;

			case 30:
			{
				if(month==4 || month==6 || month==9 || month==11)
				{
					month++;
					day=0;
				}
			}
			break;

			case 31:
			{
				if(month==1 || month==3 || month==5 || month==7|| month==8 || month==10 || month==12)
				{
					month++;
					day=0;
				}
			}
			break;

			default: ;break;
		}
		if(month==12)
		{
			year++;
		}
	}
}

////////////////////////////////////////////////////////
//Zusammenführung der internen Daten und der DCF-Daten//
////////////////////////////////////////////////////////
void match_date(void)
{
	week_day=dcf_week_day;
	day=dcf_day;
	month=dcf_month;
	year=2000+dcf_year;
}

void match_time(void)
{
	millisecond=0;
	second=0;
	minute=dcf_minute;
	hour=dcf_hour;
	timezone=dcf_timezone;
}

///////////////////////
//DCF-Pegelauswertung//
///////////////////////
void level_analysis(void)
{
	if(dataPC & (1<<PINC1))
	{

		if(counter_low>=60 && counter_low<=140)	//logisch 0
		{
//			minute_frame[bit_number]=0;			

			dcf_0();
	
			bit_number++;
			dcf_second++;

			counter_high=0;
			counter_low=0;
		}
		else
		{
			if(counter_low>=160 && counter_low<=240)	//logisch 1
			{
//				minute_frame[bit_number]=1;			
				
				dcf_1();

				bit_number++;

				counter_high=0;
				counter_low=0;
			}
			else
			{
				counter_low=0;
			}
		}
	}
	else
	{
		if(counter_high>=1750 && counter_high<=1950)
		{
			if(bit_number==59 && dcf_minute_parity%2==0 && dcf_hour_parity%2==0 && dcf_date_parity%2==0)
			{
				bit_number=0;

				match_date();
				match_time();
			}
			else
			{
				dcf_date_reset();
				dcf_time_reset();

				counter_high=0;

				dcf_sync();
			}

			counter_high=0;

			dcf_date_reset();
			dcf_time_reset();
		}
	}
}

////////
//Main//
////////
int main(void)
{
	//Setzen der Port-Richtungsregister
	DDRB |= (1<<PB0);	//PB0 Ausgang (1)
	DDRC &= ~(1<<PC1);	//PC1 Eingang (0)
	//Initialisieren der Ports
	PORTB &= ~(1<<PB0);	//PB0 auf low gesetzt (0)
	PORTC |= (1<<PC1);	//PC1 Pull-Up gesetzt (1)

	lcd_init();	//Initialisierung des LCDs

	// Timer 0 konfigurieren
	TCCR0 = (1<<WGM01) | (1<<CS00) | (1<<CS01);	//CTC Modus (1), Prescaler 64 (1) (1)
	OCR0 = 250-1; //((16000000/64)/1000) = 250
	TIMSK |= (1<<OCIE0);	//Compare Interrupt erlauben (1)
	sei();	//Global Interrupts aktivieren

	dcf_sync();	//Synchronisierung des DCF-Moduls

	while(1)
	{
		level_analysis();
		show_matched_date(0,1);
		show_matched_time(0,2);
	}
}
