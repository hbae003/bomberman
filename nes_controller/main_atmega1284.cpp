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
unsigned char controller_sent1 = 0xFF;
unsigned char controller_sent2 = 0xFF;

//for selecting levels 
unsigned char level_select = 1;
unsigned char level_opening = 0xFF;

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
				if (USART_IsSendReady(0) && controller_data1 != controller_sent1)
				{
					USART_Send(controller_data1, 0);
					controller_sent1 = controller_data1;
				}
				if (USART_IsSendReady(0) && controller_data2 != controller_sent2)
				{
					USART_Send(controller_data2, 0);
					controller_sent2 = controller_data2;
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
unsigned char num_players = 0;
unsigned char task_sent = 0xFF;
unsigned long ready_counter = 0;
unsigned long level_counter = 0;
unsigned long two_player_data = 0x00;
unsigned long one_player_data = 0x00;
unsigned long two_player_data_number = 0;
unsigned long one_player_data_number = 0;
unsigned char restart_game = 0;
unsigned char bomb_number1 = 0xFF;
unsigned char bomb_number2 = 0xFF;
unsigned char game_end = 0xFF;

//sends signal to arduino letting it know what task to run
enum task_states{press_start, player_choose, lv1, lv2, lv3, ready_wait, one_player, two_player, player1_win, player2_win, game_over, the_end} task_state;
int task_manager(int state)
{
	switch(state)
	{
		case press_start:
			if (controller_data1 == 0x06) { state = player_choose; }
		break;
		
		case player_choose:
			//if (num_players == 1) { state = one_player; }
			//if (num_players == 2) { state = two_player; }
			if (num_players == 2 || num_players == 1){ state = ready_wait; }
		break;
		
		case lv1:
			if (level_counter < 20000) { level_counter++; }
			else if (level_counter >= 10000)
			{
				level_counter = 0;
				level_select = 1;
				state = one_player;
			}
			break;
			
		case lv2:
			if (level_counter < 20000) { level_counter++; }
			else if (level_counter >= 10000)
			{
				level_counter = 0;
				level_select = 2;
				state = one_player;
			}
			break;
		
		case lv3:
			if (level_counter < 20000) { level_counter++; }
			else if (level_counter >= 10000)
			{
				level_counter = 0;
				level_select = 3;
				state = one_player;
			}
			break;
			
		case ready_wait:
			if (ready_counter < 10000) { ready_counter++; }
			else if (ready_counter >= 10000)
			{
				ready_counter = 0;
				if (num_players == 1) { state = lv1; num_players = 0; }
				if (num_players == 2) { state = two_player; num_players = 0; }
			}
		break;
		
		case one_player:
			if (level_opening == 0xA2) { state = lv2; level_opening = 0xFF; }
			if (level_opening == 0xA3) { state = lv3; level_opening = 0xFF; }
			if (restart_game == 0x69) { state = press_start; restart_game = 0; }
			if (game_end == 0x81 || game_end == 0x82) { state = game_over; }
			if (game_end == 0x83) { state = the_end; }
		break;
		
		case two_player:
			if (restart_game == 0x69) { state = press_start; restart_game = 0; }
			if (game_end == 0x81) { state = player2_win; game_end = 0xFF; }
			if (game_end == 0x82) { state = player1_win; game_end = 0xFF; }
		break;
		
		case player1_win:
			if (restart_game == 0x6A) { state = press_start; restart_game = 0; }
			break;
			
		case player2_win:
			if (restart_game == 0x6A) { state = press_start; restart_game = 0; }
			break;
			
		case game_over:
			if (restart_game == 0x6A) { state = press_start; restart_game = 0; }
			break;
			
		case the_end:
			if (restart_game == 0x6A) { state = press_start; restart_game = 0; }
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
				if (USART_IsSendReady(0) && task_sent != 0x20)
				{
					USART_Send(0x20, 0);
					task_sent = 0x20;
				}
			}
		break;
		
		case player_choose:
			if (task_counter < 50) { task_counter++; }
			else if (task_counter >= 50)
			{
				task_counter = 0;
				if (USART_IsSendReady(0) && task_sent != 0x21)
				{
					USART_Send(0x21, 0);
					task_sent = 0x21;
				}
				if (USART_HasReceived(0))
				{
					num_players = USART_Receive(0);
				}
			}
		break;
		
		case lv1:
			if (task_counter < 50) { task_counter++; }
			else if (task_counter >= 50)
			{
				//restrict number of times signal is sent
				task_counter = 0;
				if (USART_IsSendReady(0) && task_sent != 0x25)
				{
					USART_Send(0x25, 0);
					task_sent = 0x25;
				}
			}
			break;
			
		case lv2:
			if (task_counter < 50) { task_counter++; }
			else if (task_counter >= 50)
			{
				//restrict number of times signal is sent
				task_counter = 0;
				if (USART_IsSendReady(0) && task_sent != 0x26)
				{
					USART_Send(0x26, 0);
					task_sent = 0x26;
				}
			}
			break;
			
		case lv3:
			if (task_counter < 50) { task_counter++; }
			else if (task_counter >= 50)
			{
				//restrict number of times signal is sent
				task_counter = 0;
				if (USART_IsSendReady(0) && task_sent != 0x27)
				{
					USART_Send(0x27, 0);
					task_sent = 0x27;
				}
			}
			break;
		
		case ready_wait:
			if (task_counter < 50) { task_counter++; }
			else if (task_counter >= 50)
			{
				//restrict number of times signal is sent
				task_counter = 0;
				if (USART_IsSendReady(0) && task_sent != 0x22)
				{
					USART_Send(0x22, 0);
					task_sent = 0x22;
				}
			}
		break;
		
		case one_player:
			if (task_counter < 50) { task_counter++; }
			else if (task_counter >= 50)
			{
				//restrict number of times signal is sent
				if (USART_IsSendReady(0) && task_sent != 0x23)
				{
					USART_Send(0x23, 0);
					task_sent = 0x23;
				}
				if (USART_HasReceived(0))
				{
					one_player_data = USART_Receive(0);
					one_player_data_number = one_player_data & 0xF0;
					if (one_player_data_number == 0x60) { restart_game = one_player_data; }
					if (one_player_data_number == 0x70) { bomb_number1 = one_player_data; }
					if (one_player_data_number == 0x40) { bomb_number2 = one_player_data; }
					if (one_player_data_number == 0x80) { game_end = one_player_data; }
					if (one_player_data_number == 0xA0) { level_opening = one_player_data; }
				}
			}
		break;
		
		case two_player:
			if (task_counter < 50) { task_counter++; }
			else if (task_counter >= 50)
			{
				//restrict number of times signal is sent
				if (USART_IsSendReady(0) && task_sent != 0x24)
				{
					USART_Send(0x24, 0);
					task_sent = 0x24;
				}
				if (USART_HasReceived(0))
				{
					two_player_data = USART_Receive(0);
					two_player_data_number = two_player_data & 0xF0;
					if (two_player_data_number == 0x60) { restart_game = two_player_data; }
					if (two_player_data_number == 0x70) { bomb_number1 = two_player_data; }
					if (two_player_data_number == 0x40) { bomb_number2 = two_player_data; }
					if (two_player_data_number == 0x80) { game_end = two_player_data; }
				}
			}
		break;
		
		case player1_win:
		if (USART_IsSendReady(0) && task_sent != 0x2A)
		{
			USART_Send(0x2A, 0);
			task_sent = 0x2A;
		}
		if (USART_HasReceived(0))
		{
			restart_game = USART_Receive(0);
		}
		break;
		
		case player2_win:
		if (USART_IsSendReady(0) && task_sent != 0x2B)
		{
			USART_Send(0x2B, 0);
			task_sent = 0x2B;
		}
		if (USART_HasReceived(0))
		{
			restart_game = USART_Receive(0);
		}
		break;
		
		case game_over:
		if (USART_IsSendReady(0) && task_sent != 0x2C)
		{
			USART_Send(0x2C, 0);
			task_sent = 0x2C;
		}
		if (USART_HasReceived(0))
		{
			restart_game = USART_Receive(0);
		}
			break;
		
		case the_end:
			if (USART_IsSendReady(0) && task_sent != 0x2D)
			{
				USART_Send(0x2D, 0);
				task_sent = 0x2D;
			}
			if (USART_HasReceived(0))
			{
				restart_game = USART_Receive(0);
			}
			break;
		
		default:
		break;
	}
	
	return state;
}

