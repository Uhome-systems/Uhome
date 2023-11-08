#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_zcl_power_config.h"
//----------------------------------------------------------------------------------------------------------//
#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------------------------------------------------------------//
/* Basic cluster attributes initial values. */
#define SMART_VALVE_INIT_BASIC_APP_VERSION       01                                    /**< Version of the application software (1 byte). */
#define SMART_VALVE_INIT_BASIC_STACK_VERSION     10                                    /**< Version of the implementation of the Zigbee stack (1 byte). */
#define SMART_VALVE_INIT_BASIC_SV_VERSION        "V1.1" 
#define SMART_VALVE_INIT_BASIC_HW_VERSION        1                                    /**< Version of the hardware of the device (1 byte). */
#define SMART_VALVE_INIT_BASIC_MANUF_NAME        "UHome"                                 /**< Manufacturer name (32 bytes). */
#define SMART_VALVE_INIT_BASIC_MODEL_ID          "TWV"                    /**< Model number assigned by manufacturer (32-bytes long string). */
#define SMART_VALVE_INIT_BASIC_DATE_CODE         "20230816"                            /**< First 8 bytes specify the date of manufacturer of the device in ISO 8601 format (YYYYMMDD). The rest (8 bytes) are manufacturer specific. */
#define SMART_VALVE_INIT_BASIC_POWER_SOURCE      ZB_ZCL_BASIC_POWER_SOURCE_BATTERY      /**< Type of power sources available for the device. For possible values see section 3.2.2.2.8 of ZCL specification. */
#define SMART_VALVE_INIT_BASIC_LOCATION_DESC     "Office desk"                         /**< Describes the physical location of the device (16 bytes). May be modified during commisioning process. */
#define SMART_VALVE_INIT_BASIC_PH_ENV            ZB_ZCL_BASIC_ENV_UNSPECIFIED          /**< Describes the type of physical environment. For possible values see section 3.2.2.2.10 of ZCL specification. */

#define SMART_VALVE_ENDPOINT                     1
#define SMART_VALVE_LED                          LED_2

#define MOTOR_DELAY_MS                           2000
#define OPEN_CLOSE_COUNTDOWN_MS                  10000
#define FAIL_TIMER_COUNTDOWN_MS                  60000
#define PAIRING_TIMER_COUNTDOWN_MS               60000//30000
#define BATTERY_REPORT_TIMER_MS                  1500000 //TODO 3000000
//----------------------------------------------------------------------------------------------------------//
#define bat_num //needed for power config cluster to work
//----------------------------------------------------------------------------------------------------------//
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
{
    zb_uint8_t battery_voltage; // Atribute 3.3.2.2.3.1
    zb_uint8_t battery_size; // Atribute 3.3.2.2.4.2
    zb_uint8_t battery_quantity; // Atribute 3.3.2.2.4.4
    zb_uint8_t battery_rated_voltage; // Atribute 3.3.2.2.4.5
    zb_uint8_t battery_alarm_mask; // Atribute 3.3.2.2.4.6
    zb_uint8_t battery_voltage_min_threshold; // Atribute 3.3.2.2.4.7
} zb_zcl_power_config_attr_t;*/
//----------------------------------------------------------------------------------------------------------//
typedef struct
{
    zb_ieee_addr_t upgrade_server;
    zb_uint32_t    file_offset;
    zb_uint32_t    file_version;
    zb_uint16_t    stack_version;
    zb_uint32_t    downloaded_file_ver;
    zb_uint32_t    downloaded_stack_ver;
    zb_uint8_t     image_status;
    zb_uint16_t    manufacturer;
    zb_uint16_t    image_type;
    zb_uint16_t    min_block_reque;
    zb_uint16_t    image_stamp;
    zb_uint16_t    server_addr;
    zb_uint8_t     server_ep;
} ota_client_ota_upgrade_attr_t;
//----------------------------------------------------------------------------------------------------------//
/* Main application customizable context. Stores all settings and static values. */
typedef struct
{
    zb_zcl_basic_attrs_ext_t        basic_attr;
    zb_zcl_on_off_attrs_ext_t       on_off_attr;//0006
    zb_zcl_power_config_attr_t      power_attr;//0001
    zb_zcl_identify_attrs_t         identify_attr;
    ota_client_ota_upgrade_attr_t   ota_attr;
} smart_valve_device_ctx_t;

static smart_valve_device_ctx_t m_dev_ctx;

#define ZB_HA_OTA_UPGRADE_CLIENT_DEVICE_ID         0xfff0  /**< Device ID */

#define ZB_DEVICE_VER_MULTI_SENSOR        1
#define ZB_SMART_VALVE_IN_CLUSTER_NUM     4                                    /**< Number of the input (server) clusters  */
#define ZB_SMART_VALVE_OUT_CLUSTER_NUM    1                                    /**< Number of the output (client) clusters  */

#define ZB_SMART_VALVE_REPORT_ATTR_COUNT  3

