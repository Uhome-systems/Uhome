//----------------------------------------------------------------------------------------------------------
//    Changelog
//----------------------------------------------------------------------------------------------------------
/*
 ----- 09.05.2022 ------
 - changed  #define ZIGBEE_CHANNEL 1516  in sdk_config.h - (tested-ok)
 - changed names, defines of sensor clusters, contexts etc. in main.c and zb_multi_sensor.h - (tested-ok)
 - added zigbee_init() - (tested-ok)
 - removed sensorsim.h and all related functions from main.c - (tested-ok)
 - added zb_smart_valve.h and removed simmilar zb_multi_sensor.h - (tested-ok)
 - smart_valve_inits.h to project - (will test after custom board is added)
 - added custom_board.h and changed from PCA10056 to CUSTOM_BOARD in project properties (tested-ok)
 - added bsp_init() - (tested - caused hard fault)
 - partialy added bsp_init() - (tested-ok)
 - added ww_driver_pins_init() - (tested-ok)
 - added forget_network() - (tested-semi-ok(LEDs and buttons is not working))
 - removed simulator reporting timer(func and init) - (tested-hard fault)
 - resolved pervious hard fault - it was caused by another timer start in zboss_signal_handler() - (tested-ok)
 - buttons and LED working (tested-ok)
 ----- 10.05.2022 ------
 - after some test I found that buttons is woking but have some issues - (fixing-ok)
 - buttons issue is: after init, any button press/release(it works ok) 
          but after that in event queque there is reset button pressed - (fixing-ok)
 - found some changes to buttons init in bsp.c - (fixing-still need to copy tham and edit)
 - changed in project settings <file file_name="../../../../../../../components/libraries/bsp/bsp.c" /> - was wrong uproach ctl+z
 - added files to project(manualy and in solution explorer) sv_bsp.c,sv_bsp.h,sv_bsp_config.h 
          and changed their defines in main.c,sv_bsp.c - (tested-fail it started auto leave-reconnect)
 - resolving why it's auto-leaves network and cant reconnect:
          *in HA there are sometimes two times "device joined network" with same addres
          *in Ubiqua sniffer I still see constant "data request" from smart valve
          *for now I conclude that there is something wrong with reports, 
            so for now will continue with removing unneded clusters.
 ----- 11.05.2022 ------
 - removing temperature and preasure measurment clusters from init - result(failed with hard fault error ... again) - resolving
 - after multiple tests with sniffer and changing versions from repository I guess thare was some error in repo cleaning procedure,
          or it's hardware problem that appears ocasionaly. For now it's all working as good as multi sensor example. Will continue adding/removing features.
 - removing temperature and preasure measurment clusters from init - result (works good)
 - found why smart valve disconnects from net sometimes, it's related with sleep mode(in new SDK it's been chaged and now it
          works but it need to make some changes for connection to HA) for now I turned off that mode.
 - another save point in repository.
 ----- 12.05.2022 ------
 - adding power config cluster - result(hard fault on packaging descriptor, my guess it's somehow related to power config cluster declaration
          tryed different declaration, macros re-writing, change order of clusters nothing helps) - for now will return to start point and try to add on/off cluster.
 - adding on/off cluster - result(it works, but only if identify clusters in macro are last)
 - adding check_valve_on_off_status_and_set_attr() and related to reporting on/off strings of code - 
          result(reporting work as intendent)
 - adding ZB_ZCL_REGISTER_DEVICE_CB(zcl_device_cb) to zigbee_init() and zcl_device_cb() - result(works good)
 - adding motor control logic to zcl_device_cb - result(works good)
 - adding app_timer for fault(if valve opens/close too long) report - result(works good)
 - fast status check and make of new save point:
          - works good(motor,buttons,reporting,clusters)
          - works mediocre(LEDs(some wrong defines), connection(double connection), re-connection(indication of LEDs))
          - works bad or not at all(sleep(interfers with connection and auto-leave on connect), battery report(couldn't connect it))
 ----- 13.05.2022 ------
 - adding power config cluster - result(not working):
          - to connect first time after uploading new firmware about 3 times needed to restart...
          - tryed "small" and "big" power config cluster difference that for "big" need to define but_num
          - tryed to add reporting capability to voltage ID param in zb_zcl_power_config.h - no change
          - HA sees this cluster but not trying to bind it ...
          - after changing zb_set_rx_on_when_idle(ZB_FALSE) HA starts binding of power cluster.
          - another hint ALWAYS RE-BUILD project. Changing zb_set_rx_on_when_idle(ZB_FALSE) from ZB_TRUE has no effect untill RE-BUILD.
 - fast status check and make of new save point:
          - works good(motor,buttons,reporting,on/off and battery clusters)
          - works mediocre(LEDs(some wrong defines), connection(double connection), re-connection(indication of LEDs))
          - works bad or not at all(sleep)
 - LED indication of re-connect fixed. After re-connect initialization blinks LED until first packets from HA recived.
 - after power_config cluster and zb_set_rx_on_when_idle(ZB_FALSE) connection to HA now have some delays as before.
 - remed sleep_now() in case ZB_COMMON_SIGNAL_CAN_SLEEP in zigbee_helpers.c
 - found and fixed bug with fail timer works unpredictibly. Error was if timer from pervious run is not stoped.fixed - (test-ok)
 ----- 16.05.2022 ------
 - testing new SDK firmware on PCB v3:
          - connect same as v2 PCB - double connect than leave
          - re-connect ok-ish(made 5 re-coonects and all was good but with some delays)(sometimes self dissconnect after connection)
          - LED indication, motor, reporting, buttons - ok
          - all clusters work - ok
          - after few test I decided to change batteries and dissconnect dissapeared(disscharged battery volatge 1.22, 1.21, 1.21, 1.19)
 - save point in repository
 ----- 20.05.2022 ------
 - changed in sdk_config.h:
          - #define NRF_LOG_BACKEND_UART_ENABLED 01
          - #define NRF_LOG_ENABLED 01
          - #define NRF_LOG_DEFAULT_LEVEL 03
          - #define NRFX_UART_ENABLED 01
 - removed log_init() and it's mentioning in main.c
 ----- 24.05.2022 ------
 - Started at 11mA of power consumption
 - after adding nrf_power_dcdcen_set(true);(zigbee_init()) and #include "nrf_power.h"(main.c) - consumption droped to 7mA
 - #define UART_ENABLED 01 #define UART0_ENABLED 01 #define NRFX_UART_ENABLED 01 #define NRFX_UARTE_ENABLED 01 - cannot remove it 
 - disabling app timer gave me consumption  - 5mA(tested, on/off works, but not stoping at the end and no reporting on 3rd button)
 ----- 31.05.2022 ------
 - sd_app_evt_wait(); in main.c while loop is best for sleep(in this func CPU wait's for event aka NOP in while)
 - zb_bdb_set_legacy_device_support(1); - improved connection speed and stability, because of HA is old revision of Zigbee stack
 - zb_osif_sleep(100);(added in zigbee_helpers.c) best way to go sleep mode, it's WEAK func if needed can try to remake it to meet our demands.
 ----- 06.06.2022 ------
 - current rise peacks repeats every 10 seconds
 - sleep current on V2 PCB with bad LDO(LD33) is about 10uA(done by calculation not real meassurement).
 - New bug found - sometimes device hangs, consuming about 200uA more than in sleep mode and not reactiong to any commands or buttons.(Tryed to repeat but no luck)
 ----- 10.06.2022 ------
 - fixed Buzzer pin define in custom_board.h to P1.10
 - added fork define in smart_valve_inits.h for DRV8837/8838 motor driver variations
 ----- 13.06.2022 ------
 - pressed button pin consumption is about 300uA(per pin) in current configuration - need to improve
 ----- 17.06.2022 ------
 - added check_reverse_status() - to be able return from manual valve switch to automat
 - re-made logic of valve open/close in commant recived and button handler(for reverse movement)
 - tested open/close and open/close after manual reversing - ok
 - added valve_close_with_timer() and valve_open_with_timer() for more readeable code.
 ----- 22.06.2022 ------
 - added re-define of weak zb_osif_sleep() to main.c but it's commented for now
 - changed buttons PULLUP to NOPULL in custom_board.h for testing with external pullup resistors 470kOhm
 - after adding 4 external pullup resistors 470kOhm each device works fine will continue testing
 ----- 23.06.2022 ------
 - after device working from yesterday(all night in sleep mode) first responses was very slow, 
    but after reloading ZHA in HA works good again,
    I guess slow response of ZHA can be related to spamming data request of Smart Valve(will test it more)
    current consumption is 185-195uA,
 - removing TPS2114 from scheme shoved that it's current consumption(TPS2114) - 60uA.
    so after removeng it the minimal current in sleep mode is about 125-135uA
 ----- 23.06.2022 ------
 - status update on long term connection stability:
      - switched multimeter to biger scale and checked all power contacts(mb it caused problem)
      - updated HA to latest version
      overall it works better after few days in sleep it retains fast response 
      to any command and reporting of changed status of control limit switch. 
 ----- 25.07.2022 ------
 - BIG BUG found(while testing Z2M then returning to ZHA I faced problem of connecting to HA, tryed 10+ times with reset and power, but until I removed
      from network Philips lightbulb(it is router) I could not connect but after removing it connection was instant)
 ----- 11.08.2022 ------
 - changed in sdk_config.h:
          - #define NRF_LOG_BACKEND_UART_ENABLED 1
          - #define NRF_LOG_ENABLED 1
          - #define NRF_LOG_DEFAULT_LEVEL 4
          - #define NRFX_UART_ENABLED 1
          - #define ZIGBEE_TRACE_LEVEL 40
 ----- 15.08.2022 ------
 - added pairing timer 60 sec that turn off advertising after time pased even if network is not found(tested timer - ok)
 - zb_zcl_power_config.h WAS CHANGED line 475  added reporting and line 588 reporting removed
 ----- 07.09.2022 ------
 - added new file to the project changeLog.h - for this log
 - added new file Battery_meassurement.c -  for SAADC init and handker
 ----- 12.09.2022 ------
 - somehow I managed to make reporting of power cluster on HA - but I still not shure everything is ok - need to review is later --------------------------------
 - on/off logic changed to open/close to work in HA. Logic was reversed. Tested - ok
 - removed include that was lost in pervious commit
 ----- 22.09.2022 ------
 - in sdk_config.h modified #define NRFX_GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS 4//1 -- it is a number of allowed low power inputs = 4 our buttons
 ----- 23.09.2022 ------
 - added error check with output to nrf log.
 - made commit to repository with tested OTA functionality
 - removed       <file file_name="../../../../../../../components/zigbee/common/zigbee_helpers.c" /> from project settings
 - added zigbee_helpers_TWV.c and zigbee_helpers_TWV.h to modify them to our product needs without touching SDK files
 ----- 30.09.2022 ------
 - added ZB_ED_ROLE; to c_preprocessor_definitions= in project config.
 - changed LIBZBOSS_CONFIG_FILE=&quot;libzboss_config.ed.h&quot;; from LIBZBOSS_CONFIG_FILE=&quot;libzboss_config.h&quot;; in c_preprocessor_definitions= in project config.
 ----- 02.10.2022 ------
 - added user_input_indicate(); to pairing button single press - so TWV can connect to new network after boot or re-connect if network was lost
 - added user_input_indicate(); to battery reporting - so re-connect to lost network will be initiated regularly
 - changed battery reporting time to every 30 minutes
 - added m_wait_for_user_input = ZB_TRUE; to start_network_rejoin_ED(zb_uint8_t param) - so re-connecting to lost network will continue after failed attempt
 - removed start_network_rejoin(); from case ZB_BDB_SIGNAL_DEVICE_FIRST_START: in zigbee signal handler - for the TWV not auto start of re-join on boot
 ----- 17.10.2022 ------
 - rework of ADC battery measurment - tried to recreate typical alkaline battery disscharge curve. made aproximation in 3 steps 100-90%, 90-34%, 34-0%
 - added "not jumping" battery voltage - if curent meassurement % is greater than pervious%+5% than it will update current if not than current = pervious
 - adde eception to battery meassurement - if motor is working than meassured battery voltage is perviously meassured value
 ----- 31.10.2022 ------
 - removed m_is_rejoin_in_progress from sleep route IF , it caused sleep bug(if connection lost, TWV never goes to sleep)
 ----- 28.11.2022 ------
 - changed SMART_VALVE_INIT_BASIC_MANUF_NAME to UHome
 - changed SMART_VALVE_INIT_BASIC_MODEL_ID to TWV 
 ----- 07.12.2022 ------
 - added valve_ep_handler - for now it's empty
 - changed LED indication during connection - after start with button 50% blinking, after joining network fast partly connected to comunication rx/tx, after reciving identify callback tun off led
 ----- 08.12.2022 ------
 - added nrfx_wdc.c to nRF_drivers folder of the project
 - added watchdog_init() and watchdog_handler() also included "nrf_drv_clock.h" and "nrf_drv_wdt.h"
 - WDT was set to 5000ms and stop in sleep mode
 - feeding WDT in main loop and in zboss_signal_handler 
 ----- 11.12.2022 ------
 - changed NRFX_WDT_CONFIG_RELOAD_VALUE 10000 - WDT was activated at 5000
 ----- 17.12.2022 ------
 - adding second control switch
    - custom_board.h change button defines and add new button
    - cnage in sdk_config.h #define NRFX_GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS 5 - number of buttons
    - add defined, macro and pin config(for release) to smart_valve_inits.h 
    - add event case for new button in button_handler()
    - test all buttons working - OK
 ----- 18.12.2022 ------
 - changed buttons event timer APP_TIMER_TICKS(600) from 50ms
 ----- 19.12.2022 ------
 - tested all reworked open/close logic - failed - gearbox switchers are not pressed if control switchers stoped motor it breaks logic of open/close
 - added exception gearbox_moves_to_mid_possition to valve_open_close_handler so gearbox limit switcher always pressed even if control switchers pressed.
 ----- 23.12.2022 ------
 - changed battery quantity in smart_valve_clusters_attr_init from "1" to "4"
 - testing send msg error code - 0 looks good need to check sniffer
 ----- 09.01.2023 ------
 - added foonction "setter" void can_rejoin_after_leave_foo(bool can); to set permission to start network rejoin on leave. 
    It will distinguish leave-rejoin-with-button from leave command from coordinator. 
    We don't need auto rejoining after leaving network, only joining with pairing button
 ----- 18.01.2023 ------
 - changed #define PAIRING_TIMER_COUNTDOWN_MS               60000//30000
 - added BUZZER_ON on pairing button long press aka network forget
 - added  bsp_event_to_button_action_assign(BUTTON_PAIRING_LIST_NUMBER, BSP_BUTTON_ACTION_RELEASE, BSP_EVENT_KEY_5); - release pairing button event to BUZZER_OFF
 - moved stop_pairing(); to if(ZB_JOINED() && pairing_in_progress) in signal handler - so it's correct LED indication
 - changed #define IEEE_CHANNEL_MASK     0x07FFF800UL  to   0x0000E000UL - so it's less band to scan for connection
 ----- 21.01.2023 ------
 - added - else m_wait_for_user_input = ZB_FALSE; to user_input_indicate function so when TWV connects battery reporting don't mess with wait_for_user_input flag
 ----- 23.02.2023 ------
 - added motor_working = false; to open/close buttons press.
 - added !motor_working to auto close timer, so it will not activates when motor is already working.
 ----- 28.02.2023 ------
 - changed time delay for button press from 600ms to 450ms
 ----- 22.03.2023 ------
 - Auto rebooting bug was hotfixed with increased delay on motor reverse(MOTOR_DELAY_MS) witch was 200ms and now its 2000ms. 
    Tests showed its good enough for gearbox to stop moving before reverse movement starts.
 - Fixed context on timers, wrong context was on different timer calls. It was not big error but I found it when searched auto rebooting bug and fixed it.
 - Firmware release date changed for future OTA release.
 ----- 11.04.2023 ------
 - double click logic added. Its bad, because it has so many state flags. But it works.
 ----- 12.04.2023 ------
 - added main.h file to the project
 - changed variable pairing_in_progress to extern so I can use it in Zigbee_helpers.c. This made for fixing auto disconnect, making exception in pervious fix so it only works during connection by button.
 - removed unused functions and calls to debug functions
 - removed messaging cluster from device context
 - prepared for OTA package 08.
----------------------------------------------------------------------------------------------------------
*/
/*
FOTA upgrade package release instruction:
Fully test all device functionality
change SMART_VALVE_INIT_BASIC_DATE_CODE to the FOTA package release date
In TWV_flashing_using_SEGGER_hex.bat  change --application-version 0x01070101 to the next number
In TWV_PACKAGE_using_SEGGER_hex.bat change --application-version 0x01070101 and --zigbee-ota-fw-version 0x01070101 to the next number

Change version tag SMART_VALVE_INIT_BASIC_SV_VERSION
in production config file change HW version if needed

Test the full FOTA upgrade cycle on the device with the previous firmware version
Check upgraded device functionality and check the firmware version in the OTA cluster and date code in the basic cluster.
https://kdeu.atlassian.net/wiki/spaces/UHOME/pages/44433437/FOTA+upgrade+instruction - update Confluence page
D:\Dropbox\F\!ownProduct\6_Smart home\01_Valve\8_PROTOTYPE AND TESTING\OTA_Firmware_latest - and dropbox 
D:\Dropbox\F\!ownProduct\6_Smart home\01_Valve\8_PROTOTYPE AND TESTING\OTA_Firmware_latest\old_OTA_files - too
make commit with version tag
*/
//zb_get_long_address