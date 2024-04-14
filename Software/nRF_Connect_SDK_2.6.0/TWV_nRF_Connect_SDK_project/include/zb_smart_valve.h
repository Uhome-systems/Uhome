/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef ZB_SMART_VALVE_H
#define ZB_SMART_VALVE_H 1

/* Basic cluster attributes initial values. */
#define SMART_VALVE_INIT_BASIC_APP_VERSION       01                                    /**< Version of the application software (1 byte). */
#define SMART_VALVE_INIT_BASIC_STACK_VERSION     10                                    /**< Version of the implementation of the Zigbee stack (1 byte). */
#define SMART_VALVE_INIT_BASIC_SV_VERSION        "V2.24" 
#define SMART_VALVE_INIT_BASIC_HW_VERSION        1                                    /**< Version of the hardware of the device (1 byte). */
#define SMART_VALVE_INIT_BASIC_MANUF_NAME        "UHome"                                 /**< Manufacturer name (32 bytes). */
#define SMART_VALVE_INIT_BASIC_MODEL_ID          "TWV"                    /**< Model number assigned by manufacturer (32-bytes long string). */
#define SMART_VALVE_INIT_BASIC_DATE_CODE         "20241404"                            /**< First 8 bytes specify the date of manufacturer of the device in ISO 8601 format (YYYYMMDD). The rest (8 bytes) are manufacturer specific. */
#define SMART_VALVE_INIT_BASIC_POWER_SOURCE      ZB_ZCL_BASIC_POWER_SOURCE_BATTERY      /**< Type of power sources available for the device. For possible values see section 3.2.2.2.8 of ZCL specification. */
//#define SMART_VALVE_INIT_BASIC_LOCATION_DESC     "Office desk"                         /**< Describes the physical location of the device (16 bytes). May be modified during commisioning process. */
#define SMART_VALVE_INIT_BASIC_PH_ENV            ZB_ZCL_BASIC_ENV_UNSPECIFIED          /**< Describes the type of physical environment. For possible values see section 3.2.2.2.10 of ZCL specification. */

#define SMART_VALVE_ENDPOINT                     1
/** Dimmer Switch Device ID */
#define ZB_SMART_VALVE_DEVICE_ID 0x0104

/** Dimmer Switch device version */
#define ZB_DEVICE_VER_SMART_VALVE 0

/** @cond internals_doc */

/** Dimmer Switch IN (server) clusters number */
#define ZB_SMART_VALVE_IN_CLUSTER_NUM 4

/** Dimmer Switch OUT (client) clusters number */
#define ZB_SMART_VALVE_OUT_CLUSTER_NUM 0

/** Dimmer switch total (IN+OUT) cluster number */
#define ZB_SMART_VALVE_CLUSTER_NUM \
	(ZB_SMART_VALVE_IN_CLUSTER_NUM + ZB_SMART_VALVE_OUT_CLUSTER_NUM)

/** Number of attribute for reporting on Dimmer Switch device */
#define ZB_SMART_VALVE_REPORT_ATTR_COUNT 3
//----------------------------------------------------------------------------------------------------------//
#define bat_num //needed for power config cluster to work
typedef  struct zb_zcl_power_config_attr_s
{
    zb_uint8_t battery_voltage; // Atribute 3.3.2.2.3.1
    zb_uint8_t battery_size; // Atribute 3.3.2.2.4.2
    zb_uint8_t battery_quantity; // Atribute 3.3.2.2.4.4
    zb_uint8_t battery_rated_voltage; // Atribute 3.3.2.2.4.5
    zb_uint8_t battery_alarm_mask; // Atribute 3.3.2.2.4.6
    zb_uint8_t battery_voltage_min_threshold; // Atribute 3.3.2.2.4.7
    zb_uint8_t battery_percentage_remaining; // Atribute 3.3.2.2.3.1
    zb_uint8_t battery_voltage_threshold_1; // Atribute 3.3.2.2.4.8
    zb_uint8_t battery_voltage_threshold_2; // Atribute 3.3.2.2.4.8
    zb_uint8_t battery_voltage_threshold_3; // Atribute 3.3.2.2.4.8
    zb_uint8_t battery_percentage_min_threshold; // Atribute 3.3.2.2.4.9
    zb_uint8_t battery_percentage_threshold_1; // Atribute 3.3.2.2.4.10
    zb_uint8_t battery_percentage_threshold_2; // Atribute 3.3.2.2.4.10
    zb_uint8_t battery_percentage_threshold_3; // Atribute 3.3.2.2.4.10
    zb_uint32_t battery_alarm_state; // Atribute 3.3.2.2.4.11
} zb_zcl_power_config_attr_t;
/*typedef  struct zb_zcl_power_config_attr_s



/**
 * @brief Declare cluster list for Dimmer Switch device
 * @param cluster_list_name - cluster list variable name
 * @param basic_server_attr_list - attribute list for Basic cluster (server role)
 * @param identify_server_attr_list - attribute list for Identify cluster (server role)
 * @param on_off_attr_list - attribute list for On/Off cluster (client role)
 */
