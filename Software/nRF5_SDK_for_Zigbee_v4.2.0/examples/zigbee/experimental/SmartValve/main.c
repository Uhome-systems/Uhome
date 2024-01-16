//----------------------------------------------------------------------------------------------------------//
// developed by Alex Bolshov at KDE
//----------------------------------------------------------------------------------------------------------//
#define debug_log_include //TODO undef for release

#include "main.h"
#include "zboss_api.h"
#include "zb_mem_config_max.h"
#include "zb_error_handler.h"
#include "zigbee_helpers_TWV.h"
#include "app_timer.h"
#include "sv_bsp.h"
#include "boards.h"

#include "zboss_api_addons.h"
#include "zb_zcl_ota_upgrade_addons.h"
#include "zb_error_handler.h"
#include "zigbee_dfu_transport.h"
#include "nrf_dfu_settings.h"
#include "app_scheduler.h"

#include "nrf_802154.h"

//logger
#ifdef debug_log_include
#include "zb_trace.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_rtt.h"
#include "nrf_log_backend_serial.h"
#include "nrf_log_str_formatter.h"
#include "nrf_log_internal.h"
#include <SEGGER_RTT_Conf.h>
#include <SEGGER_RTT.h>
#endif
//logger

#include "nrf_drv_clock.h"
#include "nrf_drv_wdt.h"
#include "nrf_power.h"
#include "smart_valve_inits.h" 

#include "zb_smart_valve.h"

#include "nrf_drv_saadc.h"  

//bool debug;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------//
#define IEEE_CHANNEL_MASK                  0x0210F800U                        /**< Scan only one, predefined channel to find the coordinator. */
#define ERASE_PERSISTENT_CONFIG            ZB_FALSE                             /**< Do not erase NVRAM to save the network parameters after device reboot or power-off. */

#define ZIGBEE_NETWORK_STATE_LED           BSP_BOARD_LED_0                      /**< LED indicating that light switch successfully joind Zigbee network. */

#define OTA_UPGRADE_TEST_MANUFACTURER     123
#define OTA_UPGRADE_TEST_IMAGE_TYPE       321
#define OTA_UPGRADE_TEST_DATA_SIZE        BACKGROUND_DFU_DEFAULT_BLOCK_SIZE

/*! @brief Default Frequency request server about new upgrade file (minutes) */
#define OTA_UPGRADE_QUERY_IMAGE_INTERVAL  (3)

#if (NRF_DFU_HW_VERSION > 0xFFFFUL)
#error Incorrect Hardware Version value in NRF_DFU_HW_VERSION
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool valve_need_to_be_open;
bool reverse_status;
bool motor_working = false;

bool pairing_in_progress = false;

nrf_drv_wdt_channel_id m_channel_id;


bool moving_to_open = false;
bool moving_to_close = false;
bool double_click_awaiting = false;
bool full_press_flag = false;
bool close_from_mid_position_flag = false;
bool logic_need_to_be_restored = false;
bool need_to_auto_close_flag = false;
static void valve_close_with_timer(void);
static void valve_open_with_timer(void);

int reboot_couter=0;

bool buzzer_on_crutch = false;

//TODO
zb_uint8_t current_battery_percent;
zb_uint8_t pervious_battery_percent;
//
nrfx_err_t Battery_ADC_init(void);

//ZB_ZCL_DECLARE_IDENTIFY_CLIENT_ATTRIB_LIST(identify_client_attr_list);

ZB_ZCL_DECLARE_IDENTIFY_SERVER_ATTRIB_LIST(identify_server_attr_list, &m_dev_ctx.identify_attr.identify_time);

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attr_list,
                                     &m_dev_ctx.basic_attr.zcl_version,
                                     &m_dev_ctx.basic_attr.app_version,
                                     &m_dev_ctx.basic_attr.stack_version,
                                     &m_dev_ctx.basic_attr.hw_version,
                                     m_dev_ctx.basic_attr.mf_name,
                                     m_dev_ctx.basic_attr.model_id,
                                     m_dev_ctx.basic_attr.date_code,
                                     &m_dev_ctx.basic_attr.power_source,
                                     m_dev_ctx.basic_attr.location_id,
                                     &m_dev_ctx.basic_attr.ph_env,
                                     m_dev_ctx.basic_attr.sw_ver);

ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST_EXT(on_off_attr_list,
                                      &m_dev_ctx.on_off_attr.on_off,
                                      &m_dev_ctx.on_off_attr.global_scene_ctrl,
                                      &m_dev_ctx.on_off_attr.on_time,
                                      &m_dev_ctx.on_off_attr.off_wait_time);

