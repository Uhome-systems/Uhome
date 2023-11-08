cd ..\..\..
make -j -C ota\bootloader\pca10056\mbr\armgcc
make -j -C experimental\SmartValve\pca10056\blank\armgcc
cd experimental\SmartValve\OTA
nrfutil settings generate --family NRF52840 --application ..\pca10056\blank\armgcc\_build\nrf52840_xxaa.hex --application-version 0x01010101 --bootloader-version 1 --bl-settings-version 2 --app-boot-validation VALIDATE_ECDSA_P256_SHA256 --key-file priv.pem settings.hex 
mergehex -m ..\pca10056\blank\armgcc\_build\nrf52840_xxaa.hex settings.hex -o dfu_client.hex
nrfjprog -f nrf52 --eraseall
nrfjprog -f nrf52 -r --program ..\..\..\..\..\components\softdevice\mbr\hex\mbr_nrf52_2.4.1_mbr.hex --chiperase
nrfjprog -f nrf52 -r --program ..\..\..\ota\bootloader\pca10056\mbr\armgcc\_build\nrf52840_xxaa_mbr.hex
nrfjprog -f nrf52 -r --program dfu_client.hex --sectorerase
set /p DUMMY=Hit ENTER to continue...