#define ZB_DECLARE_SMART_VALVE_CLUSTER_LIST(					  \
		cluster_list_name,						  \
		basic_server_attr_list,						  \
		identify_server_attr_list,					  \
		power_config_attr_list,                      \
		on_off_attr_list)					  \
zb_zcl_cluster_desc_t cluster_list_name[] =					  \
{										  \
	ZB_ZCL_CLUSTER_DESC(							  \
		ZB_ZCL_CLUSTER_ID_BASIC,					  \
		ZB_ZCL_ARRAY_SIZE(basic_server_attr_list, zb_zcl_attr_t),	  \
		(basic_server_attr_list),					  \
		ZB_ZCL_CLUSTER_SERVER_ROLE,					  \
		ZB_ZCL_MANUF_CODE_INVALID					  \
	),									  \
	ZB_ZCL_CLUSTER_DESC(							  \
		ZB_ZCL_CLUSTER_ID_IDENTIFY,					  \
		ZB_ZCL_ARRAY_SIZE(identify_server_attr_list, zb_zcl_attr_t),	  \
		(identify_server_attr_list),					  \
		ZB_ZCL_CLUSTER_SERVER_ROLE,					  \
		ZB_ZCL_MANUF_CODE_INVALID					  \
	),									  \
	ZB_ZCL_CLUSTER_DESC(                                           \
          ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                              \
          ZB_ZCL_ARRAY_SIZE(power_config_attr_list, zb_zcl_attr_t),    \
          (power_config_attr_list),                                    \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                  \
          ZB_ZCL_MANUF_CODE_INVALID                                    \
    ),                                                             \
	ZB_ZCL_CLUSTER_DESC(							  \
		ZB_ZCL_CLUSTER_ID_ON_OFF,					  \
		ZB_ZCL_ARRAY_SIZE(on_off_attr_list, zb_zcl_attr_t),	  \
		(on_off_attr_list),					  \
		ZB_ZCL_CLUSTER_SERVER_ROLE,					  \
		ZB_ZCL_MANUF_CODE_INVALID					  \
	)									  \
}


/** @cond internals_doc */
/**
 * @brief Declare simple descriptor for Dimmer switch device
 * @param ep_name - endpoint variable name
 * @param ep_id - endpoint ID
 * @param in_clust_num - number of supported input clusters
 * @param out_clust_num - number of supported output clusters
 */
#define ZB_ZCL_DECLARE_SMART_VALVE_SIMPLE_DESC(				    \
	ep_name, ep_id, in_clust_num, out_clust_num)				    \
	ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);			    \
	ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name = \
	{									    \
		ep_id,								    \
		ZB_AF_HA_PROFILE_ID,						    \
		ZB_HA_REMOTE_CONTROL_DEVICE_ID,					    \
		ZB_DEVICE_VER_SMART_VALVE,					    \
		0,								    \
		in_clust_num,							    \
		out_clust_num,							    \
		{								    \
			ZB_ZCL_CLUSTER_ID_BASIC,				    \
			ZB_ZCL_CLUSTER_ID_IDENTIFY,				    \
			ZB_ZCL_CLUSTER_ID_POWER_CONFIG,             \
			ZB_ZCL_CLUSTER_ID_ON_OFF,				    \
		}								    \
	}

/** @endcond */ /* internals_doc */

/**
 * @brief Declare endpoint for Dimmer Switch device
 * @param ep_name - endpoint variable name
 * @param ep_id - endpoint ID
 * @param cluster_list - endpoint cluster list
 */
#define ZB_DECLARE_SMART_VALVE_EP(ep_name, ep_id, cluster_list)		      \
	ZB_ZCL_DECLARE_SMART_VALVE_SIMPLE_DESC(ep_name, ep_id,		      \
		  ZB_SMART_VALVE_IN_CLUSTER_NUM, ZB_SMART_VALVE_OUT_CLUSTER_NUM); \
	  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,            \
                                     ZB_SMART_VALVE_REPORT_ATTR_COUNT);           \
	ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID, 0, NULL,     \
		ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list, \
		(zb_af_simple_desc_1_1_t *)&simple_desc_##ep_name,		      \
		ZB_SMART_VALVE_REPORT_ATTR_COUNT, reporting_info## device_ctx_name, /* No reporting ctx */					      \
		0, NULL) /* No CVC ctx */

/** @} */

#endif /* ZB_SMART_VALVE_H */
