/*
 * tasks.h
 *
 *  Created on: Sep 24, 2018
 *      Author: nihit
 */

#ifndef TASKS_H_
#define TASKS_H_
#include "kernel.h"

void idle(void);
uint8_t readPbs();
void debounce(void);


#endif /* TASKS_H_ */
