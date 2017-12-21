/*
 * RGB_ControllerAt8.c
 *
 * Created: 2017-12-20 22:08:57
 *  Author: DawidN
 */ 

#include <avr/io.h>
#define F_CPU 11059200UL
#include <util/delay.h>
#include <avr/interrupt.h>
#define UART_BAUD_RATE 115200
#include "uart.h"

#define BLUE (1<<PC0)
#define GREEN (1<<PC2)
#define RED (1<<PC4)
#define LED_PORT PORTC
#define LED_DDR DDRC

volatile uint8_t pwm_GREEN=255, pwm_BLUE=255, pwm_RED=255; //definicje zmiennych przechowuj¹cych wartoœc pwm
volatile char inputbuff[40];
volatile int freq = 0;


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
	OCR1A = 7; 
	TIMSK |= (1 << OCIE1A);
}

char checkReceived()
{
	if(inputbuff[0]== 'S' && inputbuff[1]== 'E' && inputbuff[2]== 'T')	
		if(inputbuff[4]== 'R' && inputbuff[9]== 'G' && inputbuff[14]== 'B'&& inputbuff[19]== 'F')
			if(inputbuff[25]== 'E' && inputbuff[26]== 'N' && inputbuff[27]== 'D')
				return 0;
	return 1;
}

int main(void) 
{ 
	LED_DDR |= GREEN |BLUE | RED; //ustawiamy piny jako wyjœcia
	LED_PORT &= ~(GREEN | BLUE | RED); //wy³¹czamy na pocz¹tek wyjœcia diody RGB
	LED_PORT |= (1<<PC1) | (1<<PC3)  | (1<<PC5); //za³¹czamy pull-upy przy nieu¿ywanych pinach
	LED_DDR &= ~((1<<PC1) | (1<<PC3)| (1<<PC5)); //ustawiamy nieu¿ywane piny jako wejœcia
	_delay_ms(1000);
		
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
	CTC1_init();
	sei();
	int receivedData, aa=0;	
	char out = 0;	
	while(1) 
	{    
		receivedData = uart_available();	
		if(receivedData >= 28)
		{			
			for(int i = 0; i < 28; i++)
			{
				aa = uart_getc();				
				inputbuff[i]= aa & 0xFF;						
			}						
			if(checkReceived() == 0)
			{
				pwm_RED = (inputbuff[5]-0x30)*100+(inputbuff[6]-0x30)*10+(inputbuff[7]-0x30);
				pwm_GREEN = (inputbuff[10]-0x30)*100+(inputbuff[11]-0x30)*10+(inputbuff[12]-0x30);
				pwm_BLUE = (inputbuff[15]-0x30)*100+(inputbuff[16]-0x30)*10+(inputbuff[17]-0x30);
				freq = (inputbuff[20]-0x30)*1000+(inputbuff[21]-0x30)*100+(inputbuff[22]-0x30)*10+(inputbuff[23]-0x30);
				uart_puts("ACK\r\n");
			}
			else
				uart_puts("NACK\r\n");
			uart_flush();																						
		}	   		
	} 
	return 0; 
}




/*for (i = 0; i < 255; i++) { //rozjaœniamy diodê GREENon¹
				  pwm_GREEN++; //inkrementacja zmiennej pwm diody GREENonej
				  _delay_ms(2); //odstep czasowy
			}
			  //
		 for (i = 0; i < 255; i++) { //œciemniamy diodê GREENon¹
			  pwm_GREEN--; //dekrementacja zmiennej pwm diody GREENonej
			_delay_ms(2); //odstep czasowy
		  }
		  for (i = 0; i < 255; i++) { //rozjaœniamy diodê BLUEiesk¹
		  pwm_BLUE++; //inkrementacja zmiennej pwm diody BLUEieskiej
		  _delay_ms(2); //odstep czasowy
		  }  
		  for (i = 0; i < 255; i++) { //œciemniamy diodê BLUEiesk¹  
		   pwm_BLUE--;  //dekrementacja zmiennej pwm diody BLUEieskiej  
		   _delay_ms(2); //odstep czasowy  
		  }  
		  //  
		  for (i = 0; i < 255; i++) { //rozjaœniamy diodê REDwon¹  
		   pwm_RED++; //inkrementacja zmiennej pwm diody REDwonej  
		   _delay_ms(2); //odstep czasowy  
		  }  
		  for (i = 0; i < 255; i++) { //œciemniamy diodê REDwon¹  
		   pwm_RED--; //dekrementacja zmiennej pwm diody REDwonej  
		   _delay_ms(2); //odstep czasowy  
		  }  */