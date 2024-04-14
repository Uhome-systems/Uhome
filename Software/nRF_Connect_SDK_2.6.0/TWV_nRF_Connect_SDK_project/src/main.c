
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>

#include <dk_buttons_and_leds.h>
#include <ram_pwrdn.h>

#include <zboss_api.h>
#include <zboss_api_addons.h>
#include <zigbee/zigbee_app_utils.h>
#include <zigbee/zigbee_error_handler.h>
#include <zb_nrf_platform.h>
#include "zb_mem_config_custom.h"
#include "zb_smart_valve.h"

#include <zigbee/zigbee_fota.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/dfu/mcuboot.h>

#include <pwm.h>
#include <buttons.h>
#include <leds.h>
#include <adc.h>
/* Source endpoint used to control light bulb. */
#define SMART_VALVE_ENDPOINT      1

/* Do not erase NVRAM to save the network parameters after device reboot or
 * power-off. NOTE: If this option is set to ZB_TRUE then do full device erase
 * for all network devices before running other samples.
 */
#define ERASE_PERSISTENT_CONFIG    ZB_FALSE
/* LED indicating that smart valve successfully joind Zigbee network. */
#define ZIGBEE_NETWORK_STATE_LED   DK_LED3

/* Time after which the button state is checked again to detect button hold,
 * the dimm command is sent again.
 */
#define BUTTON_LONG_POLL_TMO       K_MSEC(500)

#if !defined ZB_ED_ROLE
#error Define ZB_ED_ROLE to compile smart valve (End Device) source code.
#endif

static void valve_open_close_handler(bool open,bool close);

uint8_t reboot_couter = 0;

bool valve_need_to_be_open;
bool reverse_status;
bool motor_working = false;

bool pairing_in_progress = false;

bool moving_to_open = false;
bool moving_to_close = false;
bool double_click_awaiting = false;
bool full_press_flag = false;
bool close_from_mid_position_flag = false;
bool logic_need_to_be_restored = false;
bool need_to_auto_close_flag = false;
static void valve_close_with_timer(void);
static void valve_open_with_timer(void);

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);


struct buttons_context {
	uint32_t state;
	atomic_t long_poll;
	struct k_timer alarm;
};

struct zb_device_ctx {
	zb_zcl_basic_attrs_ext_t basic_attr;
	zb_zcl_identify_attrs_t identify_attr;
	zb_zcl_on_off_attrs_ext_t on_off_attr;
	zb_zcl_power_config_attr_t power_attr;
};

static struct zb_device_ctx dev_ctx;

/* Declare attribute list for Basic cluster. */
ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attr_list,
                                     &dev_ctx.basic_attr.zcl_version,
                                     &dev_ctx.basic_attr.app_version,
                                     &dev_ctx.basic_attr.stack_version,
                                     &dev_ctx.basic_attr.hw_version,
                                     dev_ctx.basic_attr.mf_name,
                                     dev_ctx.basic_attr.model_id,
                                     dev_ctx.basic_attr.date_code,
                                     &dev_ctx.basic_attr.power_source,
                                     dev_ctx.basic_attr.location_id,
                                     &dev_ctx.basic_attr.ph_env,
                                     dev_ctx.basic_attr.sw_ver);

/* Declare attribute list for Identify cluster (server). */
ZB_ZCL_DECLARE_IDENTIFY_SERVER_ATTRIB_LIST(
	identify_server_attr_list,
	&dev_ctx.identify_attr.identify_time);

ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST_EXT(on_off_attr_list,
                                      &dev_ctx.on_off_attr.on_off,
                                      &dev_ctx.on_off_attr.global_scene_ctrl,
                                      &dev_ctx.on_off_attr.on_time,
                                      &dev_ctx.on_off_attr.off_wait_time);

