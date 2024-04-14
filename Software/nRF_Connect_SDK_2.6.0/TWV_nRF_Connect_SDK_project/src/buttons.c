#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "buttons.h"

#define PAIRING_BUTTON_NODE	DT_ALIAS(pairing)
#if !DT_NODE_HAS_STATUS(PAIRING_BUTTON_NODE, okay)
#error "Unsupported board: devicetree alias is not defined"
#endif

#define OPEN_BUTTON_NODE	DT_ALIAS(open)
#if !DT_NODE_HAS_STATUS(OPEN_BUTTON_NODE, okay)
#error "Unsupported board: devicetree alias is not defined"
#endif

#define CLOSE_BUTTON_NODE	DT_ALIAS(close)
#if !DT_NODE_HAS_STATUS(CLOSE_BUTTON_NODE, okay)
#error "Unsupported board: devicetree alias is not defined"
#endif

#define CONTROL1_BUTTON_NODE	DT_ALIAS(control1)
#if !DT_NODE_HAS_STATUS(CONTROL1_BUTTON_NODE, okay)
#error "Unsupported board: devicetree alias is not defined"
#endif

#define CONTROL2_BUTTON_NODE	DT_ALIAS(control2)
#if !DT_NODE_HAS_STATUS(CONTROL2_BUTTON_NODE, okay)
#error "Unsupported board: devicetree alias is not defined"
#endif

bool pairing_button_previuos_state = false;

static const struct gpio_dt_spec button_pairing = GPIO_DT_SPEC_GET_OR(PAIRING_BUTTON_NODE, gpios,{0});
static const struct gpio_dt_spec button_open = GPIO_DT_SPEC_GET_OR(OPEN_BUTTON_NODE, gpios,{0});
static const struct gpio_dt_spec button_close = GPIO_DT_SPEC_GET_OR(CLOSE_BUTTON_NODE, gpios,{0});
static const struct gpio_dt_spec button_control1 = GPIO_DT_SPEC_GET_OR(CONTROL1_BUTTON_NODE, gpios,{0});
static const struct gpio_dt_spec button_control2 = GPIO_DT_SPEC_GET_OR(CONTROL2_BUTTON_NODE, gpios,{0});

static struct gpio_callback button_pairing_cb_data;
static struct gpio_callback button_open_cb_data;
static struct gpio_callback button_close_cb_data;
static struct gpio_callback button_control1_cb_data;
static struct gpio_callback button_control2_cb_data;


static button_event_handler_t user_cb;
bool get_button_open_pressed()
{
    if(gpio_pin_get_dt(&button_open)) return true;
    else return false;
}
bool get_button_close_pressed()
{
    if(gpio_pin_get_dt(&button_close)) return true;
    else return false;
}
bool get_button_control1_pressed()
{
    if(gpio_pin_get_dt(&button_control1)) return true;
    else return false;
}
bool get_button_control2_pressed()
{
    if(gpio_pin_get_dt(&button_control2)) return true;
    else return false;
}
//////////////////////////////////////////////////////
//TODO just made it fast, need to make it better looking
static void cooldown_expired_pairing(struct k_work *work)
{
    ARG_UNUSED(work);

    int val = gpio_pin_get_dt(&button_pairing);
    if (val) pairing_button_previuos_state = true;
    else pairing_button_previuos_state = false;
    enum button_evt evt = val ? BUTTON_PAIRING_EVT_PRESSED : BUTTON_PAIRING_EVT_RELEASED;
    if (user_cb) {
        user_cb(evt);
    }
}
static void cooldown_expired_open(struct k_work *work)
{
    ARG_UNUSED(work);

    int val = gpio_pin_get_dt(&button_open);
    enum button_evt evt2 = val ? BUTTON_OPEN_EVT_PRESSED : BUTTON_OPEN_EVT_RELEASED;
    if (user_cb) {
        user_cb(evt2);
    }
}
static void cooldown_expired_close(struct k_work *work)
{
    ARG_UNUSED(work);

    int val = gpio_pin_get_dt(&button_close);
    enum button_evt evt3 = val ? BUTTON_CLOSE_EVT_PRESSED : BUTTON_CLOSE_EVT_RELEASED;
    if (user_cb) {
        user_cb(evt3);
    }
}
static void cooldown_expired_control1(struct k_work *work)
{
    ARG_UNUSED(work);

    int val = gpio_pin_get_dt(&button_control1);
    enum button_evt evt4 = val ? BUTTON_CONTROL_1_EVT_PRESSED : BUTTON_CONTROL_1_EVT_RELEASED;
    if (user_cb) {
        user_cb(evt4);
    }
}
static void cooldown_expired_control2(struct k_work *work)
{
    ARG_UNUSED(work);

    int val = gpio_pin_get_dt(&button_control2);
    enum button_evt evt5 = val ? BUTTON_CONTROL_2_EVT_PRESSED : BUTTON_CONTROL_2_EVT_RELEASED;
    if (user_cb) {
        user_cb(evt5);
    }
}
////////////////////////////////////////////////////////////////////////
static K_WORK_DELAYABLE_DEFINE(cooldown_work_pairing, cooldown_expired_pairing);
static K_WORK_DELAYABLE_DEFINE(cooldown_work_open, cooldown_expired_open);
static K_WORK_DELAYABLE_DEFINE(cooldown_work_close, cooldown_expired_close);
static K_WORK_DELAYABLE_DEFINE(cooldown_work_control1, cooldown_expired_control1);
static K_WORK_DELAYABLE_DEFINE(cooldown_work_control2, cooldown_expired_control2);
////////////////////////////////////////////////////////////////////////
void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{//TODO//-=-=-=-=-=-=-=-=-=-
//TODO//-=-=-=-=-=-=-=-=-=-
//TODO//-=-=-=-=-=-=-=-=-=-
//TODO//-=-=-=-=-=-=-=-=-=-
//TODO//-=-=-=-=-=-=-=-=-=-
    if(cb->pin_mask == BIT(button_pairing.pin)) 
    {
        if(pairing_button_previuos_state) k_work_reschedule(&cooldown_work_pairing, K_MSEC(50));
        else k_work_reschedule(&cooldown_work_pairing, K_MSEC(3000));
    }
    if(cb->pin_mask == BIT(button_open.pin)) k_work_reschedule(&cooldown_work_open, K_MSEC(50));
    if(cb->pin_mask == BIT(button_close.pin)) k_work_reschedule(&cooldown_work_close, K_MSEC(50));
    if(cb->pin_mask == BIT(button_control1.pin)) k_work_reschedule(&cooldown_work_control1, K_MSEC(50));
    if(cb->pin_mask == BIT(button_control2.pin)) k_work_reschedule(&cooldown_work_control2, K_MSEC(50));
}

