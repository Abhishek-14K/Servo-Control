/*
 * recipe.h
 *
 *  Created on: Feb 23, 2022
 *      Author: abhis
 */

#ifndef INC_RECIPE_H_
#define INC_RECIPE_H_

uint8_t opc(uint8_t x);
uint8_t param(uint8_t x);
int get_action(int x[], int servo);
int loop(int n, int servo);
void state_space(int current, int previous, int servo);


#endif /* INC_RECIPE_H_ */
