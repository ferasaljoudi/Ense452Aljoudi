#ifndef __TIMER_H__
#define __TIMER_H__

#include "stm32f1xx_hal.h"
#include <stdint.h>

void timer_init(void);
uint16_t timer_start(void);
uint16_t timer_stop(uint16_t start_time);
void run_timing_tests(void);

#endif
