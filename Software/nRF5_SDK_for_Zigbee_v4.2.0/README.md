The Firmware is based on Nordic's nRF5 SDK for Thread and Zigbee. You can download it here:
https://www.nordicsemi.com/Products/Development-software/nRF5-SDK-for-Thread-and-Zigbee/Download?lang=en#infotabs

To be able work on this firmware or just compile your own FOTA package you need to place content of this folder into "nRF5 SDK for Thread and Zigbee" folder.

Then you need to take a look at Nordic's documentation https://infocenter.nordicsemi.com/topic/struct_sdk/struct/sdk_thread_zigbee_latest.html

To open project use SEGGER IDE. 
To make your own FOTA pack there is a "OTA" folder with scripts that generate Zigbee OTA file.