unsigned char bomb_sent = 0x00;
unsigned long counter1_1 = 0;
unsigned long counter1_2 = 0;
unsigned long counter1_3 = 0;
unsigned long counter2_1 = 0;
unsigned long counter2_2 = 0;
unsigned long counter2_3 = 0;

enum bomb1_1_states {start1_1, wait1_1, send1_1} bomb1_1_state;
int bomb1_1(int state)
{
	switch (state)
	{
		case start1_1:
			if (bomb_number1 == 0x71) { state = wait1_1; bomb_number1 = 0; }
			break;
		case wait1_1:
			if (counter1_1 < 10000) { counter1_1++; }
			else { state = send1_1; counter1_1 = 0; }
			break;
		case send1_1:
			if (bomb_sent == 0x51) { state = start1_1; bomb_sent = 0; }
			break;
		default:
			break;
	}
	
	switch (state)
	{
		case start1_1:
			counter1_1 = 0;
			break;
		case wait1_1:
			break;
		case send1_1:
			if (counter1_1 < 50) { counter1_1++; }
			else if (counter1_1 >= 50)
			{
				//restrict number of times signal is sent
				if (USART_IsSendReady(0) && bomb_sent != 0x51)
				{
					USART_Send(0x51, 0);
					bomb_sent = 0x51;
				}
			}
			break;
		default:
			break;
	}

	return state;
}

