/*
 * hal_pwm.c
 *
 *  Created on: 2017Äê8ÔÂ26ÈÕ
 *      Author: lort
 */

#include "hal_pwm.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "external_include.h"

void hal_pwmInit(void)
{
    //1. mcpwm gpio initialization
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, 5);    //Set GPIO 5 as PWM0A, to which servo is connected

    //2. initial mcpwm configuration
    printf("Configuring Initial Parameters of mcpwm......\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 10000;    //frequency = 50Hz, i.e. for every servo motor time period should be 20ms
    pwm_config.cmpr_a = 500;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 500;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_1;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
}
void hal_pwmSet(float angle)
{
	mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, angle);
	//mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);  //call this each time, if operator was previously in low/high state
	mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_0);
}


