#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include "pwm.h"

static const struct pwm_dt_spec my_pwm0 = PWM_DT_SPEC_GET(DT_ALIAS(my_pwm0));

// ------------ MOTOR Defines ------------ //
// P1.01 - Enable - IN2 - 1 channel
// P1.03 - Phase - IN1 - 2 channel
// P1.00 - nSleep

#define Enable 1
#define Phase 2
#define NSLEEP DT_ALIAS(nsleep)

#define Motor_Period       PWM_KHZ(3)
#define Motor_Pulse_width  PWM_KHZ(3) //TODO add divider based on battery voltage

#define Turn_OFF_Period       PWM_MSEC(20)
#define Turn_OFF_Pulse_width  PWM_MSEC(0)

int32_t motor_Pulse_width_var_HZ;
int32_t motor_Period_HZ = 3000;

static const struct gpio_dt_spec nsleep_io = GPIO_DT_SPEC_GET(NSLEEP, gpios);
// ------------ MOTOR Defines ------------ //

// ------------ BUZZER Defines ------------ //
#define Buzzer_Period       PWM_KHZ(3)
#define Buzzer_Pulse_width  PWM_KHZ(6)

static void buzzer_off_after_delay(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(buzzer_working_time_ms, buzzer_off_after_delay);
// ------------ BUZZER Defines ------------ //

//-------------------------------------------------------------//
void buzzer_on(int work_time_ms)
//-------------------------------------------------------------//
{
    pwm_set_dt(&my_pwm0, Buzzer_Period, Buzzer_Pulse_width);
    k_work_reschedule(&buzzer_working_time_ms, K_MSEC(work_time_ms));
}
//-------------------------------------------------------------//
void buzzer_off()
//-------------------------------------------------------------//
{
    pwm_set_dt(&my_pwm0, Turn_OFF_Period, Turn_OFF_Pulse_width);
}
//-------------------------------------------------------------//
static void buzzer_off_after_delay(struct k_work *work)
//-------------------------------------------------------------//
{
    ARG_UNUSED(work);
    buzzer_off();
}

//-------------------------------------------------------------//
void valve_open()
//-------------------------------------------------------------//
{
    gpio_pin_set_dt(&nsleep_io, 1);
    pwm_set(my_pwm0.dev, 
            Phase, 
            Turn_OFF_Period, 
            Turn_OFF_Pulse_width, 
            PWM_POLARITY_NORMAL);
    pwm_set(my_pwm0.dev, 
            Enable, 
            Motor_Period, 
            Motor_Pulse_width, 
            PWM_POLARITY_NORMAL);
}
//-------------------------------------------------------------//
void valve_close()
//-------------------------------------------------------------//
{
    gpio_pin_set_dt(&nsleep_io, 1);
    pwm_set(my_pwm0.dev, 
            Phase, 
            Motor_Period, 
            Motor_Pulse_width, 
            PWM_POLARITY_NORMAL);
    pwm_set(my_pwm0.dev, 
            Enable, 
            Turn_OFF_Period, 
            Turn_OFF_Pulse_width, 
            PWM_POLARITY_NORMAL);
}
//-------------------------------------------------------------//
void motor_sleep()
//-------------------------------------------------------------//
{
    gpio_pin_set_dt(&nsleep_io, 0);
    pwm_set(my_pwm0.dev, 
            Phase, 
            Turn_OFF_Period, 
            Turn_OFF_Pulse_width, 
            PWM_POLARITY_NORMAL);
    pwm_set(my_pwm0.dev, 
            Enable, 
            Turn_OFF_Period, 
            Turn_OFF_Pulse_width, 
            PWM_POLARITY_NORMAL);
}
//-------------------------------------------------------------//
void set_motor_pwm(int32_t current_bat_voltage)
//-------------------------------------------------------------//
{
    int32_t pwm_percent = (5000 * 100)/current_bat_voltage;
    motor_Pulse_width_var_HZ = (motor_Period_HZ/100) * pwm_percent;
}

//-------------------------------------------------------------//
void configure_gpio(void)
//-------------------------------------------------------------//
{
	gpio_pin_configure_dt(&nsleep_io, GPIO_OUTPUT);
    motor_sleep();
}