#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "leds.h"

#define PAIRING_LED DT_ALIAS(led0)
#define ERROR_LED DT_ALIAS(led1)
#define OPEN_STATUS_LED DT_ALIAS(led2)
#define CLOSED_STATUS_LED DT_ALIAS(led3)

static const struct gpio_dt_spec pairing_led_io = GPIO_DT_SPEC_GET(PAIRING_LED, gpios);
static const struct gpio_dt_spec error_led_io = GPIO_DT_SPEC_GET(ERROR_LED, gpios);
static const struct gpio_dt_spec open_status_led_io = GPIO_DT_SPEC_GET(OPEN_STATUS_LED, gpios);
static const struct gpio_dt_spec close_status_led_io = GPIO_DT_SPEC_GET(CLOSED_STATUS_LED, gpios);

#define blink_time_ms 500

signed int full_time_of_blink_ms_pairing = 0;
signed int full_time_of_blink_ms_error = 0;
signed int full_time_of_blink_ms_open = 0;
signed int full_time_of_blink_ms_close = 0;

static void led_blink_delay_pairing(struct k_work *work);
static void led_blink_delay_error(struct k_work *work);
static void led_blink_delay_open(struct k_work *work);
static void led_blink_delay_close(struct k_work *work);

static K_WORK_DELAYABLE_DEFINE(led_blink_delay_ms_pairing, led_blink_delay_pairing);
static K_WORK_DELAYABLE_DEFINE(led_blink_delay_ms_error, led_blink_delay_error);
static K_WORK_DELAYABLE_DEFINE(led_blink_delay_ms_open, led_blink_delay_open);
static K_WORK_DELAYABLE_DEFINE(led_blink_delay_ms_close, led_blink_delay_close);

static void led_blink_delay_pairing(struct k_work *work)
{
    ARG_UNUSED(work);
    gpio_pin_toggle_dt(&pairing_led_io);
    full_time_of_blink_ms_pairing -= blink_time_ms;
    if(full_time_of_blink_ms_pairing)k_work_reschedule(&led_blink_delay_ms_pairing, K_MSEC(blink_time_ms));
    else full_time_of_blink_ms_pairing = 0, gpio_pin_set_dt(&pairing_led_io, false);
}
static void led_blink_delay_error(struct k_work *work)
{
    ARG_UNUSED(work);
    gpio_pin_toggle_dt(&error_led_io);
    full_time_of_blink_ms_error -= blink_time_ms;
    if(full_time_of_blink_ms_error)k_work_reschedule(&led_blink_delay_ms_error, K_MSEC(blink_time_ms));
    else full_time_of_blink_ms_error = 0, gpio_pin_set_dt(&error_led_io, false);
}
static void led_blink_delay_open(struct k_work *work)
{
    ARG_UNUSED(work);
    gpio_pin_toggle_dt(&open_status_led_io);
    full_time_of_blink_ms_open -= blink_time_ms;
    if(full_time_of_blink_ms_open)k_work_reschedule(&led_blink_delay_ms_open, K_MSEC(blink_time_ms));
    else full_time_of_blink_ms_open = 0, gpio_pin_set_dt(&open_status_led_io, false);
}
static void led_blink_delay_close(struct k_work *work)
{
    ARG_UNUSED(work);
    gpio_pin_toggle_dt(&close_status_led_io);
    full_time_of_blink_ms_close -= blink_time_ms;
    if(full_time_of_blink_ms_close)k_work_reschedule(&led_blink_delay_ms_close, K_MSEC(blink_time_ms));
    else full_time_of_blink_ms_close = 0, gpio_pin_set_dt(&close_status_led_io, false);
}

int led_control(enum leds name, bool on)
{
    int error = 0;
    switch (name) 
    {
        case PAIRING_LED_ENUM:
            error = gpio_pin_set_dt(&pairing_led_io, on);
			break;
        case ERROR_LED_ENUM:
            error = gpio_pin_set_dt(&error_led_io, on);
			break;
        case OPEN_STATUS_LED_ENUM:
            error = gpio_pin_set_dt(&open_status_led_io, on);
			break;
        case CLOSED_STATUS_LED_ENUM:
            error = gpio_pin_set_dt(&close_status_led_io, on);
			break;
    }
    return error;
}

int led_blink(enum leds name, signed int time_to_blink_ms)
{
    int error = 0;
    switch (name) 
    {
        case PAIRING_LED_ENUM:
            error = gpio_pin_set_dt(&pairing_led_io, true);
            k_work_reschedule(&led_blink_delay_ms_pairing, K_MSEC(blink_time_ms));
            full_time_of_blink_ms_pairing = time_to_blink_ms;
			break;
        case ERROR_LED_ENUM:
            error = gpio_pin_set_dt(&error_led_io, true);
            k_work_reschedule(&led_blink_delay_ms_error, K_MSEC(blink_time_ms));
            full_time_of_blink_ms_error = time_to_blink_ms;
			break;
        case OPEN_STATUS_LED_ENUM:
            error = gpio_pin_set_dt(&open_status_led_io, true);
            k_work_reschedule(&led_blink_delay_ms_open, K_MSEC(blink_time_ms));
            full_time_of_blink_ms_open = time_to_blink_ms;
			break;
        case CLOSED_STATUS_LED_ENUM:
            error = gpio_pin_set_dt(&close_status_led_io, true);
            k_work_reschedule(&led_blink_delay_ms_close, K_MSEC(blink_time_ms));
            full_time_of_blink_ms_close = time_to_blink_ms;
			break;
    }
    return error;
}

int leds_init(void)
{
    int error = 0;

    error = gpio_pin_configure_dt(&pairing_led_io, GPIO_OUTPUT);
	error = gpio_pin_configure_dt(&error_led_io, GPIO_OUTPUT);
	error = gpio_pin_configure_dt(&open_status_led_io, GPIO_OUTPUT);
	error = gpio_pin_configure_dt(&close_status_led_io, GPIO_OUTPUT);

    return error;
}