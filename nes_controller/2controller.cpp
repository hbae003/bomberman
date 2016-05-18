/*
 * controller_test.cpp
 *
 * Created: 5/13/2016 2:32:58 PM
 * Author : PeterBae
 */ 

#include <avr/io.h>
#include "timer.h"

unsigned char i;
unsigned char j;
unsigned char temp_data1;
unsigned char temp_data2;
unsigned char controller_data1 = 0x00;
unsigned char controller_data2 = 0x00;

enum controller_states {start, init, wait1, read1, wait2, wait3, final} controller_state;
void controller_read()
{
	switch(controller_state)
	{
		case start:
			controller_state = init;
		break;
		
		case init:
			PORTA = 0x44; // latch = 1
			controller_state = wait1;
		break;
		
		case wait1:
			if (i < 2) {  i++; }
			else if (i >= 2) { PORTA = 0x00; i = 0; controller_state = read1;} //latch = 0
		break;
		
		case read1:
			PORTA = 0x22; j++; controller_state = wait2; //clock = 1
		break;
		
		case wait2:
			if (i < 2) { i++; } // wait for 2 ms
			else if (i >= 2) 
			{
				controller_state = wait3;
				temp_data2 = temp_data2 << 1;
				temp_data2 = temp_data2 + ((PINA & 0x80) >> 7);
				temp_data1 = temp_data1 << 1;
				temp_data1 = temp_data1 + ((PINA & 0x08) >> 3);
				i = 0;
			}
		break;
		
		case wait3:
			if (i < 4) { i++; }
			else if (i >= 4 && j >= 7) { controller_state = final; }
			else if (i >= 4 && j < 7) { PORTA = 0x00; PORTA = 0x22; i = 0; controller_state = wait2; j++; } //clock = 0 and then clock = 1
		break;
		
		case final:
			controller_state = init;
		break;			
		
		default:
		break;
		
	}
	
	switch(controller_state)
	{
		case start:
		break;
	
		case init:
			i = 0;
			j = 0; 
			PORTA = 0x00; 
			temp_data1 = 0;
			temp_data2 = 0;
		break;
		
		case wait1:
		break;
		
		case read1:
			temp_data1 = (PINA & 0x08) >> 3; //temp_data = data in
			temp_data2 = (PINA & 0x80) >> 7;
		break;
		
		case wait2:
		break;
		
		case wait3: 
		break;
		
		case final:
			controller_data1 = temp_data1;
			controller_data2 = temp_data2;
		break;
		
		default:
		break;
		
		
	}
}

unsigned char counter = 0;
enum output_states {start1, read_control} output_state;

void controller_out()
{
	switch(output_state)
	{
		case start1:
			output_state = read_control;
			break;
			
		case read_control:
			if (counter < 50) { counter++; }
			else { counter = 0; PORTC = ~controller_data1; PORTD = ~controller_data2;} 
			break;
	}
	
	switch(output_state)
	{
		case start1:
			counter = 0;
			break;
			
		case read_control:
			break;
	}
}


int main(void)
{
    DDRA = 0x66; PORTA = 0x99;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	
	TimerSet(1);
	TimerOn();
	
    while (1) 
    {
		controller_read();
		controller_out();
		while(!TimerFlag);
		TimerFlag = 0;
    }
}

