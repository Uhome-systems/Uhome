#include "nrf_drv_saadc.h"  
#include "boards.h"
//#include "zb_smart_valve.h"



//---------------------------------------------//
void saadc_callback_handler(nrf_drv_saadc_evt_t const * p_event)
//---------------------------------------------//
{
  /*nrf_saadc_value_t value = 0;

  if(p_event -> type == NRFX_SAADC_EVT_DONE)
  {
    ret_code_t err_code;

    err_code = nrfx_saadc_sample_convert(0, &value);
    APP_ERROR_CHECK(err_code);

    current_battery_percent = (value-2120)/8;

    ZB_ZCL_SET_ATTRIBUTE(SMART_VALVE_ENDPOINT, 
                       ZB_ZCL_CLUSTER_ID_POWER_CONFIG,    
                       ZB_ZCL_CLUSTER_SERVER_ROLE,  
                       ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID,
                       (zb_uint8_t *)&m_dev_ctx.power_attr.battery_percentage_remaining,                        
                       ZB_FALSE);
  }
*/
}
//---------------------------------------------//
nrfx_err_t Battery_ADC_init(void)
//---------------------------------------------//
{
  ret_code_t error_code;

  nrf_saadc_channel_config_t channel_config;// = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(BATTERY_VOLTAGE_PIN);
  
  channel_config.resistor_p = NRF_SAADC_RESISTOR_DISABLED;
  channel_config.resistor_n = NRF_SAADC_RESISTOR_DISABLED;
  channel_config.gain       = NRF_SAADC_GAIN1_6;
  channel_config.reference  = NRF_SAADC_REFERENCE_INTERNAL;
  channel_config.acq_time   = NRF_SAADC_ACQTIME_40US;
  channel_config.mode       = NRF_SAADC_MODE_SINGLE_ENDED;
  channel_config.burst      = NRF_SAADC_BURST_DISABLED;
  channel_config.pin_p      = (nrf_saadc_input_t)(NRF_SAADC_INPUT_AIN0);
  channel_config.pin_n      = NRF_SAADC_INPUT_DISABLED;

  //Init the SAADC driver
  error_code = nrf_drv_saadc_init(NULL, saadc_callback_handler);
  APP_ERROR_CHECK(error_code);

  //Init SAADC channel
  error_code = nrfx_saadc_channel_init(0, &channel_config);
  APP_ERROR_CHECK(error_code);
  //Buffer init
  //error_code = nrfx_saadc_buffer_convert(my_buffer_pool,SAMPLE_BUFFER_LENGTH);
  //APP_ERROR_CHECK(error_code);

  return error_code;
}