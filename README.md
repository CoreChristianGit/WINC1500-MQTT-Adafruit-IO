# WINC1500-MQTT-Adafruit-IO
----------------------------------------------------------------------------------------------------------
A repo that contains useful materials to connect an Arduino to Adafruit IO using a WINC1500 Shield
----------------------------------------------------------------------------------------------------------
Follow along with Chapter 6 at: https://core-electronics.com.au/tutorials/arduino-workshop-for-beginners.html

The Adafruit_IO_MQTT.ino file is the example code to get your WINC1500 up and running with Adafruit IO

Note:
There are a few libraries required for this example code to work:
  <SPI.h>                     - Preinstalled with Arduino, should be highlighted orange in the example code 
  <Servo.h>                   - Preinstalled with Arduino, should be highlighted orange in the example code
  <WiFi101.h>                 - Available for download from the Arduino Library Manager (Sketch>Include Library>Manage Libraries)
  "Adafruit_MQTT.h"           - These final two libraries can be downloaded from:                                                         
  "Adafruit_MQTT_Client.h"    - https://github.com/adafruit/Adafruit_MQTT_Library/archive/master.zip
                              - Note that the unzipped file must be renamed 'Adafruit_MQTT' and manually placed within the Arduino
                              - Libraries folder
