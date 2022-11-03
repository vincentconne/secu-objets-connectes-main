# Transforme un fichier intel hex en bianire pour arduino et l'envoie sur la carte

# UTILISATION :
# execution_intelhex_perso fichier.hex fichier.bin

# création du fichier bin à partir d'un fichier hex perso
# P:\secu-objets-connectes\arduino-1.8.16\portable\packages\Intel\tools\arc-elf32\1.6.9+1.0.1\bin\arc-elf32-objcopy.exe -v -I ihex -O binary  .\firmware_tp3_v2.33_modif.hex .\firmware_tp3_modif.bin
P:\secu-objets-connectes\arduino-1.8.16\portable\packages\Intel\tools\arc-elf32\1.6.9+1.0.1\bin\arc-elf32-objcopy.exe -v -I ihex -O binary  $args[0] $args[1]


# téléversement du firmware perso qu'on a genéré avec la commande précédente 
# P:\secu-objets-connectes\arduino-1.8.16\portable\packages\Intel\tools\arduino101load\2.0.1/arduino101load -dfu=P:\secu-objets-connectes\arduino-1.8.16\portable\packages\arduino\tools\dfu-util\0.9.0-arduino1 -bin=P:\secu-objets-connectes\firmware_tp3_modif.bin -port=COM4 -v -ble_fw_str="ATP1BLE00R-1631C4439" -ble_fw_pos=169984 -rtos_fw_str="" -rtos_fw_pos=0 -core=2.0.0
P:\secu-objets-connectes\arduino-1.8.16\portable\packages\Intel\tools\arduino101load\2.0.1/arduino101load -dfu=P:\secu-objets-connectes\arduino-1.8.16\portable\packages\arduino\tools\dfu-util\0.9.0-arduino1 -bin=P:\secu-objets-connectes\$args[1] -port=COM4 -v -ble_fw_str="ATP1BLE00R-1631C4439" -ble_fw_pos=169984 -rtos_fw_str="" -rtos_fw_pos=0 -core=2.0.0
