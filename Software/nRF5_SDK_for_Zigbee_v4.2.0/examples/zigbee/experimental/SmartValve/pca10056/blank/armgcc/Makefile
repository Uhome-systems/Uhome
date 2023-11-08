PROJECT_NAME     := zigbee_multi_sensor_pca10056
TARGETS          := nrf52840_xxaa
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../../..
PROJ_DIR := ../../..

$(OUTPUT_DIRECTORY)/nrf52840_xxaa.out: \
  LINKER_SCRIPT  := zigbee_multi_sensor_gcc_nrf52.ld

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52840.S \
  ../config/sv_bsp.c \
  $(PROJ_DIR)/pca10056/blank/ses/Battery_measurement.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_saadc.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_uart.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52840.c \
  $(SDK_ROOT)/external/zboss/osif/zb_nrf52_common.c \
  $(SDK_ROOT)/external/zboss/osif/zb_nrf52_logger.c \
  $(SDK_ROOT)/external/zboss/osif/zb_nrf52_nrf_logger.c \
  $(SDK_ROOT)/external/zboss/osif/zb_nrf52_nvram.c \
  $(SDK_ROOT)/external/zboss/osif/zb_nrf52_timer.c \
  $(SDK_ROOT)/external/zboss/osif/zb_nrf52_transceiver.c \
  $(SDK_ROOT)/external/zboss/src/commissioning/bdb/bdb_finding_binding.c \
  $(SDK_ROOT)/external/zboss/src/zcl/ha_sas.c \
  $(SDK_ROOT)/external/zboss/src/zb_error/zb_error_to_string.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_alarms_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_attr_value.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_basic_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_binary_input.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_c_daily_schedule.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_c_drlc.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_c_messaging.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_c_metering.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_c_price.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_c_wwah.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_color_control_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_common.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_continuous_value_change_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_control4_networking.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_dehumid_control.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_diagnostics_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_door_lock.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_el_measurement.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_en50523_appliance_events_and_alerts.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_fan_control.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_general_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_groups.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_ias_ace_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_ias_wd_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_ias_zone_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_identify_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_illuminance_measurement.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_level_control_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_main.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_meter_identification.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_nvram.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_occupancy_sensing_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_on_off_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_on_off_switch_config.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_ota_upgrade_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_ota_upgrade_common.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_ota_upgrade_minimal.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_ota_upgrade_srv_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_poll_control_client.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_poll_control_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_power_config_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_pressure_measurement.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_rel_humidity.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_reporting.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_s_daily_schedule.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_s_drlc.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_s_messaging.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_s_metering.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_s_price.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_s_wwah.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_scenes.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_shade_config_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_temp_measurement.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_thermostat_commands.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_thermostat_ui_config.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_time.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_window_covering.c \
  $(SDK_ROOT)/external/zboss/src/zcl/zcl_wwah_common.c \
  $(SDK_ROOT)/external/zboss/src/commissioning/bdb/zdo_commissioning_bdb.c \
  $(SDK_ROOT)/external/zboss/src/commissioning/bdb/zdo_commissioning_bdb_formation.c \
  $(SDK_ROOT)/external/zboss/src/commissioning/legacy/zdo_commissioning_classic.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_init.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_shared.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer2.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/assert/assert.c \
  $(SDK_ROOT)/components/libraries/crc32/crc32.c \
  $(SDK_ROOT)/components/libraries/timer/drv_rtc.c \
  $(SDK_ROOT)/components/libraries/mem_manager/mem_manager.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_nvmc.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
  $(SDK_ROOT)/components/libraries/queue/nrf_queue.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/components/libraries/sortlist/nrf_sortlist.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/components/libraries/sha256/sha256.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_rng.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_uart.c \
  $(SDK_ROOT)/modules/nrfx/hal/nrf_ecb.c \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd/nrf_nvic.c \
  $(SDK_ROOT)/modules/nrfx/hal/nrf_nvmc.c \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd/nrf_soc.c \
  $(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_rng.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_systick.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uart.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uarte.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_init.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_shared.c \
  $(PROJ_DIR)/../../../dfu/dfu_public_key.c \
  $(PROJ_DIR)/main.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/components/iot/background_dfu/background_dfu_block.c \
  $(SDK_ROOT)/components/iot/background_dfu/background_dfu_operation.c \
  $(SDK_ROOT)/components/iot/background_dfu/background_dfu_state.c \
  $(SDK_ROOT)/external/nano-pb/pb_common.c \
  $(SDK_ROOT)/external/nano-pb/pb_decode.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/dfu-cc.pb.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_flash.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_handling_error.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_mbr.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_settings.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_transport.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_utils.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_validation.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_ver_validation.c \
  $(SDK_ROOT)/components/zigbee/ota/zb_zcl_ota_upgrade_addons.c \
  $(SDK_ROOT)/components/zigbee/ota/zigbee_dfu_transport.c \
  $(SDK_ROOT)/components/zigbee/common/zigbee_helpers.c \
  $(SDK_ROOT)/components/zigbee/common/zigbee_logger_eprxzcl.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_chacha_poly_aead.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecdh.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_eddsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_hmac.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR)/pca10056/blank/ses \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/external/zboss/src/include \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310 \
  $(SDK_ROOT)/components/libraries/crypto/backend/cifra \
  $(SDK_ROOT)/external/fprintf \
  $(PROJ_DIR) \
  $(SDK_ROOT)/components/iot/background_dfu \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/external/zboss/osif \
  $(SDK_ROOT)/external/nrf_cc310/include \
  $(SDK_ROOT)/components/libraries/atomic_fifo \
  ../config \
  $(SDK_ROOT)/external/nRF-IEEE-802.15.4-radio-driver/src \
  $(SDK_ROOT)/external/nRF-IEEE-802.15.4-radio-driver/src/fem/three_pin_gpio \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/external/zboss/include \
  $(SDK_ROOT)/external/nrf_oberon/include \
  $(SDK_ROOT)/components/iot/background_dfu/transport \
  $(SDK_ROOT)/components/libraries/crypto \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/mem_manager \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/external/nrf_oberon \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components/libraries/crypto/backend/optiga \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_sw \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/integration/nrfx \
  $(SDK_ROOT)/components/zigbee/ota \
  $(SDK_ROOT)/modules/nrfx/drivers/include \
  $(SDK_ROOT)/components/zigbee/common \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl \
  $(SDK_ROOT)/external/zboss/include/zcl \
  $(SDK_ROOT)/components/libraries/bootloader/dfu \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/external/zboss/include/ha \
  $(SDK_ROOT)/external/nano-pb \
  $(SDK_ROOT)/modules/nrfx \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/libraries/stack_info \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/external/zboss/include/osif \
  $(SDK_ROOT)/components/libraries/sortlist \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(SDK_ROOT)/modules/nrfx/hal \
  $(SDK_ROOT)/components/libraries/mutex \
  $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/libraries/pwr_mgmt \
  $(SDK_ROOT)/external/zboss/include/addons \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw \
  $(SDK_ROOT)/external/nrf_cc310_bl/include \
  $(SDK_ROOT)/external/zboss/src/zb_error \
  $(SDK_ROOT)/integration/nrfx/legacy \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/external/nRF-IEEE-802.15.4-radio-driver/src/fem \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/libraries/sha256 \
  $(SDK_ROOT)/components/softdevice/mbr/headers \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon \
  $(SDK_ROOT)/components/libraries/bootloader \
	