ZB_ZCL_DECLARE_POWER_CONFIG_BATTERY_ATTRIB_LIST_EXT(power_config_attr_list,
                                                    &dev_ctx.power_attr.battery_voltage,
                                                    &dev_ctx.power_attr.battery_size,
                                                    &dev_ctx.power_attr.battery_quantity,
                                                    &dev_ctx.power_attr.battery_rated_voltage,
                                                    &dev_ctx.power_attr.battery_alarm_mask,
                                                    &dev_ctx.power_attr.battery_voltage_min_threshold,
                                                    &dev_ctx.power_attr.battery_percentage_remaining,
                                                    &dev_ctx.power_attr.battery_voltage_threshold_1,
                                                    &dev_ctx.power_attr.battery_voltage_threshold_2,
                                                    &dev_ctx.power_attr.battery_voltage_threshold_3,
                                                    &dev_ctx.power_attr.battery_percentage_min_threshold,
                                                    &dev_ctx.power_attr.battery_percentage_threshold_1,
                                                    &dev_ctx.power_attr.battery_percentage_threshold_2,
                                                    &dev_ctx.power_attr.battery_percentage_threshold_3,
                                                    &dev_ctx.power_attr.battery_alarm_state);

/* Declare cluster list for Smart Valve device. */
ZB_DECLARE_SMART_VALVE_CLUSTER_LIST(
	smart_valve_clusters,
	basic_attr_list,
	identify_server_attr_list,
	power_config_attr_list,
	on_off_attr_list);

/* Declare endpoint for Smart Valve device. */
ZB_DECLARE_SMART_VALVE_EP(
	smart_valve_ep,
	SMART_VALVE_ENDPOINT,
	smart_valve_clusters);

/* Declare application's device context (list of registered endpoints)
 * for Smart Valve device.
 */
#ifndef CONFIG_ZIGBEE_FOTA
ZBOSS_DECLARE_DEVICE_CTX_1_EP(smart_valve_ctx, smart_valve_ep);
#else

  #if SMART_VALVE_ENDPOINT == CONFIG_ZIGBEE_FOTA_ENDPOINT
    #error "Smart valve and Zigbee OTA endpoints should be different."
  #endif

extern zb_af_endpoint_desc_t zigbee_fota_client_ep;
ZBOSS_DECLARE_DEVICE_CTX_2_EP(smart_valve_ctx,
			      zigbee_fota_client_ep,
			      smart_valve_ep);
#endif /* CONFIG_ZIGBEE_FOTA */

/* Forward declarations. */
void battery_meassure_and_report(void);
static void valve_open_with_timer(void);
static void valve_close_with_timer(void);
static void battery_report_timer_handler(struct k_work *work);
static void fail_report_timer_handler(struct k_work *work);
static void stop_pairing(void);
static void pairing_advertising_off_timer_handler(struct k_work *work);
static void check_valve_on_off_status_and_set_attr(void);
// timer delays
static K_WORK_DELAYABLE_DEFINE(fail_report_timer_work, fail_report_timer_handler);
static K_WORK_DELAYABLE_DEFINE(pairing_advertising_off_timer_work, pairing_advertising_off_timer_handler);
static K_WORK_DELAYABLE_DEFINE(battery_report_timer_work, battery_report_timer_handler);


#define MOTOR_DELAY_MS                           2000
#define OPEN_CLOSE_COUNTDOWN_MS                  10000
#define FAIL_TIMER_COUNTDOWN_MS                  60000
#define PAIRING_TIMER_COUNTDOWN_MS               60000//30000
#define BATTERY_REPORT_TIMER_MS                  1500000 //TODO 3000000
// timer delays