ZB_ZCL_DECLARE_POWER_CONFIG_BATTERY_ATTRIB_LIST_EXT (power_config_attr_list,
                                                    &m_dev_ctx.power_attr.battery_voltage,
                                                    &m_dev_ctx.power_attr.battery_size,
                                                    &m_dev_ctx.power_attr.battery_quantity,
                                                    &m_dev_ctx.power_attr.battery_rated_voltage,
                                                    &m_dev_ctx.power_attr.battery_alarm_mask,
                                                    &m_dev_ctx.power_attr.battery_voltage_min_threshold,
                                                    &m_dev_ctx.power_attr.battery_percentage_remaining,
                                                    &m_dev_ctx.power_attr.battery_voltage_threshold_1,
                                                    &m_dev_ctx.power_attr.battery_voltage_threshold_2,
                                                    &m_dev_ctx.power_attr.battery_voltage_threshold_3,
                                                    &m_dev_ctx.power_attr.battery_percentage_min_threshold,
                                                    &m_dev_ctx.power_attr.battery_percentage_threshold_1,
                                                    &m_dev_ctx.power_attr.battery_percentage_threshold_2,
                                                    &m_dev_ctx.power_attr.battery_percentage_threshold_3,
                                                    &m_dev_ctx.power_attr.battery_alarm_state);

/*ZB_ZCL_DECLARE_POWER_CONFIG_ATTRIB_LIST(power_config_attr_list,
                                        &m_dev_ctx.power_attr.battery_voltage,
                                        &m_dev_ctx.power_attr.battery_size,
                                        &m_dev_ctx.power_attr.battery_quantity,
                                        &m_dev_ctx.power_attr.battery_rated_voltage,
                                        &m_dev_ctx.power_attr.battery_alarm_mask,
                                        &m_dev_ctx.power_attr.battery_voltage_min_threshold);*/

/* OTA cluster attributes data */
ZB_ZCL_DECLARE_OTA_UPGRADE_ATTRIB_LIST(ota_upgrade_attr_list,
                                       m_dev_ctx.ota_attr.upgrade_server,
                                       &m_dev_ctx.ota_attr.file_offset,
                                       &m_dev_ctx.ota_attr.file_version,
                                       &m_dev_ctx.ota_attr.stack_version,
                                       &m_dev_ctx.ota_attr.downloaded_file_ver,
                                       &m_dev_ctx.ota_attr.downloaded_stack_ver,
                                       &m_dev_ctx.ota_attr.image_status,
                                       &m_dev_ctx.ota_attr.manufacturer,
                                       &m_dev_ctx.ota_attr.image_type,
                                       &m_dev_ctx.ota_attr.min_block_reque,
                                       &m_dev_ctx.ota_attr.image_stamp,
                                       &m_dev_ctx.ota_attr.server_addr,
                                       &m_dev_ctx.ota_attr.server_ep,
                                       (uint16_t)NRF_DFU_HW_VERSION,
                                       OTA_UPGRADE_TEST_DATA_SIZE,
                                       OTA_UPGRADE_QUERY_IMAGE_INTERVAL);

ZB_DECLARE_SMART_VALVE_CLUSTER_LIST(smart_valve_clusters,
                                     basic_attr_list,
                                     on_off_attr_list,
                                     power_config_attr_list,
                                     identify_server_attr_list,
                                     ota_upgrade_attr_list);//,
                                     //identify_client_attr_list);

ZB_ZCL_DECLARE_SMART_VALVE_EP(smart_valve_ep,
                               SMART_VALVE_ENDPOINT,
                               smart_valve_clusters);

ZBOSS_DECLARE_DEVICE_CTX_1_EP(smart_valve_ctx, smart_valve_ep);


