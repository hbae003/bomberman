/*
 * controller_test.cpp
 *
 * Created: 5/13/2016 2:32:58 PM
 * Author : PeterBae
 */ 

#include <avr/io.h>
#include "timer.h"
#include "usart_atmega1284.h"

const unsigned char UP = 0x08;
const unsigned char DOWN = 0x04;
const unsigned char LEFT = 0x02;
const unsigned char RIGHT = 0x01;
const unsigned char SELECT = 0x20;
const unsigned char START = 0x10;
const unsigned char Y = 0x40;
const unsigned char B = 0x80;

unsigned char i;
unsigned char j;
unsigned char temp_data1;
unsigned char temp_data2;
unsigned char controller_data1 = 0x00;
unsigned char controller_data2 = 0x10;

typedef struct task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

enum controller_states {start, init, wait1, read1, wait2, wait3, final} controller_state;
int controller_read(int state)
{
	switch(state)
	{
		case start:
			state = init;
		break;
		
		case init:
			PORTA = 0x44; // latch = 1
			state = wait1;
		break;
		
		case wait1:
			if (i < 2) {  i++; }
			else if (i >= 2) { PORTA = 0x00; i = 0; state = read1;} //latch = 0
		break;
		
		case read1:
			PORTA = 0x22; j++; state = wait2; //clock = 1
		break;
		
		case wait2:
			if (i < 2) { i++; } // wait for 2 ms
			else if (i >= 2) 
			{
				state = wait3;
				temp_data2 = temp_data2 << 1;
				temp_data2 = temp_data2 + ((PINA & 0x80) >> 7);
				temp_data1 = temp_data1 << 1;
				temp_data1 = temp_data1 + ((PINA & 0x08) >> 3);
				i = 0;
			}
		break;
		
		case wait3:
			if (i < 4) { i++; }
			else if (i >= 4 && j >= 7) { state = final; }
			else if (i >= 4 && j < 7) { PORTA = 0x00; PORTA = 0x22; i = 0; state = wait2; j++; } //clock = 0 and then clock = 1
		break;
		
		case final:
			state = init;
		break;			
		
		default:
		break;
		
	}
	
	switch(state)
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
			//if nothing gets pressed, controller data shows nothing is pressed
			controller_data1 = 0x00;
			controller_data2 = 0x10;
			
			//NOT the data
			temp_data1 = ~temp_data1; temp_data2 = ~temp_data2;
			//first change controller data1
			if (temp_data1 == UP) { controller_data1 = 0x01; }
			else if (temp_data1 == DOWN) { controller_data1 = 0x02; }
			else if (temp_data1 == LEFT) { controller_data1 = 0x03; }
			else if (temp_data1 == RIGHT) { controller_data1 = 0x04; }
			else if (temp_data1 == SELECT) { controller_data1 = 0x05; }
			else if (temp_data1 == START) { controller_data1 = 0x06; }
			else if (temp_data1 == Y) { controller_data1 = 0x07; }
			else if (temp_data1 == B) { controller_data1 = 0x08; }
				
			//change controller data2
			if (temp_data2 == UP) { controller_data2 = 0x11; }
			else if (temp_data2 == DOWN) { controller_data2 = 0x12; }
			else if (temp_data2 == LEFT) { controller_data2 = 0x13; }
			else if (temp_data2 == RIGHT) { controller_data2 = 0x14; }
			else if (temp_data2 == SELECT) { controller_data2 = 0x15; }
			else if (temp_data2 == START) { controller_data2 = 0x16; }
			else if (temp_data2 == Y) { controller_data2 = 0x17; }
			else if (temp_data2 == B) { controller_data2 = 0x18; }
		break;
		
		default:
		break;
		
	}
	return state;
}

//__CONTROLLER_1__
//	NO PRESS	0x00
//	UP		0x01
//	DOWN	0x02
//	LEFT	0x03
//	RIGHT	0x04
//	SELECT	0x05
//	START	0x06
//	Y		0x07
//	B		0x08

//__CONTROLLER_2__
//	NO PRESS	0x10
//	UP		0x11
//	DOWN	0x12
//	LEFT	0x13
//	RIGHT	0x14
//	SELECT	0x15
//	START	0x16
//	Y		0x17
//	B		0x18

unsigned char counter = 0;
enum output_states {start1, read_control} output_state;

int controller_out(int state)
{
	switch(state)
	{
		case start1:
			state = read_control;
			break;
			
		case read_control:
			if (counter < 200) { counter++; }
			else 
			{ 
				counter = 0; 
				PORTC = controller_data1 | (controller_data2 << 4);
				if (USART_IsSendReady(0))
				{
					USART_Send(controller_data1, 0);
				}
				if (USART_IsSendReady(0))
				{
					USART_Send(controller_data2, 0);
				}
			} 
			break;
	}
	
	switch(state)
	{
		case start1:
			counter = 0;
			break;
			
		case read_control:
			break;
	}
	return state;
}

unsigned char task_counter = 0x00;
unsigned char send_stop = 0x00;
unsigned char num_players = 0;
//sends signal to arduino letting it know what task to run
enum task_states{press_start, player_choose, one_player, two_player} task_state;
int task_manager(int state)
{
	switch(state)
	{
		case press_start:
			if (controller_data1 == 0x06) { state = player_choose; }
		break;
		
		case player_choose:
			if (num_players == 1) { state = one_player; }
			if (num_players == 2) { state = two_player; }
		break;
		
		case one_player:
		break;
		
		case two_player:
		break;
		
		default:
		break;
	}
	
	switch(state)
	{
		case press_start:
			if (task_counter < 50) { task_counter++; }
			else if (task_counter >= 50)
			{
				task_counter = 0;
				if (USART_IsSendReady(0))
				{
					USART_Send(0x20, 0);
				}
			}
		break;
		
		case player_choose:
			if (task_counter < 50) { task_counter++; }
			else if (task_counter >= 50)
			{
				task_counter = 0;
				if (USART_IsSendReady(0))
				{
					USART_Send(0x21, 0);
				}
				if (USART_HasReceived(0))
				{
					num_players = USART_Receive(0);
				}
			}
		break;
		
		case one_player:
		break;
		
		case two_player:
			if (task_counter < 50) { task_counter++; }
			else if (task_counter >= 50)
			{
				//restrict number of times signal is sent
				if (USART_IsSendReady(0) && send_stop < 10)
				{
					USART_Send(0x22, 0);
					send_stop++;
				}
			}
		break;
		
		default:
		break;
	}
	
	return state;
}

int main(void)
{
    DDRA = 0x66; PORTA = 0x99;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	static task task1, task2, task3;
	task *tasks[] = { &task1, &task2, &task3};
	const unsigned short numTasks = 3;

	task1.state = start;//Task initial state.
	task1.period = 1;//Task Period.
	task1.elapsedTime = 1;//Task current elapsed time.
	task1.TickFct = &controller_read;//Function pointer for the tick.

	task2.state = start1;
	task2.period = 1;
	task2.elapsedTime = 1;
	task2.TickFct = &controller_out;
	
	task3.state = press_start;
	task3.period = 1;
	task3.elapsedTime = 1;
	task3.TickFct = &task_manager;
		
	initUSART(0);
	TimerSet(1);
	TimerOn();

	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
	}
}