//----------------------------------------------------------------------------------------------------------//
/** @brief Declares cluster list.
 *
 *  @param cluster_list_name            Cluster list variable name.
 *  @param basic_attr_list              Attribute list for the Basic cluster.
 *  @param identify_client_attr_list    Attribute list for the Identify cluster (client).
 *  @param identify_server_attr_list    Attribute list for the Identify cluster (server).
 *  @param temp_measure_attr_list       Attribute list for the Temperature Measurement cluster.
 *  @param pressure_measure_attr_list   Attribute list for the Pressure Measurement cluster.
 */
#define ZB_DECLARE_SMART_VALVE_CLUSTER_LIST(                           \
      cluster_list_name,                                               \
      basic_attr_list,                                                 \
      on_off_attr_list,                                                \
      power_config_attr_list,                                          \
      identify_server_attr_list,                                       \
      ota_upgrade_attr_list)                                           \
      zb_zcl_cluster_desc_t cluster_list_name[] =                      \
      {                                                                \
        ZB_ZCL_CLUSTER_DESC(                                           \
          ZB_ZCL_CLUSTER_ID_BASIC,                                     \
          ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),           \
          (basic_attr_list),                                           \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                  \
          ZB_ZCL_MANUF_CODE_INVALID                                    \
        ),                                                             \
        ZB_ZCL_CLUSTER_DESC(                                           \
          ZB_ZCL_CLUSTER_ID_ON_OFF,                                    \
          ZB_ZCL_ARRAY_SIZE(on_off_attr_list, zb_zcl_attr_t),          \
          (on_off_attr_list),                                          \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                  \
          ZB_ZCL_MANUF_CODE_INVALID                                    \
        ),                                                             \
        ZB_ZCL_CLUSTER_DESC(                                           \
          ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                              \
          ZB_ZCL_ARRAY_SIZE(power_config_attr_list, zb_zcl_attr_t),    \
          (power_config_attr_list),                                    \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                  \
          ZB_ZCL_MANUF_CODE_INVALID                                    \
        ),                                                             \
        ZB_ZCL_CLUSTER_DESC(                                           \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                  \
          ZB_ZCL_ARRAY_SIZE(identify_server_attr_list, zb_zcl_attr_t), \
          (identify_server_attr_list),                                 \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                  \
          ZB_ZCL_MANUF_CODE_INVALID                                    \
        ),                                                             \
        ZB_ZCL_CLUSTER_DESC(                                           \
          ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,                               \
          ZB_ZCL_ARRAY_SIZE(ota_upgrade_attr_list, zb_zcl_attr_t),     \
          (ota_upgrade_attr_list),                                     \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                  \
          ZB_ZCL_MANUF_CODE_INVALID                                    \
        )                                                              \
      }

//----------------------------------------------------------------------------------------------------------//
/** @brief Declares simple descriptor for the "Device_name" device.
 *  
 *  @param ep_name          Endpoint variable name.
 *  @param ep_id            Endpoint ID.
 *  @param in_clust_num     Number of the supported input clusters.
 *  @param out_clust_num    Number of the supported output clusters.
 */
#define ZB_ZCL_DECLARE_SMART_VALVE_DESC(ep_name, ep_id, in_clust_num, out_clust_num)  \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =         \
  {                                                                                   \
    ep_id,                                                                            \
    ZB_AF_HA_PROFILE_ID,                                                              \
    ZB_HA_LEVEL_CONTROL_SWITCH_DEVICE_ID,                                             \
    ZB_DEVICE_VER_MULTI_SENSOR,                                                       \
    0,                                                                                \
    in_clust_num,                                                                     \
    out_clust_num,                                                                    \
    {                                                                                 \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                        \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                       \
      ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                                                 \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                     \
      ZB_ZCL_CLUSTER_ID_OTA_UPGRADE                                                   \
    }                                                                                 \
  }

//----------------------------------------------------------------------------------------------------------//
/** @brief Declares endpoint for the multisensor device.
 *   
 *  @param ep_name          Endpoint variable name.
 *  @param ep_id            Endpoint ID.
 *  @param cluster_list     Endpoint cluster list.
 */
#define ZB_ZCL_DECLARE_SMART_VALVE_EP(ep_name, ep_id, cluster_list)               \
  ZB_ZCL_DECLARE_SMART_VALVE_DESC(ep_name,                                        \
      ep_id,                                                                      \
      ZB_SMART_VALVE_IN_CLUSTER_NUM,                                              \
      ZB_SMART_VALVE_OUT_CLUSTER_NUM);                                            \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,            \
                                     ZB_SMART_VALVE_REPORT_ATTR_COUNT);           \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id,                                     \
      ZB_AF_HA_PROFILE_ID,                                                        \
      0,                                                                          \
      NULL,                                                                       \
      ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                     \
      cluster_list,                                                               \
      (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                           \
      ZB_SMART_VALVE_REPORT_ATTR_COUNT, reporting_info## device_ctx_name, 0, NULL)


#ifdef __cplusplus
}
#endif

