#ifndef _LEDS_H_
#define _LEDS_H_

enum leds {
    PAIRING_LED_ENUM,
    ERROR_LED_ENUM,
    OPEN_STATUS_LED_ENUM,
    CLOSED_STATUS_LED_ENUM
};

int led_blink(enum leds name, signed int time_to_blink_ms);

int led_control(enum leds name, bool on);

int leds_init(void);

#endif /* _LEDS_H_ */