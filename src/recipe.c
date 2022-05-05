/*
 * recipe.c
 *
 *  Created on: Feb 20, 2022
 *      Author: abhis
 */

#include <stdio.h>
#include <stdlib.h>
#include "stm32l476xx.h"
#include "main.h"

UART_HandleTypeDef huart2;
int index1, index2, s, current_position, previous_position, current_state, previous_state, previous_position2;
int* current_recipe;
int* current_recipe2;
int err[];
int n,k,j,loop_param, w, wait_servo1, ran, r, q, loop_flag;
int n2,k2,j2,loop_param2, w2, wait_servo2, ran2, r2, q2, loop_flag2;

char flag;

enum command {pause, cont, left, right, nothing, begin , waiting, delaying,  looping, looperr, wrong};
enum command2 {pause2, cont2, left2, right2, nothing2, begin2 , waiting2, delaying2,  looping2, looperr2};
enum command servo;
enum command2 servo2;
enum state {paused, running, ended, in_random};
enum state2 {paused2, running2, ended2, in_random2};
enum state servo_state;
enum state2 servo_state2;
uint8_t opc(uint8_t x) // This function is used to check value of opcode
{
uint8_t lastXbits = x & 224;
return lastXbits;
}

uint8_t param(uint8_t x) // This function is used to check value of parameter
{
unsigned  mask1;
mask1 = ((1 << 5) - 1) << 0;
uint8_t isolatedXbits = x & mask1;
return isolatedXbits;
}


int get_action(uint8_t x[], int sum) // Executes recipe for servo 1. Takes in recipe array and size as input
{

	current_recipe = x; // pointer to recipe array
if(index1 < sum && servo_state != ended) // Checks if array index is within range and servo state has not ended
{
	if(servo == pause){ // If servo is paused
		set_led_1(0); // LED 1 off
		set_led_3(0); // LED 3 on
	}
	else if (servo == cont){ // If servo is running normally
		set_led_1(1); // LED 1 on
		set_led_3(0); // LED 3 off
		 if(servo_state == in_random){ // If servo is executing special opcode
				set_led_1(1); // LED 1 on
				set_led_3(0); // LED 3 off
			rando(); // Calls function to execute special opcode
		}
		else{
			set_led_1(1); // LED 1 on
			set_led_3(0); // LED 3 off
		state_space(current_recipe[index1], previous_position); // Call state space function to execute servo's next move
		if(servo == looping){ // Check if servo 1 is in a loop
			set_led_1(1); // LED 1 on
			set_led_3(0); // LED 3 off
			;
		}
		else{
			set_led_1(1); // LED 1 on
			set_led_3(0); // LED 3 off
		index1++; // Increment index value
		}
		}
	}
	else if (servo == begin){ // Check if servo is being restarted or started
		set_led_1(1); // LED 1 on
		set_led_3(0); // LED 3 off
		index1 = 0; // Change index value back to 0
		servo_state = running; // Change servo state to running
		servo = cont; // Change servo 1 to to be in continue
	}

	else if (servo == looping){ // If servo is looping
		set_led_1(1); // LED 1 on
		set_led_3(0); // LED 3 off
		loop(); // Execute loop function
	}
	else if (servo == nothing){ // In case servo is told to do nothing
		;
	}
	else if(servo == delaying){ // If servo is told to wait
		set_led_1(1); // LED 1 on
		set_led_3(0); // LED 3 off
		servo1_wait(); // Execute the wait function for servo 1
	}

	else{
	if(opc(current_recipe[index1])==0)  // If recipe has ended
	{
		set_led_1(0); // LED 1 off
		set_led_3(0);  // LED 3 off
		s = sprintf((char *)err, "End2!\r\n");
		servo_state = ended; // Change servo state to end
		HAL_UART_Transmit_IT (&huart2, err, s);
	}
	else{
		set_led_1(1); // LED 1 on
		set_led_3(0); // LED 3 off
	state_space(current_recipe[index1], previous_position); // Call state space function to execute servo's next move

	index1++; // Increment index value
	}
	}

}
else if(servo == looperr){ // If servo enters loop error state
	set_led_1(1);
	set_led_4(1);
	}
else if(servo== wrong){ // If wrong recipe command is entered
	set_led_1(1);
	set_led_3(1);
}

else{
	set_led_1(0);
	set_led_3(0);
	servo_state = ended;
}
}