int button_init(button_event_handler_t handler)
{
    int err = -1;

    if (!handler) {
        return -EINVAL;
    }

    user_cb = handler;

// check if port is configured //
	if (!device_is_ready(button_pairing.port) ||
        !device_is_ready(button_open.port) ||
        !device_is_ready(button_close.port) ||
        !device_is_ready(button_control1.port) ||
        !device_is_ready(button_control2.port)) {
		return -EIO;
	}
// check if port is configured //

// configure buttons to input //
	err = gpio_pin_configure_dt(&button_pairing, GPIO_INPUT);
	err = gpio_pin_configure_dt(&button_open, GPIO_INPUT);
	err = gpio_pin_configure_dt(&button_close, GPIO_INPUT);
	err = gpio_pin_configure_dt(&button_control1, GPIO_INPUT);
	err = gpio_pin_configure_dt(&button_control2, GPIO_INPUT);
    if (err) {
       return err;
	}
// configure buttons to input //

// configure interrupt on both edges //
	err = gpio_pin_interrupt_configure_dt(&button_pairing,GPIO_INT_EDGE_BOTH);
	err = gpio_pin_interrupt_configure_dt(&button_open,GPIO_INT_EDGE_BOTH);
	err = gpio_pin_interrupt_configure_dt(&button_close,GPIO_INT_EDGE_BOTH);
	err = gpio_pin_interrupt_configure_dt(&button_control1,GPIO_INT_EDGE_BOTH);
	err = gpio_pin_interrupt_configure_dt(&button_control2,GPIO_INT_EDGE_BOTH);
	if (err) {
		return err;
	}
// configure interrupt on both edges //

// init and add callback for each button //
	gpio_init_callback(&button_pairing_cb_data, button_pressed, BIT(button_pairing.pin));
    err = gpio_add_callback(button_pairing.port, &button_pairing_cb_data);

	gpio_init_callback(&button_open_cb_data, button_pressed, BIT(button_open.pin));
    err = gpio_add_callback(button_open.port, &button_open_cb_data);

	gpio_init_callback(&button_close_cb_data, button_pressed, BIT(button_close.pin));
    err = gpio_add_callback(button_close.port, &button_close_cb_data);

	gpio_init_callback(&button_control1_cb_data, button_pressed, BIT(button_control1.pin));
    err = gpio_add_callback(button_control1.port, &button_control1_cb_data);

	gpio_init_callback(&button_control2_cb_data, button_pressed, BIT(button_control2.pin));
	err = gpio_add_callback(button_control2.port, &button_control2_cb_data);
    if (err) {
        return err;
    }
// init and add callback for each button //
    return 0;
}