enum bomb1_2_states {start1_2, wait1_2, send1_2} bomb1_2_state;

int bomb1_2(int state)
{
	switch (state)
	{
		case start1_2:
			if (bomb_number1 == 0x72) { state = wait1_2; bomb_number1 = 0; }
			break;
		case wait1_2:
			if (counter1_2 < 10000) { counter1_2++; }
			else { state = send1_2; counter1_2 = 0; }
			break;
		case send1_2:
			if (bomb_sent == 0x52) { state = start1_2; bomb_sent = 0; }
			break;
		default:
			break;
	}
	
	switch (state)
	{
		case start1_2:
			counter1_2 = 0;
			break;
		case wait1_2:
			break;
		case send1_2:
			if (counter1_2 < 50) { counter1_2++; }
			else if (counter1_2 >= 50)
			{
				//restrict number of times signal is sent
				if (USART_IsSendReady(0) && bomb_sent != 0x52)
				{
					USART_Send(0x52, 0);
					bomb_sent = 0x52;
				}
			}
			break;
		default:
			break;
	}

	return state;
}

enum bomb1_3_states {start1_3, wait1_3, send1_3} bomb1_3_state;

int bomb1_3(int state)
{
	switch (state)
	{
		case start1_3:
			if (bomb_number1 == 0x73) { state = wait1_3; bomb_number1 = 0; }
			break;
		case wait1_3:
			if (counter1_3 < 10000) { counter1_3++; }
			else { state = send1_3; counter1_3 = 0; }
			break;
		case send1_3:
			if (bomb_sent == 0x53) { state = start1_3; bomb_sent = 0; }
			break;
		default:
			break;
	}
	
	switch (state)
	{
		case start1_3:
			counter1_3 = 0;
			break;
		case wait1_3:
			break;
		case send1_3:
			if (counter1_3 < 50) { counter1_3++; }
			else if (counter1_3 >= 50)
			{
				//restrict number of times signal is sent
				if (USART_IsSendReady(0) && bomb_sent != 0x53)
				{
					USART_Send(0x53, 0);
					bomb_sent = 0x53;
				}
			}
			break;
		default:
			break;
	}

	return state;
}