int loop() //Function to execute loop command
{

	if(opc(current_recipe[j])!=160){ // Check if end loop command has been reached
		if(opc(current_recipe[j])==128){ // Check if a nested loop is detected
			set_led_1(1);
			set_led_4(1);
			servo_state = ended; // Change servo state to end
			servo = looperr; // Servo is in loop error state
			s = sprintf((char *)err, "ERl1!\r\n");
			HAL_UART_Transmit_IT (&huart2, err, s);
		}
		j++; // Increment until loop end command is found

	}


	else{
		if(n < loop_param) // Check if current iteration is within given parameter range
		{  // initialize n to be 0
			if(k == j) // Check if iteration has reached the end
			{
				set_led_1(1);
				set_led_3(0);
				n++; //Indicates that an iteration has been completed, and updates the iteration value
				k = index1+1; // Point to the first command in loop
			}
			else if(k < j)
			{ // initialize k to be index1+1
				set_led_1(1);
				set_led_3(0);
				state_space(current_recipe[k], previous_position); // Call state space function to execute servo's next move
				loop_flag = 1; // Assign a flag if servo is looping
				//servo = looping;
				k++; // Move to next command within loop
			}


		}

		else
		{
			index1 = j+1; // Once loop has ended point to command after end loop
			loop_flag = 0; // assign flag once loop has ended
			set_led_1(1);
			set_led_3(0);
			servo = cont; // Set servo to continue
		}
	}
}


void servo1_wait() // Function to make a servo delay after moving to a new position
{
	if(wait_servo1>0){ // Check if servo still needs to wait
		wait_servo1--; // Decrement wait variable
	}
	else{
		if(loop_flag == 1){ // Check if servo is looping
			set_led_1(1);
			set_led_3(0);
			servo = looping; // Need to change to looping since servo will be in continue mode before exiting
		}
		else{
			loop_flag = 0; // If servo not looping
			set_led_1(1);
			set_led_3(0);
		servo = cont; // Set servo to be in continue mode
		}
	}
}

void rando() // Function to execute special opcode. Moves to user given number of random positions.
{
		if(r<ran){ // Check if index value of iteration is less than input parameter
			q = rand()%6; // Get a random position from 0 to 5
			TIM2->CCR1 = ((q+1)*20)+10; // Move servo to that position
			wait_servo1 = abs(q - previous_position); //Assign delay value
			servo = delaying; // Put servo in delay state
			servo1_wait(); // Call delay function
			previous_position = q; // Change current position to previous position
			r++; // Increment index value

		}
		else{
			r = 0; // Change value of r to 0
			set_led_1(1);
			set_led_3(0);
			servo_state == running; // Assign servo to be in running state
			index1++; // Increment index value
		}

}


