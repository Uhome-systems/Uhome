#ifndef _BUTTONS_H_
#define _BUTTONS_H_

enum button_evt {
    BUTTON_PAIRING_EVT_PRESSED,
    BUTTON_PAIRING_EVT_RELEASED,
    BUTTON_OPEN_EVT_PRESSED,
    BUTTON_OPEN_EVT_RELEASED,
    BUTTON_CLOSE_EVT_PRESSED,
    BUTTON_CLOSE_EVT_RELEASED, 
    BUTTON_CONTROL_1_EVT_PRESSED,
    BUTTON_CONTROL_1_EVT_RELEASED, 
    BUTTON_CONTROL_2_EVT_PRESSED,
    BUTTON_CONTROL_2_EVT_RELEASED
};

typedef void (*button_event_handler_t)(enum button_evt evt);

int button_init(button_event_handler_t handler);

#endif /* _BUTTONS_H_ */