//------------------------------------------------------------//
static void battery_report_timer_handler(struct k_work *work)
//------------------------------------------------------------//
{   
  ARG_UNUSED(work);
  battery_meassure_and_report();
  reboot_couter++;
  if(reboot_couter >= 12)
  {
    reboot_couter=0;
    NVIC_SystemReset();
  }
  k_work_reschedule(&battery_report_timer_work, K_MSEC(BATTERY_REPORT_TIMER_MS));
}
//------------------------------------------------------------//
static void fail_report_timer_handler(struct k_work *work)
//------------------------------------------------------------//
{   
  ARG_UNUSED(work);
  if(need_to_auto_close_flag && !motor_working) 
  {
    need_to_auto_close_flag = false;
    valve_open_close_handler(false, true);
  }
  else if(!need_to_auto_close_flag)
  {
    motor_working = false;
    motor_sleep();
    check_valve_on_off_status_and_set_attr();
  }
}
//------------------------------------------------------------//
static void stop_pairing(void)
//------------------------------------------------------------//
{
    //bsp_indication_set(BSP_INDICATE_IDLE);
    k_work_cancel_delayable(&pairing_advertising_off_timer_work);
    pairing_in_progress = false;
}
//------------------------------------------------------------//
static void pairing_advertising_off_timer_handler(struct k_work *work)
//------------------------------------------------------------//
{   
  ARG_UNUSED(work);
  if(logic_need_to_be_restored)
  {
    logic_need_to_be_restored = false;
    if(moving_to_close) valve_open_close_handler(false, true);
    else if(moving_to_open) valve_open_close_handler(true, false);
    moving_to_open = false;
    moving_to_close = false;
  }
  else stop_pairing();
}
//-------------------------------------------------------------//
static void app_clusters_attr_init(void)
//-------------------------------------------------------------//
{
	/* Basic cluster attributes data */
    dev_ctx.basic_attr.zcl_version   = ZB_ZCL_VERSION;
    dev_ctx.basic_attr.app_version   = SMART_VALVE_INIT_BASIC_APP_VERSION;
    dev_ctx.basic_attr.stack_version = SMART_VALVE_INIT_BASIC_STACK_VERSION;
    dev_ctx.basic_attr.hw_version    = (uint8_t)NRF_UICR->CUSTOMER[1];

    uint32_t serial = NRF_UICR->CUSTOMER[0];
    zb_char_t  location_id_convert[17];
    sprintf(location_id_convert, "Serial: %X", serial);

    ZB_ZCL_SET_STRING_VAL(dev_ctx.basic_attr.sw_ver,
                          SMART_VALVE_INIT_BASIC_SV_VERSION,
                          ZB_ZCL_STRING_CONST_SIZE(SMART_VALVE_INIT_BASIC_SV_VERSION));


    ZB_ZCL_SET_STRING_VAL(dev_ctx.basic_attr.mf_name,
                          SMART_VALVE_INIT_BASIC_MANUF_NAME,
                          ZB_ZCL_STRING_CONST_SIZE(SMART_VALVE_INIT_BASIC_MANUF_NAME));

    ZB_ZCL_SET_STRING_VAL(dev_ctx.basic_attr.model_id,
                          SMART_VALVE_INIT_BASIC_MODEL_ID,
                          ZB_ZCL_STRING_CONST_SIZE(SMART_VALVE_INIT_BASIC_MODEL_ID));

    ZB_ZCL_SET_STRING_VAL(dev_ctx.basic_attr.date_code,
                          SMART_VALVE_INIT_BASIC_DATE_CODE,
                          ZB_ZCL_STRING_CONST_SIZE(SMART_VALVE_INIT_BASIC_DATE_CODE));
///////////////////
  meassure_battery_percentage();
  if(get_usb_powered_status()) dev_ctx.basic_attr.power_source = ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE;
  else dev_ctx.basic_attr.power_source = ZB_ZCL_BASIC_POWER_SOURCE_BATTERY;
///////////////////
    ZB_ZCL_SET_STRING_VAL(dev_ctx.basic_attr.location_id,
                          location_id_convert,
                          ZB_ZCL_STRING_CONST_SIZE(location_id_convert));


    dev_ctx.basic_attr.ph_env = SMART_VALVE_INIT_BASIC_PH_ENV;

   /* power config cluster attributes data */

    dev_ctx.power_attr.battery_voltage = 0x2A;
    dev_ctx.power_attr.battery_size = ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_AAA;
    dev_ctx.power_attr.battery_quantity = 4;
    dev_ctx.power_attr.battery_rated_voltage = 0x30;
    dev_ctx.power_attr.battery_alarm_mask = 0x30;
    dev_ctx.power_attr.battery_voltage_min_threshold = 0x01;

    dev_ctx.power_attr.battery_percentage_remaining = 0xC6;//0x00 = 0%, 0x64 = 50%, and 0xC8 = 100%
    dev_ctx.power_attr.battery_voltage_threshold_1 = 0x01;
    dev_ctx.power_attr.battery_voltage_threshold_2 = 0x01;
    dev_ctx.power_attr.battery_voltage_threshold_3 = 0x01;
    dev_ctx.power_attr.battery_percentage_min_threshold = 0x01;
    dev_ctx.power_attr.battery_percentage_threshold_1 = 0x01;
    dev_ctx.power_attr.battery_percentage_threshold_2 = 0x01;
    dev_ctx.power_attr.battery_percentage_threshold_3 = 0x01;
    if(dev_ctx.power_attr.battery_alarm_state==0xFFFFFFFF)dev_ctx.power_attr.battery_alarm_state = 0x00;

	/* Identify cluster attributes data. */
	dev_ctx.identify_attr.identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;
}
//-------------------------------------------------------------//
static void identify_cb(zb_bufid_t bufid)
//-------------------------------------------------------------//
{
	zb_ret_t zb_err_code;
  zb_err_code = 0;
	if (bufid) {
		/* Schedule a self-scheduling function that will toggle the LED. */
		//ZB_SCHEDULE_APP_CALLBACK(toggle_identify_led, bufid);
		buzzer_on(500);
	} else {
		/* Cancel the toggling function alarm and turn off LED. */
		//zb_err_code = ZB_SCHEDULE_APP_ALARM_CANCEL(toggle_identify_led, ZB_ALARM_ANY_PARAM);
		//ZVUNUSED(zb_err_code);
		/* Update network status/idenitfication LED. */
		if (ZB_JOINED()) {
			//dk_set_led_on(ZIGBEE_NETWORK_STATE_LED);
		} else {
			//dk_set_led_off(ZIGBEE_NETWORK_STATE_LED);
		}
	}
}
//-------------------------------------------------------------//
static void ota_evt_handler(const struct zigbee_fota_evt *evt)
//-------------------------------------------------------------//
{
	switch (evt->id) {
	case ZIGBEE_FOTA_EVT_PROGRESS:
		//dk_set_led(OTA_ACTIVITY_LED, evt->dl.progress % 2);
		break;

	case ZIGBEE_FOTA_EVT_FINISHED:
		LOG_INF("Reboot application.");
		/* Power on unused sections of RAM to allow MCUboot to use it. */
		if (IS_ENABLED(CONFIG_RAM_POWER_DOWN_LIBRARY)) {
			power_up_unused_ram();
		}

		sys_reboot(SYS_REBOOT_COLD);
		break;

	case ZIGBEE_FOTA_EVT_ERROR:
		LOG_ERR("OTA image transfer failed.");
		break;

	default:
		break;
	}
}
//-------------------------------------------------------------//
static void zcl_device_cb(zb_bufid_t bufid)
//-------------------------------------------------------------//
{
	zb_uint8_t cluster_id;
	zb_uint8_t attr_id;
	zb_zcl_device_callback_param_t  *device_cb_param =
		ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);

  switch (device_cb_param->device_cb_id) 
  {
    	case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
		        cluster_id = device_cb_param->cb_param.set_attr_value_param.cluster_id;
		        attr_id = device_cb_param->cb_param.set_attr_value_param.attr_id;
            if (cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF) 
            {
			        uint8_t value = device_cb_param->cb_param.set_attr_value_param.values.data8;

			        LOG_INF("on/off attribute setting to %hd", value);
			        if (attr_id == ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID) 
              {
				        if(value)
                {
                  valve_open_close_handler(true, false);
                }
                else
                {
                  valve_open_close_handler(false, true);
                }
			        }
		        }  
          break;
      case ZB_ZCL_OTA_UPGRADE_VALUE_CB_ID:
          zigbee_fota_zcl_cb(bufid);
          break;
      default:
			    device_cb_param->status = RET_NOT_IMPLEMENTED;
		      break;
  }
}
//-------------------------------------------------------------//
void zboss_signal_handler(zb_bufid_t bufid)
//-------------------------------------------------------------//
{
	zb_zdo_app_signal_hdr_t *sig_hndler = NULL;
	zb_zdo_app_signal_type_t sig = zb_get_app_signal(bufid, &sig_hndler);
	zb_ret_t status = ZB_GET_APP_SIGNAL_STATUS(bufid);

	/* Update network status LED. */
	//zigbee_led_status_update(bufid, ZIGBEE_NETWORK_STATE_LED);

	if(ZB_JOINED()) led_control(PAIRING_LED_ENUM,false);

#ifdef CONFIG_ZIGBEE_FOTA
	/* Pass signal to the OTA client implementation. */
	zigbee_fota_signal_handler(bufid);
#endif /* CONFIG_ZIGBEE_FOTA */

	switch (sig) {
	case ZB_BDB_SIGNAL_DEVICE_REBOOT:
	/* fall-through */
	case ZB_BDB_SIGNAL_STEERING:
		/* Call default signal handler. */
		ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
		if (status == RET_OK) {
			//TODO add log
		}
		break;
	case ZB_ZDO_SIGNAL_LEAVE:
		/* If device leaves the network, reset bulb short_addr. */
		if (status == RET_OK) {
			//zb_zdo_signal_leave_params_t *leave_params =
				//ZB_ZDO_SIGNAL_GET_PARAMS(sig_hndler, zb_zdo_signal_leave_params_t);

		}
		/* Call default signal handler. */
		ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
		break;
  case ZB_COMMON_SIGNAL_CAN_SLEEP:
		//if(get_usb_powered_status() == false) zb_sleep_now(); //USB powered is not working now
    zb_sleep_now();
		break;

	default:
		/* Call default signal handler. */
		ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
		break;
	}

	if (bufid) {
		zb_buf_free(bufid);
	}
}
//------------------------------------------------------------//
static void valve_open_close_handler(bool open,bool close)
//------------------------------------------------------------//
{// motor open      get_button_open_pressed()   get_button_control1_pressed()
  // motor closed   get_button_close_pressed()  get_button_control2_pressed()
  if(open)//
  {
      if(get_button_close_pressed() && get_button_control2_pressed()) 
      {
        valve_open_with_timer();//
        logic_need_to_be_restored  = false;
      }
      else if(get_button_open_pressed() && get_button_control2_pressed()) 
      {
        valve_close_with_timer();//
        logic_need_to_be_restored  = true;
      }
      else if(!get_button_open_pressed()) 
      {
         valve_open_with_timer();//???
         logic_need_to_be_restored  = false;
      }
  }
  if(close)
  {
      if(get_button_open_pressed() && get_button_control1_pressed()) 
      {
        valve_close_with_timer();//
        logic_need_to_be_restored  = false;
      }
      else if(get_button_close_pressed() && get_button_control1_pressed()) 
      {
        valve_open_with_timer();//
        logic_need_to_be_restored  = true;
      }
      else if(!get_button_close_pressed()) 
      {
        valve_close_with_timer();//??
        logic_need_to_be_restored  = false;
      }
      else if(get_button_close_pressed() && !get_button_control1_pressed() && !get_button_control2_pressed())
      {
        valve_open_with_timer();
        logic_need_to_be_restored  = true;
        //close_from_mid_position_flag = true;
      }
  }
}
//------------------------------------------------------------//
static void check_valve_on_off_status_and_set_attr(void)
//------------------------------------------------------------//
{
  if(get_button_control1_pressed()) 
  {
   if(logic_need_to_be_restored && motor_working) moving_to_open = true, double_click_awaiting = true;
    dev_ctx.on_off_attr.on_off = (zb_bool_t)ZB_ZCL_ON_OFF_IS_ON;//open 
    need_to_auto_close_flag = false;
    k_work_cancel_delayable(&fail_report_timer_work);
  }
  else if(get_button_control2_pressed()) 
  {
    if(logic_need_to_be_restored && motor_working) moving_to_close = true, double_click_awaiting = true;
    //if(close_from_mid_position_flag) close_from_mid_position_flag = false, valve_open_close_handler(false, true);
    dev_ctx.on_off_attr.on_off = (zb_bool_t)ZB_ZCL_ON_OFF_IS_OFF;//close
    need_to_auto_close_flag = false;
    k_work_cancel_delayable(&fail_report_timer_work);
  }
  else
  {
    k_work_reschedule(&fail_report_timer_work, K_MSEC(OPEN_CLOSE_COUNTDOWN_MS));
    need_to_auto_close_flag = true;
  }

  ZB_ZCL_SET_ATTRIBUTE(SMART_VALVE_ENDPOINT, 
                       ZB_ZCL_CLUSTER_ID_ON_OFF,    
                       ZB_ZCL_CLUSTER_SERVER_ROLE,  
                       ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                       (zb_uint8_t *)&dev_ctx.on_off_attr.on_off,                        
                       ZB_FALSE);
}
//---------------------------------------------//
static void forget_network(void)
//---------------------------------------------//
{
    if (!pairing_in_progress)//ZB_JOINED())
    {
        //zb_reset(0);
        zb_bdb_reset_via_local_action(0);//buffer reference (if 0, buffer will be allocated automatically)
        k_work_reschedule(&pairing_advertising_off_timer_work, K_MSEC(PAIRING_TIMER_COUNTDOWN_MS));
        //NRF_LOG_INFO("DEVICE WAS RESETED TO FACTORY DEFAULT");
        //pairing_in_progress = true;
        buzzer_on(1500);
        led_blink(PAIRING_LED_ENUM,10000);
    }
}
//---------------------------------------------//
static void valve_close_with_timer(void)
//---------------------------------------------//
{
  motor_working = true;
  valve_close();
  k_work_cancel_delayable(&fail_report_timer_work);
  k_work_reschedule(&fail_report_timer_work, K_MSEC(FAIL_TIMER_COUNTDOWN_MS));
  //NRF_LOG_INFO("Valve closing");
}
//---------------------------------------------//
static void valve_open_with_timer(void)
//---------------------------------------------//
{
  motor_working = true;
  valve_open();
  k_work_cancel_delayable(&fail_report_timer_work);
  k_work_reschedule(&fail_report_timer_work, K_MSEC(FAIL_TIMER_COUNTDOWN_MS));
  //NRF_LOG_INFO("Valve opening");
}
//-------------------------------------------------------------//
static void button_event_handler(enum button_evt evt)
//-------------------------------------------------------------//
{
	switch (evt) {
	case BUTTON_PAIRING_EVT_PRESSED:
			forget_network();
			break;
	case BUTTON_PAIRING_EVT_RELEASED:

			break;
	case BUTTON_OPEN_EVT_PRESSED:
			if(get_button_control1_pressed() || get_button_control2_pressed())
            {
              motor_sleep();
              motor_working = false;
              k_work_cancel_delayable(&fail_report_timer_work);
              full_press_flag = false;
            }
            else full_press_flag = true;
			break;
	case BUTTON_OPEN_EVT_RELEASED:
			break;
	case BUTTON_CLOSE_EVT_PRESSED:
			if(get_button_control1_pressed() || get_button_control2_pressed())
            {
              motor_sleep();
              motor_working = false;
              k_work_cancel_delayable(&fail_report_timer_work);
              full_press_flag = false;
            }
            else full_press_flag = true;
			break;
	case BUTTON_CLOSE_EVT_RELEASED:
			
			break;
	case BUTTON_CONTROL_1_EVT_PRESSED:
			if(full_press_flag) 
            {
              motor_sleep();
              motor_working = false;
              k_work_cancel_delayable(&fail_report_timer_work);
              full_press_flag = false;
            }
            check_valve_on_off_status_and_set_attr();
			break;
	case BUTTON_CONTROL_1_EVT_RELEASED:
			if(logic_need_to_be_restored && double_click_awaiting) 
            {
              double_click_awaiting = false;
              motor_sleep();  
              k_work_reschedule(&pairing_advertising_off_timer_work, K_MSEC(MOTOR_DELAY_MS));
            }
            else check_valve_on_off_status_and_set_attr();
			break;
	case BUTTON_CONTROL_2_EVT_PRESSED:
			if(full_press_flag) 
            {
              motor_sleep();
              motor_working = false;
              k_work_cancel_delayable(&fail_report_timer_work);
              full_press_flag = false;
            }
            check_valve_on_off_status_and_set_attr();
			break;
	case BUTTON_CONTROL_2_EVT_RELEASED:
			if(logic_need_to_be_restored && double_click_awaiting) 
            {
              double_click_awaiting = false;
              motor_sleep();  
              k_work_reschedule(&pairing_advertising_off_timer_work, K_MSEC(MOTOR_DELAY_MS));
            }
            else check_valve_on_off_status_and_set_attr();
			break;
	default:
			break;
	}
}
//-------------------------------------------------------------//
void battery_meassure_and_report(void)
//-------------------------------------------------------------//
{
  dev_ctx.power_attr.battery_percentage_remaining = (zb_uint8_t)meassure_battery_percentage();
  if(get_usb_powered_status()) 
  {
    dev_ctx.basic_attr.power_source = ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE;
    ZB_ZCL_SET_ATTRIBUTE(SMART_VALVE_ENDPOINT, 
                       ZB_ZCL_CLUSTER_ID_BASIC,    
                       ZB_ZCL_CLUSTER_SERVER_ROLE,  
                       ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID,
                       &dev_ctx.basic_attr.power_source,                        
                       ZB_FALSE);   
                       
    dev_ctx.power_attr.battery_percentage_remaining = 0xC8;
  }
  else
  {
    dev_ctx.basic_attr.power_source = ZB_ZCL_BASIC_POWER_SOURCE_BATTERY;
    ZB_ZCL_SET_ATTRIBUTE(SMART_VALVE_ENDPOINT, 
                       ZB_ZCL_CLUSTER_ID_BASIC,    
                       ZB_ZCL_CLUSTER_SERVER_ROLE,  
                       ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID,
                       &dev_ctx.basic_attr.power_source,                        
                       ZB_FALSE);      
  }
  // TODO next lines are placeholders need to review them later
  // TODO also need to check in the pwm.c the set_motor_pwm()
  set_motor_pwm(((dev_ctx.power_attr.battery_percentage_remaining + 67)*5)+2755);

  ZB_ZCL_SET_ATTRIBUTE(SMART_VALVE_ENDPOINT, 
                       ZB_ZCL_CLUSTER_ID_POWER_CONFIG,    
                       ZB_ZCL_CLUSTER_SERVER_ROLE,  
                       ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID,
                       (zb_uint8_t *)&dev_ctx.power_attr.battery_percentage_remaining,                        
                       ZB_FALSE);   
  //TODO
  // add timer reset
  // add tests
}
//-------------------------------------------------------------//
int main(void)
//-------------------------------------------------------------//
{
	int err = -1;

	// remover because of reboot every 5 hours
  // buzzer_on(1000);

	LOG_INF("Starting ZBOSS");
	
	/* Initialize. */
	err = button_init(button_event_handler);
	if (err) {
		printk("Button Init failed: %d\n", err);
		return 0;
	}

  err = leds_init();

	configure_gpio();
  

  zb_zdo_pim_set_long_poll_interval(ZB_PIM_DEFAULT_LONG_POLL_INTERVAL);

	zigbee_erase_persistent_storage(ERASE_PERSISTENT_CONFIG);
	zb_set_ed_timeout(ED_AGING_TIMEOUT_64MIN);
	zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(7000));//was 3000, set to 7000 so it more than poll interval, related to error in SDK search SDK Connect issue

	zigbee_configure_sleepy_behavior(true);

  zb_bdb_set_legacy_device_support(1);// added for test

	/* Power off unused sections of RAM to lower device power consumption. */
	if (IS_ENABLED(CONFIG_RAM_POWER_DOWN_LIBRARY)) {
		power_down_unused_ram();
	}