void state_space(int current, int previous) //Executes servo's next move based on current command and previous position
{
	if (opc(current) == 32) // if opcode is MOV
	{
		if(param(current)<6){ // Check if parameter is within range
		TIM2->CCR1 = ((param(current)+1)*20)+10; // Move servo 1
		wait_servo1 = abs(param(current) - previous); // Assign delay value
		servo = delaying; // Put servo in delay state
		servo1_wait(); // Call delay function
		previous_position = param(current); // Change current position to previous position
		}
		else{
			servo = wrong; // Else put servo in wrong state if out of bounds
			set_led_1(1);
			set_led_3(1);

		}
	}

	else if(opc(current)== 64) //if opcode is WAIT
	{
		if(param(current)<32){ // Check if parameter within bounds

		if(servo == waiting){ // Check if servo is in waiting condition
			if(w>0) // Check if parameter is greater than 0
			{
				w--; // Decrement towards zero
				index1--; // Reduce index value so that it gets added back to same value again
			}
			else{
				servo = cont; // If waiting period is over, put servo in continue mode
			}
		}
		else{
			w = (param(current)+1)*3; // Assign variable for servo wait time
			servo = waiting; // Put servo in waiting mode
		}
		}
		else{
			servo = wrong; // If parameter out of bounds, put servo in wrong mode
			set_led_1(1);
			set_led_3(1);
		}
	}

	else if(opc(current)== 128) //for LOOP
	{
		if(param(current)<32){ // Check if parameter is within bounds
		if(param(current)==0){ // If parameter is zero, ignore the loop condition and move on with the program
			;
		}
		else
		{
			j = index1+1; //Point j to first command in loop
			k = index1+1; // Point k to first command in loop
			n = 0; // Initialize n as 0
			servo = looping; // Put servo in looping condition
			loop_param = param(current); // Initialize loop parameter
			loop(); // Call loop function
		}
		}
		else{
			servo = wrong; // If out of bounds
			set_led_1(1);
			set_led_3(1);
		}
	}

	else if(opc(current)== 160) //for END_LOOP
	{
		; // If this opcode is encountered in case of no loop or after a loop is finished, it is ignored
	}

	else if(opc(current)== 0) //for RECIPE_END
	{
		s = sprintf((char *)err, "End3!\r\n"); // Incase a recipe is ended, put servo in end state
		set_led_1(0);
		set_led_3(0);
				servo_state = ended;
				HAL_UART_Transmit_IT (&huart2, err, s);
				//delay(100);
	}

	else if(opc(current)== 96) //for S1
	{
		servo_state = in_random; // Put servo in special state for new opcode
		ran = param(current); // Assign parameter for number of random positions
		index1--; // Decrement index value until all number of positions are visited
		rando(); // Call function for special opcode
	}

	else if(opc(current)== 192) //for S2
	{
		set_led_1(0);
		set_led_3(1);
		s = sprintf((char *)err, "ERR12!\r\n");
		HAL_UART_Transmit_IT (&huart2, err, s);
		servo_state = ended; // Error state for this opcode
	}

	else if(opc(current)== 224) //for S3
	{
		set_led_1(0);
		set_led_3(1);
		s = sprintf((char *)err, "ERR13!\r\n");
		HAL_UART_Transmit_IT (&huart2, err, s);
		servo_state = ended; // Error state for this opcode
	}

	else{
		servo = wrong; // If any other opcode is entered, servo goes into wrong condition
		set_led_1(1);
		set_led_3(1);
	}
}

// The exact same procedure is followed for servo 2 in the below functions.

int get_action2(uint8_t x[], int sum) // takes in recipe
{

	current_recipe2 = x;
if(index2 < sum && servo_state2 != ended2)
{
	if(servo2 == pause2){
		set_led_2(0);
	}
	else if (servo2 == cont2){
		set_led_2(1);
		 if(servo_state2 == in_random2){
			rando2();
		}
		else{
		state_space2(current_recipe2[index2], previous_position2);
		if(servo2 == looping2){
			;
		}
		else{
		index2++;
		}
		}
	}
	else if (servo2 == begin2){
		set_led_2(1);

		index2 = 0;
		servo_state2 = running2;
		servo2 = cont2;
	}

	else if (servo2 == looping2){
		set_led_2(1);

		loop2();
	}
	else if (servo2 == nothing2){
		;
	}
	else if(servo2 == delaying2){
		set_led_2(1);

		servo2_wait();
	}

	else{
	if(opc(current_recipe2[index2])==0)
	{
		s = sprintf((char *)err, "End2!\r\n");
		set_led_2(0);

		servo_state2 = ended2;
		HAL_UART_Transmit_IT (&huart2, err, s);
	}
	else{
		set_led_2(1);

	state_space2(current_recipe2[index2], previous_position2);

	index2++;

	}
	}
}
else if(servo2 == looperr2){
	set_led_2(1);

}
else{
	servo_state2 = ended2;
	set_led_2(0);
}
}

int loop2() //takes in loop parameter
{

	if(opc(current_recipe2[j2])!=160){
		if(opc(current_recipe2[j2])==128){
			set_led_2(1);

			servo_state2 = ended2;
			servo2 = looperr2;
			s = sprintf((char *)err, "ERl1!\r\n");
			HAL_UART_Transmit_IT (&huart2, err, s);
		}
		j2++;

	}


	else{
		if(n2 < loop_param2)
		{  // initialize n to be 0
			if(k2 == j2)
			{
				n2++;
				k2 = index2+1;
			}
			else if(k2 < j2)
			{ // initialize k to be index1+1
				state_space2(current_recipe2[k2], previous_position2);
				loop_flag2 = 1;
				//servo = looping;
				k2++;
			}


		}

		else
		{
			index2 = j2+1;
			loop_flag2 = 0;
			servo2 = cont2;
		}
	}
}

