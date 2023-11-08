Here is basic guide how to upgrade your TWV with FOTA pack in Home Assitant using ZHA or Zigbee2MQTT.

OTA in ZHA:
1. Go to File editor and press folder icon
2. Select configuration.yaml file
3. Add those lines to it

zha:
  zigpy_config:
    network:
      channel: 15             # What channel the radio should try to use.
    ota:
      otau_directory: /config/FOTA
      ikea_provider: true                        # Auto update Trådfri devices
      ledvance_provider: true                    # Auto update LEDVANCE/OSRAM devices
      salus_provider: true                       # Auto update SALUS/Computime devices
      #otau_directory: /path/to/your/ota/folder  

4. Again in File editor press folder icon and create new folder named “FOTA“
5. Go to FOTA folder and press upload file
6. We need to upload FOTA image file witch have extension .zigbee
7. Now we need to restart HA go to Settings → System and press restart 


How to manually start FOTA process:
1. Go to TWV page in Settings → Device & Service → Device → TWV# and press down arrow 
2. Copy your ieee and store it somewhere or for next steps open new window:
3. In your Home Assistant go to Developer Tools->Services 
4. Press “Go to YAML mode“
5. Paste in this command:

service: zha.issue_zigbee_cluster_command
data:
  ieee: f4:ce:36:1f:4f:64:e7:64
  endpoint_id: 1
  cluster_id: 25
  cluster_type: out
  command: 0
  command_type: client
  args:
    - 0
    - 100
  manufacturer: 123

6. You need to change your ieee: f4:ce:36:1f:4f:64:e7:64
7. Return to Developer tools and paste your ieee
8. press “Call service“
9. If everything done right you the "Call service“ button become green
10. Now go to Settings → Devices & Services → ZHA and press add device
11. Here press show logs
12. If the above is done correctly, you will see logs about the FOTA process that are constantly changing and this wall of text will not stop. You can even see line of progress in % witch will look like: OTA upgrade state 1.0 - it’s means 1% done.


Zigbee2MQTT OTA update with local file

1. Go to File editor → open folder zigbee2mqtt → there open configuration.yaml file
2. Add those lines to the configuration.yaml file

ota:
    zigbee_ota_override_index_location: my_index.json

3. In File editor press browse file system
4. Press create new file
5. Create file named “my_index.json“
6. In the same directory create folder named “ota“
7. Go to that folder and press upload file
8. Select file to upload. Latest OTA file lies in this folder.
9. Go back to zigbee2mqtt folder and open created file “my_index.json“ and fill it with these lines. OTA image file name may be different from what you have, correct it accordingly. 

[
    {
        "url": "/config/zigbee2mqtt/ota/007B-0141-01060101-good_image.zigbee"
    }    
]
 
10. Save file and restart Zigbee2MQTT addon. To restart go to Settings → addons → Zigbee2MQTT → restart
11. After restart you can update your device with OTA. Go to Zigbee2MQTT addon and select OTA. Here you can “check for new update“ and if Z2M have new update The button will change to “update device“
12. After you click "update device", Z2M will ask you to continue ok or cancel, click ok and OTA will start automatically. Z2M provides you with a percentage of complete and estimated completion time.