#ifdef CONFIG_ZIGBEE_FOTA
	/* Initialize Zigbee FOTA download service. */
	zigbee_fota_init(ota_evt_handler);

	/* Register callback for handling ZCL commands. */
	ZB_ZCL_REGISTER_DEVICE_CB(zcl_device_cb);
#endif /* CONFIG_ZIGBEE_FOTA */

	/* Register smart valve device context (endpoints). */
	ZB_AF_REGISTER_DEVICE_CTX(&smart_valve_ctx);

	app_clusters_attr_init();

	/* Register handlers to identify notifications */
	ZB_AF_SET_IDENTIFY_NOTIFICATION_HANDLER(SMART_VALVE_ENDPOINT, identify_cb);
#ifdef CONFIG_ZIGBEE_FOTA
	ZB_AF_SET_IDENTIFY_NOTIFICATION_HANDLER(CONFIG_ZIGBEE_FOTA_ENDPOINT, identify_cb);
#endif /* CONFIG_ZIGBEE_FOTA */

	/* Start Zigbee default thread. */
	zigbee_enable();
	LOG_INF("ZBOSS started");
  battery_meassure_and_report();
  k_work_reschedule(&battery_report_timer_work, K_MSEC(BATTERY_REPORT_TIMER_MS));
	while (1) {
		k_sleep(K_FOREVER);
	}
} 