enum bomb2_1_states {start2_1, wait2_1, send2_1} bomb2_1_state;

int bomb2_1(int state)
{
	switch (state)
	{
		case start2_1:
			if (bomb_number2 == 0x41) { state = wait2_1; bomb_number2 = 0; }
			break;
		case wait2_1:
			if (counter2_1 < 10000) { counter2_1++; }
			else { state = send2_1; counter2_1 = 0; }
			break;
		case send2_1:
			if (bomb_sent == 0x54) { state = start2_1; bomb_sent = 0; }
			break;
		default:
			break;
	}
	
	switch (state)
	{
		case start2_1:
			counter2_1 = 0;
			break;
		case wait2_1:
			break;
		case send2_1:
			if (counter2_1 < 50) { counter2_1++; }
			else if (counter2_1 >= 50)
			{
				//restrict number of times signal is sent
				if (USART_IsSendReady(0) && bomb_sent != 0x54)
				{
					USART_Send(0x54, 0);
					bomb_sent = 0x54;
				}
			}
			break;
		default:
			break;
	}

	return state;
}

enum bomb2_2_states {start2_2, wait2_2, send2_2} bomb2_2_state;

int bomb2_2(int state)
{
	switch (state)
	{
		case start2_2:
			if (bomb_number2 == 0x42) { state = wait2_2; bomb_number2 = 0; }
			break;
		case wait2_2:
			if (counter2_2 < 10000) { counter2_2++; }
			else { state = send2_2; counter2_2 = 0; }
			break;
		case send2_2:
			if (bomb_sent == 0x55) { state = start2_2; bomb_sent = 0; }
			break;
		default:
			break;
	}
	
	switch (state)
	{
		case start2_2:
			counter2_2 = 0;
			break;
		case wait2_2:
			break;
		case send2_2:
			if (counter2_2 < 50) { counter2_2++; }
			else if (counter2_2 >= 50)
			{
				//restrict number of times signal is sent
				if (USART_IsSendReady(0) && bomb_sent != 0x55)
				{
					USART_Send(0x55, 0);
					bomb_sent = 0x55;
				}
			}
			break;
		default:
			break;
	}

	return state;
}

enum bomb2_3_states {start2_3, wait2_3, send2_3} bomb2_3_state;

int bomb2_3(int state)
{
	switch (state)
	{
		case start2_3:
			if (bomb_number2 == 0x43) { state = wait2_3; bomb_number2 = 0; }
			break;
		case wait2_3:
			if (counter2_3 < 10000) { counter2_3++; }
			else { state = send2_3; counter2_3 = 0; }
			break;
		case send2_3:
			if (bomb_sent == 0x56) { state = start2_3; bomb_sent = 0; }
			break;
		default:
			break;
	}
	
	switch (state)
	{
		case start2_3:
			counter2_3 = 0;
			break;
		case wait2_3:
			break;
		case send2_3:
			if (counter2_3 < 50) { counter2_3++; }
			else if (counter2_3 >= 50)
			{
				//restrict number of times signal is sent
				if (USART_IsSendReady(0) && bomb_sent != 0x56)
				{
					USART_Send(0x56, 0);
					bomb_sent = 0x56;
				}
			}
			break;
		default:
			break;
	}

	return state;
}

enum bomb_drop_states {start_drop, level1, level2, level3} bomb_drop_state;
unsigned long armer_counter = 0;

