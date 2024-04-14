#ifndef _PWM_H_
#define _PWM_H_

void buzzer_on(int work_time_ms);
void buzzer_off(void);

void valve_open(void);
void valve_close(void);
void motor_sleep(void);

void set_motor_pwm(int32_t current_bat_voltage);

void configure_gpio(void);

#endif /* _PWM_H_ */