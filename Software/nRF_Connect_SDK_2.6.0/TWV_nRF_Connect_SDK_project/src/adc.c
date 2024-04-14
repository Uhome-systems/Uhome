#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>

#include "adc.h"

bool usb_powered = false;

////----------ADC-----------------------------------------------////
#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),
/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

//-------------------------------------------------------------//
bool get_usb_powered_status()
//-------------------------------------------------------------//
{
	return usb_powered;
}

//-------------------------------------------------------------//
uint8_t meassure_battery_percentage(void)
//-------------------------------------------------------------//
{
	uint16_t buf;
    int32_t val_mv;
	int error = 0;

	struct adc_sequence sequence = {
		.buffer = &buf,

		.buffer_size = sizeof(buf),
	};
	adc_channel_setup_dt(&adc_channels[0]);
	(void)adc_sequence_init_dt(&adc_channels[0], &sequence);
	adc_read(adc_channels[0].dev, &sequence);

  if (adc_channels[0].channel_cfg.differential) 
    {
			val_mv = (int32_t)((int16_t)buf);
		} 
    else 
    {
			val_mv = (int32_t)buf;
		}
	
	error = adc_raw_to_millivolts_dt(&adc_channels[0], &val_mv);
	val_mv = val_mv*2;


	if (buf < 500 || buf > 65000) usb_powered = true;
	else usb_powered = false;

	if (val_mv > 6500) val_mv = 6500;
  
  	if (val_mv > 5800) 		return (uint8_t)(179 + (val_mv-5800)/33);
  	else if	(val_mv > 4800) return (uint8_t)(67 + (val_mv-4800)/9);

  	else if	(val_mv > 4000) return (uint8_t)(0 + (val_mv-4000)/12);
  	else 					return (uint8_t)0;
    
}