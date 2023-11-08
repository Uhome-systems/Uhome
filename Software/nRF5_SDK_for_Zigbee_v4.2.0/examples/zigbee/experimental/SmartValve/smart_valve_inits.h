/////////////////////////////////////////////////////////
// developed by Alex Bolshov at KDE                   ///
/////////////////////////////////////////////////////////
// all related to work logic of water wall is in here ///
/////////////////////////////////////////////////////////
#ifndef SMART_VALVE_INITS_H
#define SMART_VALVE_INITS_H

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-// simple "macros"
#define DRV_8837
#ifdef DRV_8838
#define VALVE_OPEN  nrf_gpio_pin_set(NSLEEP_DRIVER_PIN),  \
                    nrf_gpio_pin_set(PHASE_DRIVER_PIN),   \
                    nrf_gpio_pin_set(ENABLE_DRIVER_PIN);  \

#define VALVE_CLOSE nrf_gpio_pin_set(NSLEEP_DRIVER_PIN),  \
                    nrf_gpio_pin_clear(PHASE_DRIVER_PIN), \
                    nrf_gpio_pin_set(ENABLE_DRIVER_PIN);  \

#define MOTOR_SLEEP nrf_gpio_pin_clear(NSLEEP_DRIVER_PIN),\
                    nrf_gpio_pin_clear(PHASE_DRIVER_PIN), \
                    nrf_gpio_pin_clear(ENABLE_DRIVER_PIN);\

#endif

#ifdef DRV_8837
#define VALVE_OPEN  nrf_gpio_pin_set(NSLEEP_DRIVER_PIN),  \
                    nrf_gpio_pin_clear(PHASE_DRIVER_PIN), \
                    nrf_gpio_pin_set(ENABLE_DRIVER_PIN);  \

#define VALVE_CLOSE nrf_gpio_pin_set(NSLEEP_DRIVER_PIN),  \
                    nrf_gpio_pin_set(PHASE_DRIVER_PIN),   \
                    nrf_gpio_pin_clear(ENABLE_DRIVER_PIN);\

#define MOTOR_SLEEP nrf_gpio_pin_clear(NSLEEP_DRIVER_PIN),\
                    nrf_gpio_pin_clear(PHASE_DRIVER_PIN), \
                    nrf_gpio_pin_clear(ENABLE_DRIVER_PIN);\

#endif 

#define BUTTON_CLOSE_PRESSED !nrf_gpio_pin_read(BUTTON_CLOSE)
#define BUTTON_CLOSE_RELEASED nrf_gpio_pin_read(BUTTON_CLOSE)

#define BUTTON_OPEN_PRESSED !nrf_gpio_pin_read(BUTTON_OPEN)
#define BUTTON_OPEN_RELEASED nrf_gpio_pin_read(BUTTON_OPEN)

#define BUTTON_CONTROL_PRESSED !nrf_gpio_pin_read(BUTTON_CONTROL)
#define BUTTON_CONTROL_RELEASED nrf_gpio_pin_read(BUTTON_CONTROL)

#define BUTTON_CONTROL2_PRESSED !nrf_gpio_pin_read(BUTTON_CONTROL2)
#define BUTTON_CONTROL2_RELEASED nrf_gpio_pin_read(BUTTON_CONTROL2)

#define COMMAND_TO_OPEN_VALVE value//really 1 - on open, 0 off closed
#define COMMAND_TO_CLOSE_VALVE !value//need to change also VALVE_CLOSE VALVE_OPEN

#define VALVE_LED_ON    nrf_gpio_pin_clear(SMART_VALVE_LED);
#define VALVE_LED_OFF   nrf_gpio_pin_set(SMART_VALVE_LED);

#define BUZZER_ON       nrf_gpio_pin_set(BUZZER);
#define BUZZER_OFF      nrf_gpio_pin_clear(BUZZER);
#define BUZZER_TOGGLE   nrf_gpio_pin_toggle(BUZZER);
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-// simple "macros" END


//----------------------------//
void ww_driver_pins_init()
//----------------------------//
{
    bsp_board_leds_off();
    //bsp_board_leds_on();

    //nrf_gpio_cfg_input(BATTERY_VOLTAGE_PIN,NRF_GPIO_PIN_NOPULL);

    nrf_gpio_cfg_output(BUZZER);

    nrf_gpio_cfg_output(NSLEEP_DRIVER_PIN);
    nrf_gpio_cfg_output(ENABLE_DRIVER_PIN);
    nrf_gpio_cfg_output(PHASE_DRIVER_PIN);

    nrf_gpio_pin_clear(NSLEEP_DRIVER_PIN);
    nrf_gpio_pin_clear(PHASE_DRIVER_PIN);
    nrf_gpio_pin_clear(ENABLE_DRIVER_PIN);

    nrf_gpio_cfg_sense_set(BUTTON_PAIRING,NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_set(BUTTON_OPEN,NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_set(BUTTON_CLOSE,NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_set(BUTTON_CONTROL,NRF_GPIO_PIN_SENSE_LOW);//NRF_GPIO_PIN_SENSE_HIGH //NRF_GPIO_PIN_SENSE_LOW
    nrf_gpio_cfg_sense_set(BUTTON_CONTROL2,NRF_GPIO_PIN_SENSE_LOW);

    bsp_event_to_button_action_assign(BUTTON_CONTROL_LIST_NUMBER, BSP_BUTTON_ACTION_RELEASE, BSP_EVENT_KEY_6);
    bsp_event_to_button_action_assign(BUTTON_CONTROL2_LIST_NUMBER, BSP_BUTTON_ACTION_RELEASE, BSP_EVENT_KEY_7);
    bsp_event_to_button_action_assign(BUTTON_PAIRING_LIST_NUMBER, BSP_BUTTON_ACTION_LONG_PUSH, BSP_EVENT_RESET);

    bsp_event_to_button_action_assign(BUTTON_PAIRING_LIST_NUMBER, BSP_BUTTON_ACTION_RELEASE, BSP_EVENT_KEY_5);
}
//---------------------------------------------//
static void timers_init(void)
//---------------------------------------------//
{
    ret_code_t err_code;

    // Initialize timer module.
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

#endif