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
#define UART_BAUD_RATE 9600
#include "uart.h"

#define BLUE (1<<PC0)
#define GREEN (1<<PC2)
#define RED (1<<PC4)
#define LED_PORT PORTC
#define LED_DDR DDRC

volatile uint8_t pwm_GREEN=255, pwm_BLUE=255, pwm_RED=255; //definicje zmiennych przechowuj�cych warto�c pwm
volatile uint8_t tmp_GREEN = 0, tmp_BLUE=0, tmp_RED=0; 
volatile char inputbuff[40];
int freq = 0, cnt2 = 0;
char mix = 0, wave = 0;
char onOff = 0;
uint8_t cntMix = 0, cnt = 0;

ISR(TIMER1_COMPA_vect) 
{
	if (onOff == 1)
	{				
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
	else
		LED_PORT &= ~(GREEN | BLUE | RED);			
	cnt++;	
}  

void blik()
{
	
	if(cnt2<freq)	
		onOff = 1;	
	else
	{
		if(cnt2 < 2*freq)		
			onOff = 0;		
		else
		{	
			cnt2 = 0;
			if(mix == 1)
				mixSet();	
		}			
	}
	cnt2++;	
}

void mixSet()
{
	if(cntMix ==1)
	{
		pwm_RED=255;
		pwm_GREEN=0;
		pwm_BLUE=0;
	}
	if(cntMix ==2)
	{
		pwm_RED=0;
		pwm_GREEN=255;
		pwm_BLUE=0;
	}
	if(cntMix ==3)
	{
		pwm_RED=0;
		pwm_GREEN=0;
		pwm_BLUE=255;
	}
	if(cntMix >=3)
	cntMix = 0;
	cntMix++;
}


void blikWave()
{	
	static char upDown = 0;
	if(cnt2>freq)
	{		
		if(upDown == 0 && mix == 0)
		{		
			if(pwm_BLUE>0)	
				pwm_BLUE--;
			if(pwm_GREEN>0)
				pwm_GREEN--;
			if(pwm_RED>0)
				pwm_RED--;
			if((pwm_BLUE+pwm_GREEN+pwm_RED) == 0)
				upDown = 1;
		}
		
			
		if(upDown == 1 && mix == 0)
		{
			if(pwm_BLUE<tmp_BLUE)
				pwm_BLUE++;
			if(pwm_GREEN<tmp_GREEN)
				pwm_GREEN++;
			if(pwm_RED<tmp_RED)
				pwm_RED++;
			if(pwm_BLUE+pwm_GREEN+pwm_RED == tmp_BLUE+tmp_GREEN+tmp_RED)
				upDown = 0;
		}			
		cnt2 = 0;
		if(mix == 1)
			mixSetWave();		
	}
	cnt2++;
}

void mixSetWave()
{
	static char upDownM = 1;
	if(pwm_RED<255 && upDownM == 1)
		pwm_RED++;			
	if(pwm_RED == 255 && pwm_GREEN<255 && upDownM == 1)
		pwm_GREEN++;			
	if(pwm_RED == 255 && pwm_GREEN==255 && pwm_BLUE<255 && upDownM == 1)		
		pwm_BLUE++;
	if(pwm_RED == 255 && pwm_GREEN==255 && pwm_BLUE==255 && upDownM == 1)
		upDownM = 0;
		
	if(pwm_RED>0 && upDownM == 0)
		pwm_RED--;
	if(pwm_RED == 0 && pwm_GREEN>0 && upDownM == 0)
		pwm_GREEN--;
	if(pwm_RED == 0 && pwm_GREEN==0 && pwm_BLUE>0 && upDownM == 0)
		pwm_BLUE--;
	if(pwm_RED == 0 && pwm_GREEN==0 && pwm_BLUE==0 && upDownM == 0)
		upDownM = 1;
	
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
		if(inputbuff[4]== 'T'&& inputbuff[7]== 'R' && inputbuff[12]== 'G' && inputbuff[17]== 'B'&& inputbuff[22]== 'F'&& inputbuff[28]== 'M'&& inputbuff[31]== 'W')
			if(inputbuff[34]== 'E' && inputbuff[35]== 'N' && inputbuff[36]== 'D')
				return 0;
	return 1;
}

int main(void) 
{ 
	LED_DDR |= GREEN |BLUE | RED; //ustawiamy piny jako wyj�cia
	LED_PORT &= ~(GREEN | BLUE | RED); //wy��czamy na pocz�tek wyj�cia diody RGB
	LED_PORT |= (1<<PC1) | (1<<PC3)  | (1<<PC5); //za��czamy pull-upy przy nieu�ywanych pinach
	LED_DDR &= ~((1<<PC1) | (1<<PC3)| (1<<PC5)); //ustawiamy nieu�ywane piny jako wej�cia			
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
	CTC1_init();
	sei();
	int receivedData, aa=0;	
	char out = 0;	
	uart_flush();
	while(1) 
	{    
		receivedData = uart_available();	
		if(receivedData >= 37)
		{			
			for(int i = 0; i < 37; i++)
			{
				aa = uart_getc();				
				inputbuff[i]= aa & 0xFF;	
				uart_putc(inputbuff[i]);								
			}						
			if(checkReceived() == 0)
			{
				cli();
				onOff = inputbuff[5]-0x30;
				pwm_RED = tmp_RED= (inputbuff[8]-0x30)*100+(inputbuff[9]-0x30)*10+(inputbuff[10]-0x30);
				pwm_GREEN = tmp_GREEN = (inputbuff[13]-0x30)*100+(inputbuff[14]-0x30)*10+(inputbuff[15]-0x30);
				pwm_BLUE = tmp_BLUE =(inputbuff[18]-0x30)*100+(inputbuff[19]-0x30)*10+(inputbuff[20]-0x30);
				freq = (inputbuff[23]-0x30)*1000+(inputbuff[24]-0x30)*100+(inputbuff[25]-0x30)*10 +(inputbuff[26]-0x30);
				mix = inputbuff[29]-0x30;
				wave = inputbuff[32]-0x30;
				sei();			
				uart_puts("ACK\r\n");				
			}
			else
				uart_puts("NACK\r\n");
			uart_flush();																						
		}
		if(cnt % 20 == 0 && freq > 0)
			if(wave != 1)
				blik();
			else
				blikWave();	   		
	} 
	return 0; 
}