int single_bomb_drop(int state)
{
	switch (state)
	{
		case start_drop:
			if (task_sent == 0x23 && level_select == 1) { state = level1; }
			if (task_sent == 0x23 && level_select == 2) { state = level2; }
			if (task_sent == 0x23 && level_select == 3) { state = level3; }
			break;
		
		case level1:
			if (task_sent != 0x23) { state = start; game_end = 0xFF; }
			armer_counter++;
			break;
			
		case level2:
			if (task_sent != 0x23) { state = start; game_end = 0xFF; }
			armer_counter++;
			break;
		
		case level3:
			if (task_sent != 0x23) { state = start; game_end = 0xFF; }
			armer_counter++;
			break;
		
		default:
			break;
	}
	
	switch (state)
	{
		case start_drop:
			armer_counter = 0;
			break;
		
		case level1:
			if(armer_counter % 20 == 0)
			{
				if (USART_IsSendReady(0))
				{
					USART_Send(0x90, 0);
				}
				if (USART_IsSendReady(0))
				{
					USART_Send(0x91, 0);
				}
			}
			break;
		
		case level2:
			if(armer_counter % 15 == 0)
			{
				if (USART_IsSendReady(0))
				{
					USART_Send(0x90, 0);
				}
				if (USART_IsSendReady(0))
				{
					USART_Send(0x91, 0);
				}
			}
			break;
		
		case level3:
			if(armer_counter % 10 == 0)
			{
				if (USART_IsSendReady(0))
				{
					USART_Send(0x90, 0);
				}
				if (USART_IsSendReady(0))
				{
					USART_Send(0x91, 0);
				}
			}
			break;
		
		default:
			break;
	}
	return state;
}

enum safe_states {safe_init, safe_send} safe_state;
	
unsigned char safe_counter = 0;
int safe_safe(int state)
{
	switch(state)
	{
		case safe_init:
			if (task_sent == 0x24) { state = safe_send; }
			break;
		
		case safe_send:
			if (task_sent != 0x24) { state = safe_init; }
			break;
			
		default:
			break;
	}
	
	switch(state)
	{
		case safe_init:
		safe_counter = 0;
		break;
		
		case safe_send:
			safe_counter++;
			if(safe_counter >= 100)
			{
				safe_counter = 0;
				if (USART_IsSendReady(0))
				{
					USART_Send(0xB0, 0);
				}
				if (USART_IsSendReady(0))
				{
					USART_Send(0xB1, 0);
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
	
	static task task1, task2, task3, task4, task5, task6, task7, task8, task9, task10, task11;
	task *tasks[] = { &task1, &task2, &task3, &task4, &task5, &task6, &task7, &task8, &task9, &task10, &task11};
	const unsigned short numTasks = 11;

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
	
	task4.state = start1_1;
	task4.period = 1;
	task4.elapsedTime = 1;
	task4.TickFct = &bomb1_1;
	
	task5.state = start1_2;
	task5.period = 1;
	task5.elapsedTime = 1;
	task5.TickFct = &bomb1_2;
	
	task6.state = start1_3;
	task6.period = 1;
	task6.elapsedTime = 1;
	task6.TickFct = &bomb1_3;
	
	task7.state = start2_1;
	task7.period = 1;
	task7.elapsedTime = 1;
	task7.TickFct = &bomb2_1;
	
	task8.state = start2_2;
	task8.period = 1;
	task8.elapsedTime = 1;
	task8.TickFct = &bomb2_2;
	
	task9.state = start2_3;
	task9.period = 1;
	task9.elapsedTime = 1;
	task9.TickFct = &bomb2_3;
	
	task10.state = start_drop;
	task10.period = 500;
	task10.elapsedTime = 500;
	task10.TickFct = &single_bomb_drop;
	
	task11.state = safe_init;
	task11.period = 400;
	task11.elapsedTime = 400;
	task11.TickFct = &safe_safe;
	
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