/* Create an instance of app_timer for OTA server periodical discovery. */
APP_TIMER_DEF(m_ota_server_discovery_timer);
static zb_zcl_ota_upgrade_client_periodical_discovery_ctx_t m_discovery_ctx;
//---------------------------------------------//
APP_TIMER_DEF(fail_rep_app_timer);
APP_TIMER_DEF(pairing_advertising_off_timer);
APP_TIMER_DEF(battery_report_timer);
//---------------------------------------------//
static uint32_t context;
static uint32_t context2;
static uint32_t context3;
//------------------------------------------------------------//
static void valve_open_close_handler(bool open,bool close)
//------------------------------------------------------------//
{// motor open      BUTTON_OPEN_PRESSED   BUTTON_CONTROL_PRESSED
  // motor closed   BUTTON_CLOSE_PRESSED  BUTTON_CONTROL2_PRESSED
  if(open)//
  {
      if(BUTTON_CLOSE_PRESSED && BUTTON_CONTROL2_PRESSED) 
      {
        valve_open_with_timer();//
        logic_need_to_be_restored  = false;
      }
      else if(BUTTON_OPEN_PRESSED && BUTTON_CONTROL2_PRESSED) 
      {
        valve_close_with_timer();//
        logic_need_to_be_restored  = true;
      }
      else if(BUTTON_OPEN_RELEASED) 
      {
         valve_open_with_timer();//???
         logic_need_to_be_restored  = false;
      }
  }
  if(close)
  {
      if(BUTTON_OPEN_PRESSED && BUTTON_CONTROL_PRESSED) 
      {
        valve_close_with_timer();//
        logic_need_to_be_restored  = false;
      }
      else if(BUTTON_CLOSE_PRESSED && BUTTON_CONTROL_PRESSED) 
      {
        valve_open_with_timer();//
        logic_need_to_be_restored  = true;
      }
      else if(BUTTON_CLOSE_RELEASED) 
      {
        valve_close_with_timer();//??
        logic_need_to_be_restored  = false;
      }
      else if(BUTTON_CLOSE_PRESSED && BUTTON_CONTROL_RELEASED && BUTTON_CONTROL2_RELEASED)
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
  if(BUTTON_CONTROL_PRESSED) 
  {
   if(logic_need_to_be_restored && motor_working) moving_to_open = true, double_click_awaiting = true;
    m_dev_ctx.on_off_attr.on_off = (zb_bool_t)ZB_ZCL_ON_OFF_IS_ON;//open 
    need_to_auto_close_flag = false;
    app_timer_stop(fail_rep_app_timer);
  }
  else if(BUTTON_CONTROL2_PRESSED) 
  {
    if(logic_need_to_be_restored && motor_working) moving_to_close = true, double_click_awaiting = true;
    //if(close_from_mid_position_flag) close_from_mid_position_flag = false, valve_open_close_handler(false, true);
    m_dev_ctx.on_off_attr.on_off = (zb_bool_t)ZB_ZCL_ON_OFF_IS_OFF;//close
    need_to_auto_close_flag = false;
    app_timer_stop(fail_rep_app_timer);
  }
  else
  {
    app_timer_start(fail_rep_app_timer, APP_TIMER_TICKS(OPEN_CLOSE_COUNTDOWN_MS), &context);
    need_to_auto_close_flag = true;
  }

  ZB_ZCL_SET_ATTRIBUTE(SMART_VALVE_ENDPOINT, 
                       ZB_ZCL_CLUSTER_ID_ON_OFF,    
                       ZB_ZCL_CLUSTER_SERVER_ROLE,  
                       ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                       (zb_uint8_t *)&m_dev_ctx.on_off_attr.on_off,                        
                       ZB_FALSE);
}
//------------------------------------------------------------//
static void fail_report_timer_handler(void * context)
//------------------------------------------------------------//
{   
  if(need_to_auto_close_flag && !motor_working) 
  {
    need_to_auto_close_flag = false;
    valve_open_close_handler(false, true);
  }
  else if(!need_to_auto_close_flag)
  {
    motor_working = false;
    MOTOR_SLEEP
    check_valve_on_off_status_and_set_attr();
  }
}
//------------------------------------------------------------//
static void stop_pairing(void)
//------------------------------------------------------------//
{
    bsp_indication_set(BSP_INDICATE_IDLE);
    app_timer_stop(pairing_advertising_off_timer);
    pairing_in_progress = false;
    BUZZER_OFF
}
//------------------------------------------------------------//
static void pairing_advertising_off_timer_handler(void * context)
//------------------------------------------------------------//
{   
  if(logic_need_to_be_restored)
  {
    logic_need_to_be_restored = false;
    if(moving_to_close) valve_open_close_handler(false, true);
    else if(moving_to_open) valve_open_close_handler(true, false);
    moving_to_open = false;
    moving_to_close = false;
  }
  else if(buzzer_on_crutch)
  {
    BUZZER_OFF
    app_timer_start(pairing_advertising_off_timer, APP_TIMER_TICKS(PAIRING_TIMER_COUNTDOWN_MS-BUZZER_PAIRING_TIME_MS), &context2);
    buzzer_on_crutch = false;
  }
  else stop_pairing();
}
//------------------------------------------------------------//
static void measure_battery_and_report(void)
//------------------------------------------------------------//
{
  nrf_saadc_value_t value = 0;
  ret_code_t err_code;

  err_code = nrfx_saadc_sample_convert(0, &value);
  APP_ERROR_CHECK(err_code);
  //NRF_LOG_INFO("Battery measured. Error code: %d", err_code);
  //NRF_LOG_INFO("Battery ADC value: %d", value);
  //----new battery %----
  if(value>3730)value=3730;
  
  if(value<=3730 && value>3320)
  {
    current_battery_percent = (zb_uint8_t)(179 + (value-3320)/21);
  }
  if(value<=3320 && value>2755)
  {
    current_battery_percent = (zb_uint8_t)(67 + (value-2755)/5);
  }
  if(value<=2755 && value>1840)
  {
    current_battery_percent = (zb_uint8_t)(0 + (value-1840)/14);
  }
  if(value<=1840) current_battery_percent = (zb_uint8_t)0;

  if(pervious_battery_percent && current_battery_percent > pervious_battery_percent && (current_battery_percent < (pervious_battery_percent+10))) current_battery_percent = pervious_battery_percent;
  if(motor_working) current_battery_percent = pervious_battery_percent;
  pervious_battery_percent = current_battery_percent;
  //----new battery %----
  NRF_LOG_INFO("ADC value: %d , Battery: %d", value, current_battery_percent/2);

  /*if(value < 2120) value = 2120;

  current_battery_percent = (zb_uint8_t)((value-2120)/8);
  
  if(current_battery_percent>0xC8) current_battery_percent = 0xC8;*/

  m_dev_ctx.power_attr.battery_percentage_remaining = (zb_uint8_t)current_battery_percent;
  ZB_ZCL_SET_ATTRIBUTE(SMART_VALVE_ENDPOINT, 
                       ZB_ZCL_CLUSTER_ID_POWER_CONFIG,    
                       ZB_ZCL_CLUSTER_SERVER_ROLE,  
                       ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID,
                       (zb_uint8_t *)&m_dev_ctx.power_attr.battery_percentage_remaining,                        
                       ZB_FALSE);     


  user_input_indicate();
}
//------------------------------------------------------------//
static void battery_report_timer_handler(void * context)
//------------------------------------------------------------//
{   
  measure_battery_and_report();
  reboot_couter++;
  if(reboot_couter >= 12)
  {
    reboot_couter=0;
    NVIC_SystemReset();
  }
}
//------------------------------------------------------------//
static void smart_valve_clusters_attr_init(void)
//------------------------------------------------------------//
{
    /* Basic cluster attributes data */
    m_dev_ctx.basic_attr.zcl_version   = ZB_ZCL_VERSION;
    m_dev_ctx.basic_attr.app_version   = SMART_VALVE_INIT_BASIC_APP_VERSION;
    m_dev_ctx.basic_attr.stack_version = SMART_VALVE_INIT_BASIC_STACK_VERSION;
    //m_dev_ctx.basic_attr.hw_version    = SMART_VALVE_INIT_BASIC_HW_VERSION;
    ZB_ZCL_SET_STRING_VAL(m_dev_ctx.basic_attr.sw_ver,
                          SMART_VALVE_INIT_BASIC_SV_VERSION,
                          ZB_ZCL_STRING_CONST_SIZE(SMART_VALVE_INIT_BASIC_SV_VERSION));


    ZB_ZCL_SET_STRING_VAL(m_dev_ctx.basic_attr.mf_name,
                          SMART_VALVE_INIT_BASIC_MANUF_NAME,
                          ZB_ZCL_STRING_CONST_SIZE(SMART_VALVE_INIT_BASIC_MANUF_NAME));

    ZB_ZCL_SET_STRING_VAL(m_dev_ctx.basic_attr.model_id,
                          SMART_VALVE_INIT_BASIC_MODEL_ID,
                          ZB_ZCL_STRING_CONST_SIZE(SMART_VALVE_INIT_BASIC_MODEL_ID));

    ZB_ZCL_SET_STRING_VAL(m_dev_ctx.basic_attr.date_code,
                          SMART_VALVE_INIT_BASIC_DATE_CODE,
                          ZB_ZCL_STRING_CONST_SIZE(SMART_VALVE_INIT_BASIC_DATE_CODE));

    m_dev_ctx.basic_attr.power_source = SMART_VALVE_INIT_BASIC_POWER_SOURCE;

    ZB_ZCL_SET_STRING_VAL(m_dev_ctx.basic_attr.location_id,
                          SMART_VALVE_INIT_BASIC_LOCATION_DESC,
                          ZB_ZCL_STRING_CONST_SIZE(SMART_VALVE_INIT_BASIC_LOCATION_DESC));


    m_dev_ctx.basic_attr.ph_env = SMART_VALVE_INIT_BASIC_PH_ENV;

    /* Identify cluster attributes data */
    m_dev_ctx.identify_attr.identify_time        = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

    /* On/Off cluster attributes data */
    check_valve_on_off_status_and_set_attr();

    /* power config cluster attributes data */

    m_dev_ctx.power_attr.battery_voltage = 0x2A;
    m_dev_ctx.power_attr.battery_size = ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_AAA;
    m_dev_ctx.power_attr.battery_quantity = 4;
    m_dev_ctx.power_attr.battery_rated_voltage = 0x30;
    m_dev_ctx.power_attr.battery_alarm_mask = 0x30;
    m_dev_ctx.power_attr.battery_voltage_min_threshold = 0x01;

    m_dev_ctx.power_attr.battery_percentage_remaining = 0xC6;//0x00 = 0%, 0x64 = 50%, and 0xC8 = 100%
    m_dev_ctx.power_attr.battery_voltage_threshold_1 = 0x01;
    m_dev_ctx.power_attr.battery_voltage_threshold_2 = 0x01;
    m_dev_ctx.power_attr.battery_voltage_threshold_3 = 0x01;
    m_dev_ctx.power_attr.battery_percentage_min_threshold = 0x01;
    m_dev_ctx.power_attr.battery_percentage_threshold_1 = 0x01;
    m_dev_ctx.power_attr.battery_percentage_threshold_2 = 0x01;
    m_dev_ctx.power_attr.battery_percentage_threshold_3 = 0x01;
    if(m_dev_ctx.power_attr.battery_alarm_state==0xFFFFFFFF)m_dev_ctx.power_attr.battery_alarm_state = 0x00;

    /* OTA cluster attributes data */
    zb_ieee_addr_t addr = ZB_ZCL_OTA_UPGRADE_SERVER_DEF_VALUE;
    ZB_MEMCPY(m_dev_ctx.ota_attr.upgrade_server, addr, sizeof(zb_ieee_addr_t));
    m_dev_ctx.ota_attr.file_offset = ZB_ZCL_OTA_UPGRADE_FILE_OFFSET_DEF_VALUE;
    m_dev_ctx.ota_attr.file_version = s_dfu_settings.app_version;
    m_dev_ctx.ota_attr.stack_version = ZB_ZCL_OTA_UPGRADE_FILE_HEADER_STACK_PRO;
    m_dev_ctx.ota_attr.downloaded_file_ver  = ZB_ZCL_OTA_UPGRADE_DOWNLOADED_FILE_VERSION_DEF_VALUE;
    m_dev_ctx.ota_attr.downloaded_stack_ver = ZB_ZCL_OTA_UPGRADE_DOWNLOADED_STACK_DEF_VALUE;
    m_dev_ctx.ota_attr.image_status = ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_DEF_VALUE;
    m_dev_ctx.ota_attr.manufacturer = OTA_UPGRADE_TEST_MANUFACTURER;
    m_dev_ctx.ota_attr.image_type = OTA_UPGRADE_TEST_IMAGE_TYPE;
    m_dev_ctx.ota_attr.min_block_reque = 0;
    m_dev_ctx.ota_attr.image_stamp = ZB_ZCL_OTA_UPGRADE_IMAGE_STAMP_MIN_VALUE;
}
//---------------------------------------------//
static void reboot_application(zb_uint8_t param)
//---------------------------------------------//
{
    UNUSED_VARIABLE(param);
    NRF_LOG_FINAL_FLUSH();

    #if NRF_MODULE_ENABLED(NRF_LOG_BACKEND_RTT)
    // To allow the buffer to be flushed by the host.
    nrf_delay_ms(100);
    #endif

    NVIC_SystemReset();
}
//---------------------------------------------//
static void forget_network(void)
//---------------------------------------------//
{
    if (!pairing_in_progress)//ZB_JOINED())
    {
        zb_bdb_reset_via_local_action(0);//buffer reference (if 0, buffer will be allocated automatically)
        bsp_indication_set(BSP_INDICATE_ADVERTISING_DIRECTED);//BSP_INDICATE_SCANNING BSP_INDICATE_ADVERTISING_DIRECTED
        app_timer_start(pairing_advertising_off_timer, APP_TIMER_TICKS(BUZZER_PAIRING_TIME_MS), &context2);
        NRF_LOG_INFO("DEVICE WAS RESETED TO FACTORY DEFAULT");
        pairing_in_progress = true;
        buzzer_on_crutch = true;
        BUZZER_ON
    }
}
//---------------------------------------------//
static void valve_close_with_timer(void)
//---------------------------------------------//
{
  motor_working = true;
  VALVE_CLOSE
  app_timer_stop(fail_rep_app_timer);
  app_timer_start(fail_rep_app_timer, APP_TIMER_TICKS(FAIL_TIMER_COUNTDOWN_MS), &context);
  NRF_LOG_INFO("Valve closing");
}
//---------------------------------------------//
static void valve_open_with_timer(void)
//---------------------------------------------//
{
  motor_working = true;
  VALVE_OPEN
  app_timer_stop(fail_rep_app_timer);
  app_timer_start(fail_rep_app_timer, APP_TIMER_TICKS(FAIL_TIMER_COUNTDOWN_MS), &context);
  NRF_LOG_INFO("Valve opening");
}
//---------------------------------------------//
static void buttons_handler(bsp_event_t evt)
//---------------------------------------------//
{
    switch(evt)
    {
       case BSP_EVENT_RESET:
            forget_network();
            NRF_LOG_INFO("LONG PAIRING button pressed");
            break;
        
       case BSP_EVENT_KEY_5: //BUTTON_1 button pairing
            BUZZER_OFF
            break;

        case BSP_EVENT_KEY_0: //BUTTON_1 button pairing
            user_input_indicate();  // Inform default signal handler about user input at the device.
            MOTOR_SLEEP// TODO
            NRF_LOG_INFO("PAIRING button pressed");
            break;

        case BSP_EVENT_KEY_1: //BUTTON_2 open  ////////////////////////////////////////////////////////////////////////////////////   
            if(BUTTON_CONTROL_PRESSED || BUTTON_CONTROL2_PRESSED)
            {
              MOTOR_SLEEP
              motor_working = false;
              app_timer_stop(fail_rep_app_timer);
              full_press_flag = false;
            }
            else full_press_flag = true;
            NRF_LOG_INFO("OPEN button pressed");
            break;

        case BSP_EVENT_KEY_2://BUTTON_3 close 
            if(BUTTON_CONTROL_PRESSED || BUTTON_CONTROL2_PRESSED)
            {
              MOTOR_SLEEP
              motor_working = false;
              app_timer_stop(fail_rep_app_timer);
              full_press_flag = false;
            }
            else full_press_flag = true;
            NRF_LOG_INFO("CLOSE button pressed");
            break;

        case BSP_EVENT_KEY_6: // was BSP_EVENT_KEY_3://BUTTON_4 control
            if(full_press_flag) 
            {
              MOTOR_SLEEP
              motor_working = false;
              app_timer_stop(fail_rep_app_timer);
              full_press_flag = false;
            }
            check_valve_on_off_status_and_set_attr();
            NRF_LOG_INFO("CONTROL button pressed");
            break;

        case BSP_EVENT_KEY_7: //was BSP_EVENT_KEY_4://BUTTON_5 control2
            if(full_press_flag) 
            {
              MOTOR_SLEEP
              motor_working = false;
              app_timer_stop(fail_rep_app_timer);
              full_press_flag = false;
            }
            check_valve_on_off_status_and_set_attr();
            NRF_LOG_INFO("CONTROL2 button pressed");
            break;

        case BSP_EVENT_KEY_3: // was BSP_EVENT_KEY_6: //BUTTON_4 control1 release
            if(logic_need_to_be_restored && double_click_awaiting) 
            {
              double_click_awaiting = false;
              MOTOR_SLEEP  
              app_timer_start(pairing_advertising_off_timer, APP_TIMER_TICKS(MOTOR_DELAY_MS), &context2);  
            }
            else check_valve_on_off_status_and_set_attr();
            NRF_LOG_INFO("CONTROL1 button released");
            break;

        case BSP_EVENT_KEY_4: //was BSP_EVENT_KEY_7: //BUTTON_5 control2 release
            if(logic_need_to_be_restored && double_click_awaiting) 
            {
              double_click_awaiting = false;
              MOTOR_SLEEP  
              app_timer_start(pairing_advertising_off_timer, APP_TIMER_TICKS(MOTOR_DELAY_MS), &context2);  
            }
            else check_valve_on_off_status_and_set_attr();
            NRF_LOG_INFO("CONTROL2 button released");
            break;

        default:
            NRF_LOG_INFO("Unhandled BSP Event received: %d", evt);
            return;
    }

}
//---------------------------------------------//
static void leds_buttons_init(void)
//---------------------------------------------//
{
    ret_code_t error_code;

    /* Initialize LEDs and buttons - use BSP to control them. */
    error_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, buttons_handler);

    APP_ERROR_CHECK(error_code);
    NRF_LOG_INFO("Buttons and LEDs init. Error code: %d", error_code);
    /* By default the bsp_init attaches BSP_KEY_EVENTS_{0-4} to the PUSH events of the corresponding buttons. */
    
    ww_driver_pins_init();
    NRF_LOG_INFO("Buttons and LEDs was initialised");
}
//------------------------------------------------------------//
static void identify_handler(zb_uint8_t param)
//------------------------------------------------------------//
{
    if (param)
    {
        bsp_indication_set(BSP_INDICATE_ADVERTISING_DIRECTED);
        //bsp_board_leds_on();
        NRF_LOG_INFO("Start identifying.");
    }
    else
    {
        bsp_indication_set(BSP_INDICATE_IDLE);
        //bsp_board_leds_off();
        NRF_LOG_INFO("Stop identifying.");
    }
}
//------------------------------------------------------------//
void zboss_signal_handler(zb_bufid_t bufid)
//------------------------------------------------------------//
{
    zb_zdo_app_signal_hdr_t  * p_sg_p      = NULL;
    zb_zdo_app_signal_type_t   sig         = zb_get_app_signal(bufid, &p_sg_p);
    zb_ret_t                   status      = ZB_GET_APP_SIGNAL_STATUS(bufid);
    
    
    if(ZB_JOINED() && pairing_in_progress)
    {
      stop_pairing();
    }

    nrf_drv_wdt_channel_feed(m_channel_id);
    //NRF_LOG_INFO("WDT feed");

    switch (sig)
    {
        case ZB_BDB_SIGNAL_DEVICE_REBOOT:
            NRF_LOG_INFO("ZBOSS recived BDB signal DEVICE REBOOT");
            /* fall-through */
        case ZB_BDB_SIGNAL_STEERING:
            /* Call default signal handler. */
            ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
            NRF_LOG_INFO("ZBOSS recived BDB signal STEERING");
            if (status == RET_OK)
            {
            //TODO add extended pan ID 00000000000000000000000000 resolve
                zb_ext_pan_id_t extended_pan_id;
                zb_get_extended_pan_id(extended_pan_id);
                if (extended_pan_id[0] == 0
                 && extended_pan_id[1] == 0
                 && extended_pan_id[2] == 0
                 && extended_pan_id[3] == 0
                 && extended_pan_id[4] == 0
                 && extended_pan_id[5] == 0
                 && extended_pan_id[6] == 0
                 && extended_pan_id[7] == 0)
                {
                    zb_reset(0);//leave_local_req();
                    NRF_LOG_INFO("ZBOSS Joined empty network, rebooting ZBOSS stack ----- ------");
                }

                ret_code_t err_code = zb_zcl_ota_upgrade_client_with_periodical_discovery_start(&m_discovery_ctx);
                APP_ERROR_CHECK(err_code);
                NRF_LOG_INFO("OTA upgrade client started. Error code: %d", err_code);
            }
            break;
        case ZB_NLME_STATUS_INDICATION:
            NRF_LOG_INFO("ZBOSS recived NLME signal");
            /* fall-through */

        default:
            /* Call default signal handler. */
            ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
            break;
    }

    if (bufid)
    {
        zb_buf_free(bufid);
    }
}
//---------------------------------------------//
static void zcl_device_cb(zb_bufid_t bufid)
//---------------------------------------------//
{
    zb_uint8_t                       cluster_id;
    //zb_uint8_t                       attr_id;
    zb_zcl_device_callback_param_t * p_device_cb_param = ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);

    NRF_LOG_INFO("zcl_device_cb id %hd", p_device_cb_param->device_cb_id);
    
    /* Set default response value. */
    p_device_cb_param->status = RET_OK;

    switch (p_device_cb_param->device_cb_id)
    {
        case ZB_ZCL_OTA_UPGRADE_VALUE_CB_ID:
        {
            zb_zcl_ota_upgrade_value_param_t * p_ota_upgrade_value = &(p_device_cb_param->cb_param.ota_value_param);

            switch (p_ota_upgrade_value->upgrade_status)
            {
                case ZB_ZCL_OTA_UPGRADE_STATUS_START:
                    /* Check if OTA client is in the middle of image download.
                        If so, silently ignore the second QueryNextImageResponse packet from OTA server. */
                    if (zb_zcl_ota_upgrade_get_ota_status(p_device_cb_param->endpoint) != ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_NORMAL)
                    {
                        p_ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_BUSY;
                    }

                    /* Check if we're not downgrading.
                       If we do, let's politely say no since we do not support that. */
                    else if (p_ota_upgrade_value->upgrade.start.file_version > m_dev_ctx.ota_attr.file_version)
                    {
                        p_ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_OK;
                    }
                    else
                    {
                        p_ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_ABORT;
                    }
                    break;

                case ZB_ZCL_OTA_UPGRADE_STATUS_RECEIVE:
                    /* Process image block. */
                    p_ota_upgrade_value->upgrade_status = zb_process_chunk(p_ota_upgrade_value, bufid);
                    //bsp_board_led_invert(OTA_ACTIVITY);
                    break;

                case ZB_ZCL_OTA_UPGRADE_STATUS_CHECK:
                    p_ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_OK;
                    break;

                case ZB_ZCL_OTA_UPGRADE_STATUS_APPLY:
                    //bsp_board_led_on(OTA_ACTIVITY);
                    p_ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_OK;
                    break;

                case ZB_ZCL_OTA_UPGRADE_STATUS_FINISH:
                    /* It is time to upgrade FW. */
                    /* We use callback so the stack can have time to i.e. send response etc */
                    UNUSED_RETURN_VALUE(ZB_SCHEDULE_APP_CALLBACK(reboot_application, 0));
                    break;

                case ZB_ZCL_OTA_UPGRADE_STATUS_ABORT:
                    NRF_LOG_INFO("Zigbee DFU Aborted");
                    p_ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_ABORT;
                    //bsp_board_led_off(OTA_ACTIVITY);
                    zb_abort_dfu();
                    break;

                default:
                    p_device_cb_param->status = RET_NOT_IMPLEMENTED;
                    break;
            }
            /* No need to free the buffer - stack handles that if needed */
        }
            break;

        case ZB_ZCL_IDENTIFY_EFFECT_CB_ID:
            
            break;

        case ZB_ZCL_LEVEL_CONTROL_SET_VALUE_CB_ID:

            break;

        case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
            cluster_id = p_device_cb_param->cb_param.set_attr_value_param.cluster_id;
            //attr_id    = p_device_cb_param->cb_param.set_attr_value_param.attr_id;

            if (cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF)
            {
                uint8_t value = p_device_cb_param->cb_param.set_attr_value_param.values.data8;

                if(COMMAND_TO_OPEN_VALVE)
                {
                  valve_open_close_handler(true, false);
                  NRF_LOG_INFO("recived command to open valve");
                }
                else if(COMMAND_TO_CLOSE_VALVE)
                {
                  valve_open_close_handler(false, true);
                  NRF_LOG_INFO("recived command to close valve");
                }
            }
            else
            {
                NRF_LOG_INFO("ZCL handler goes to other cluster section");
                /* Other clusters can be processed here */
            }
            break;

        default:
            p_device_cb_param->status = RET_ERROR;
            break;
    }
}