# Libraries common to all targets
LIB_FILES += \
  $(SDK_ROOT)/external/nrf_cc310_bl/lib/cortex-m4/hard-float/libnrf_cc310_bl_0.9.12.a \
  $(SDK_ROOT)/external/nrf_oberon/lib/cortex-m4/hard-float/liboberon_3.0.3.a \
  $(SDK_ROOT)/external/zboss/lib/gcc/libzboss.ed.a \
  $(SDK_ROOT)/external/zboss/lib/gcc/libgppb.a \
  $(SDK_ROOT)/external/zboss/lib/gcc/nrf52840/nrf_radio_driver.a \

# Optimization flags
OPT = -Os -ggdb3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DAPP_TIMER_V2
CFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DBOOTLOADER_START_ADDR=BOOTLOADER_ADDRESS
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DCONFIG_ZIGBEE_ERROR_TO_STRING_ENABLED
CFLAGS += -DENABLE_FEM
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DLIBZBOSS_CONFIG_FILE=\"libzboss_config.ed.h\"
CFLAGS += -DMBR_PRESENT
CFLAGS += -DNRF52840_XXAA
CFLAGS += -DNRF_DFU_SETTINGS_VERSION=2
CFLAGS += -DZBOSS_BUILD
CFLAGS += -DZB_ED_ROLE
CFLAGS += -DZB_NRF_INTERNAL
CFLAGS += -DZB_NVRAM_FLASH_AUTO_ADDRESS
CFLAGS += -DZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED
CFLAGS += -DZB_ZCL_USE_ZCL7_AS_DEFAULT_REVISION
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums -Wno-packed-bitfield-compat

# C++ flags common to all targets
CXXFLAGS += $(OPT)
# Assembler flags common to all targets
ASMFLAGS += -ggdb3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DAPP_TIMER_V2
ASMFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
ASMFLAGS += -DBOARD_CUSTOM
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DENABLE_FEM
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DLIBZBOSS_CONFIG_FILE=\"libzboss_config.ed.h\"
ASMFLAGS += -DMBR_PRESENT
ASMFLAGS += -DNRF52840_XXAA
ASMFLAGS += -DZB_ED_ROLE
ASMFLAGS += -DNRF_DFU_SETTINGS_VERSION=2

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

nrf52840_xxaa: CFLAGS += -D__HEAP_SIZE=8192
nrf52840_xxaa: CFLAGS += -D__STACK_SIZE=8192
nrf52840_xxaa: ASMFLAGS += -D__HEAP_SIZE=8192
nrf52840_xxaa: ASMFLAGS += -D__STACK_SIZE=8192

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm -lstdc++


.PHONY: default help

# Default target - first one defined
default: nrf52840_xxaa

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		nrf52840_xxaa
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash erase

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/nrf52840_xxaa.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/nrf52840_xxaa.hex --sectorerase
	nrfjprog -f nrf52 --reset

erase:
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := ../config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