void servo2_wait()
{
	if(wait_servo2>0){
		wait_servo2--;
	}
	else{
		if(loop_flag2 == 1){
			servo2 = looping2;
		}
		else{
			loop_flag2 = 0;
		servo2 = cont2;
		}
	}
}

void rando2()
{
		if(r2<ran2){
			q2 = rand()%6;
			TIM2->CCR2 = ((q2+1)*20)+10;
			wait_servo2 = abs(q2 - previous_position2);
			servo2 = delaying2;
			servo2_wait();
			previous_position2 = q2;
			r2++;

		}
		else{
			r2 = 0;
			servo_state2 == running2;
			index2++;
		}

}


void state_space2(int current, int previous) //takes in current command, prev position
{
	if (opc(current) == 32) // for MOV
	{
		if(param(current)<6){
		TIM2->CCR2 = ((param(current)+1)*20)+10;

		wait_servo2 = abs(param(current) - previous);
		servo2 = delaying2;
		servo2_wait();
		previous_position2 = param(current);
		}
		else{

		}

	}

	else if(opc(current)== 64) //for WAIT
	{

		if(param(current)<32){

		if(servo2 == waiting2){
			if(w2>0)
			{
				w2--;
				index2--;
			}
			else{
				servo2 = cont2;
			}
		}
		else{
			w2 = (param(current)+1)*3;
			servo2 = waiting2;
		}
		}
		else{

		}
	}

	else if(opc(current)== 128) //for LOOP
	{
		if(param(current)<32){
		if(param(current)==0){
			;
		}
		else
		{
			j2 = index2+1;
			k2 = index2+1;
			n2 = 0;
			servo2 = looping2;
			loop_param2 = param(current);
			loop2();
		}
		}
		else{

		}
	}

	else if(opc(current)== 160) //for END_LOOP
	{
		;
	}

	else if(opc(current)== 0) //for RECIPE_END
	{
		s = sprintf((char *)err, "End3!\r\n");
				servo_state2 = ended2;

				HAL_UART_Transmit_IT (&huart2, err, s);

	}

	else if(opc(current)== 96) //for S1
	{
		servo_state2 = in_random2;
		ran2 = param(current);
		index2--;
		rando2();
	}

	else if(opc(current)== 192) //for S2
	{
		s = sprintf((char *)err, "ERR12!\r\n");
		HAL_UART_Transmit_IT (&huart2, err, s);
		servo_state2 = ended2;

	}

	else if(opc(current)== 224) //for S3
	{
		s = sprintf((char *)err, "ERR13!\r\n");
		HAL_UART_Transmit_IT (&huart2, err, s);
		servo_state2 = ended2;

	}

	else{
	}
}


void user(char servo1, char servo22)
{
	switch(servo1)
	{
	case 'P': servo = pause; // Put servo in pause condition
			break;

	case 'C':servo = cont; // Put servo in continue condition
			servo_state = running; // Put servo in running state
			break;
	case 'R': if(servo == pause || servo_state == ended){ // Check if either servo is paused or recipe has ended
		if(TIM2->CCR1 >= 50) // Make sure servo moves within bounds
		{
			TIM2->CCR1 -= 20; // Move the servo to right
		}
	}
			break;

	case 'L':if(servo == pause || servo_state == ended){ // Check if either servo is paused or recipe has ended
		if(TIM2->CCR1 <= 110) // Make sure servo moves within bounds
		{
			TIM2->CCR1 += 20; // Move the servo to left
		}
	}
			break;

	case 'N':servo = nothing; // Put servo in nothing condition
		break;

	case 'B': servo = begin; // Put servo in begin or restart condition
				index1 = 0; // Assign index to be 0 again
				servo_state = running; // Put servo in running state
			  break;

	}




	switch(servo22)
	{
	case 'p': servo2 = pause2; // pause tim
			break;

	case 'c':servo2 = cont2;
			servo_state2 = running2;
			break;
	case 'r': if(servo2 == pause2 || servo_state2 == ended2){
		if(TIM2->CCR2 >= 50)
		{
			TIM2->CCR2 -= 20;
		}
	}
			break;

	case 'l':if(servo2 == pause2 || servo_state2 == ended2){
		if(TIM2->CCR2 <= 110)
		{
			TIM2->CCR2 += 20;
		}
	}
			break;

	case 'n':servo2 = nothing2;
		break;

	case 'b': servo2 = begin2;
				index2 = 0;
				servo_state2 = running2;
			  break;


}

}



