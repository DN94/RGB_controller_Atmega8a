/*
 * RGB_ControllerAt8.c
 *
 * Created: 2017-12-20 22:08:57
 *  Author: DawidN
 */ 


#define F_CPU 11059200UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define UART_BAUD_RATE 115200
#include "uart.h"

#define BLUE (1<<PC0)
#define GREEN (1<<PC4)
#define RED (1<<PC2)
#define LED_PORT PORTC
#define LED_DDR DDRC

volatile uint8_t pwm_GREEN, pwm_BLUE, pwm_RED; //definicje zmiennych przechowuj�cych warto�c pwm
uint8_t i; //zmienna na potrzeby p�tli for


char printbuff[30];
volatile char inputbuff[10];



ISR(TIMER1_COMPA_vect) 
{
	static uint8_t cnt = 0; 
	cnt++;
	if (pwm_GREEN > cnt) 
		LED_PORT |= GREEN; 
	else 
		LED_PORT &= ~GREEN; 
	if (pwm_RED > cnt) 
		LED_PORT |= RED; 
	else 
		LED_PORT &= ~RED; 

	if (pwm_BLUE > cnt) 
		LED_PORT |= BLUE; 
	else 
		LED_PORT &= ~BLUE; 
	if (cnt > 255) 
		cnt = 0; 
}  

void CTC1_init() //inciclalizacja timera1  
{		
	TCCR1B |= (1 << WGM12)|(1 << CS11)|(1 << CS10);
	TCNT1 = 0;	
	OCR1A = 9; 
	TIMSK |= (1 << OCIE1A);
}


int main(void) 
{ 
	LED_DDR |= GREEN |BLUE | RED; //ustawiamy piny jako wyj�cia
	LED_PORT &= ~(GREEN | BLUE | RED); //wy��czamy na pocz�tek wyj�cia diody RGB
	LED_PORT |= (1<<PC1) | (1<<PC3)  | (1<<PC5); //za��czamy pull-upy przy nieu�ywanych pinach
	LED_DDR &= ~((1<<PC1) | (1<<PC3)| (1<<PC5)); //ustawiamy nieu�ywane piny jako wej�cia
	_delay_ms(1000);	
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
	CTC1_init();
	sei();
	unsigned int receivedData, aa=0;	
	char out = 0;	
	while(1) 
	{    
		/*receivedData = uart_available();	
		if(receivedData >= 6)
		{	
			out = 0;									
			for(int i = 0; i < 6; i++)
			{					
				aa = uart_getc();					
				inputbuff[i]= aa & 0xFF;
				if(inputbuff[0]== 'D' && i !=0)
				{
					out +=  (inputbuff[i]-0x30)<<(i-1);
				}											    	
			}					
			uart_flush();																	
		}	*/
		for (i = 0; i < 255; i++) { //rozja�niamy diod� GREENon�
				  pwm_GREEN++; //inkrementacja zmiennej pwm diody GREENonej
				  _delay_ms(2); //odstep czasowy
			}
			  //
		 for (i = 0; i < 255; i++) { //�ciemniamy diod� GREENon�
			  pwm_GREEN--; //dekrementacja zmiennej pwm diody GREENonej
			_delay_ms(2); //odstep czasowy
		  }
		  for (i = 0; i < 255; i++) { //rozja�niamy diod� BLUEiesk�
		  pwm_BLUE++; //inkrementacja zmiennej pwm diody BLUEieskiej
		  _delay_ms(2); //odstep czasowy
		  }  
		  for (i = 0; i < 255; i++) { //�ciemniamy diod� BLUEiesk�  
		   pwm_BLUE--;  //dekrementacja zmiennej pwm diody BLUEieskiej  
		   _delay_ms(2); //odstep czasowy  
		  }  
		  //  
		  for (i = 0; i < 255; i++) { //rozja�niamy diod� REDwon�  
		   pwm_RED++; //inkrementacja zmiennej pwm diody REDwonej  
		   _delay_ms(2); //odstep czasowy  
		  }  
		  for (i = 0; i < 255; i++) { //�ciemniamy diod� REDwon�  
		   pwm_RED--; //dekrementacja zmiennej pwm diody REDwonej  
		   _delay_ms(2); //odstep czasowy  
		  }  
		   		
	} 
	return 0; 
}