//------------------------------------------------------------//
void watchdog_handler(void)
//------------------------------------------------------------//
{
    bsp_board_leds_off();
    //NOTE: The max amount of time we can spend in WDT interrupt is two cycles of 32768[Hz] clock - after that, reset occurs
}
//------------------------------------------------------------//
void watchdog_init(void)
//------------------------------------------------------------//
{
    nrf_drv_clock_init();
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
    uint32_t err_code = nrf_drv_wdt_init(&config, watchdog_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
    nrf_drv_wdt_enable();
}
//------------------------------------------------------------//
static zb_uint8_t valve_ep_handler(zb_bufid_t bufid)
//------------------------------------------------------------//
{
	return ZB_FALSE;
}
//------------------------------------------------------------//
static void log_init(void)
//------------------------------------------------------------//
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}
//------------------------------------------------------------//
void zigbee_init()
//------------------------------------------------------------//
{
    zb_ret_t       zb_err_code;
    zb_ret_t       err_code;
    zb_ieee_addr_t ieee_addr;
    
    /* Set Zigbee stack logging level and traffic dump subsystem. */
    #ifdef debug_log_include
    ZB_SET_TRACE_LEVEL(ZIGBEE_TRACE_LEVEL);
    ZB_SET_TRACE_MASK(ZIGBEE_TRACE_MASK); //TODO test other values, 0 is default
    ZB_SET_TRAF_DUMP_OFF();
    #else
    ZB_TRACE_LEVEL(0);//TODO test
    #endif

    #ifdef debug_log_include
    log_init();
    #endif
    // enabling DC/DC on power input
    nrf_power_dcdcen_set(true);
    
    watchdog_init();

    /* Initialize loging system and GPIOs. */
    leds_buttons_init();
    timers_init();

    // create timer for fail report
    app_timer_create(&fail_rep_app_timer, APP_TIMER_MODE_SINGLE_SHOT, fail_report_timer_handler);  
    app_timer_create(&pairing_advertising_off_timer, APP_TIMER_MODE_SINGLE_SHOT, pairing_advertising_off_timer_handler);  
    app_timer_create(&battery_report_timer, APP_TIMER_MODE_REPEATED, battery_report_timer_handler);  

    /* Initialize the Zigbee DFU Transport */
    zb_dfu_init(SMART_VALVE_ENDPOINT);

    /* Initialize Zigbee stack. */
    ZB_INIT("smart valve");

    /* Set device address to the value read from FICR registers. */
    zb_osif_get_ieee_eui64(ieee_addr);
    zb_set_long_address(ieee_addr);

    /* Set static long IEEE address. */
    zb_set_network_ed_role(IEEE_CHANNEL_MASK);
    zigbee_erase_persistent_storage(ERASE_PERSISTENT_CONFIG);

    //zb_set_ed_timeout(ED_AGING_TIMEOUT_64MIN);
    zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(60000));// def - 3000 //MAC data polling - max 7500
    zb_set_rx_on_when_idle(ZB_FALSE);//ZB_FALSE//ZB_TRUE
    //zb_sleep_set_threshold(100);

    // HA has ord Zigbee stack revision that does not support Trust Center Link Key exchange(need to search more info)
    zb_bdb_set_legacy_device_support(1);

    /* Initialize application context structure. */
    UNUSED_RETURN_VALUE(ZB_MEMSET(&m_dev_ctx, 0, sizeof(m_dev_ctx)));

    /* Register temperature sensor device context (endpoints). */
    ZB_AF_REGISTER_DEVICE_CTX(&smart_valve_ctx);

    /* Register handlers to identify notifications */
    ZB_AF_SET_IDENTIFY_NOTIFICATION_HANDLER(SMART_VALVE_ENDPOINT, identify_handler);
    
    // callback for ZCL commands
    ZB_ZCL_REGISTER_DEVICE_CB(zcl_device_cb);

    ZB_AF_SET_ENDPOINT_HANDLER(SMART_VALVE_ENDPOINT,valve_ep_handler);
    /* Initialize sensor device attibutes */
    smart_valve_clusters_attr_init();
    
    /* Initialize Periodic OTA server discovery */
    err_code = zb_zcl_ota_upgrade_client_with_periodical_discovery_init(&m_discovery_ctx, &m_ota_server_discovery_timer, SMART_VALVE_ENDPOINT);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("OTA upgrade init. Error code: %d", err_code);
    /** Start Zigbee Stack. */
    zb_err_code = zboss_start_no_autostart();
    ZB_ERROR_CHECK(zb_err_code);
    NRF_LOG_INFO("ZBOSS stack init. Error code: %d", zb_err_code);

    //zigbee_power_down_unused_ram();
    //nrf_802154_tx_power_set(0);
    //nrf_drv_clock_lfclk_request(NULL);
    //nrfx_clock_hfclk_stop();
}
//------------------------------------------------------------//
void other_inits(void)
//------------------------------------------------------------//
{
    zb_ret_t       err_code;

    err_code = Battery_ADC_init();
    NRF_LOG_INFO("SAADC init. Error code: %d", err_code);

    measure_battery_and_report();
    app_timer_start(battery_report_timer, APP_TIMER_TICKS(BATTERY_REPORT_TIMER_MS), &context3);
}    
//------------------------------------------------------------//
int main(void)
//------------------------------------------------------------//
{
    zigbee_init();
    other_inits();
    nrf_802154_tx_power_set(8);
    while(1)
    {
    //measure_battery_and_report();
        nrf_drv_wdt_channel_feed(m_channel_id);
        zboss_main_loop_iteration();
        app_sched_execute();
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
    }
}
