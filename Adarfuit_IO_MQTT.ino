/*  Name: Adafruit_IO_MQTT
 *  Date Created: 1/06/2018
 *  Description: Connects to various Adafruit feeds for subscribing and publishing 
 *  Connections:
 * 		D10 =>	WINC1500 Chip Select (Shield)
 * 		D9 	=>	Servo
 * 		D8	=>	Red LED
 * 		D7	=> 	WINC1500 Interrupt Out (Shield)
 * 		D5  =>	WINC1500 Hardware Reset (Shield)
 * 		D3  =>	Pushbutton
 * 		D2  =>	WINC1500 Enable (Shield)
 */

//  Include Relevant Libraries
#include <SPI.h>
#include <WiFi101.h>
#include <Servo.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

//  Declare WIFI Pins for the WINC1500
#define WINC_CS   10
#define WINC_IRQ  7
#define WINC_RST  5
#define WINC_EN   2

//  Declare WIFI credentials for local access point 
char ssid[] = "Network ID";
char pass[] = "Password";
int status = WL_IDLE_STATUS;

//  Declare Adafruit Constants for Publish-Subscribe 
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "Your Adafruit Username"
#define AIO_KEY         "Your Adafruit Key"

//  Set up the WIFI Client
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

//  Set up feeds for publishing and subscribing respectively
Adafruit_MQTT_Publish buttonPress = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/buttonpress");
Adafruit_MQTT_Subscribe ledon = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ledon");
Adafruit_MQTT_Subscribe servo = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/servo");

//  Create the Servo Object
Servo myServo;

//  Define Pin Assignments
#define servoPin 9
#define ledPin 8
#define butPin 3

//  Declare variables and constants
uint8_t servoPos = 0;
uint8_t prevButtonCount = 0;
uint8_t firstRun = 1;
uint32_t buttonCount = 0; 

void setup(){
  //  Initialise the WiFi pins
  WiFi.setPins(WINC_CS, WINC_IRQ, WINC_RST, WINC_EN);

  //  Initialise the Serial Port
  Serial.begin(115200);
  Serial.println(F("Adafruit IO using MQTT and WINC1500"));

  //  Initialise the Client
  Serial.println(F("Attempting to initialise the WINC1500 Shield"));
  if(WiFi.status() == WL_NO_SHIELD){
    Serial.println(F("WINC1500 not found"));
    while(1);
  } else {
    Serial.println(F("WINC1500 located on board!"));
  }
  
  //  Define pinModes
  pinMode(ledPin, OUTPUT);
  pinMode(butPin, INPUT_PULLUP);

  //  Declare the interrupt
  attachInterrupt(digitalPinToInterrupt(butPin), incrementButtonPresses, FALLING);

  //  Initialise the Servo Pin
  myServo.attach(servoPin);

  //  Create subscriptions 
  mqtt.subscribe(&ledon);
  mqtt.subscribe(&servo);
}

void loop(){
  //  Check for a connection to the internet and Adafruit IO 
  MQTT_connect();

  if(firstRun){
    buttonPress.publish(buttonCount);
    firstRun = 0;
  }
  
  //  Create a pointer to the subscribe object
  Adafruit_MQTT_Subscribe *subscription;
  while((subscription = mqtt.readSubscription(500))){
    //  If the subscription pertains to the ledon feed, do this 
    if(subscription == &ledon){
      //  If the subscription said ON, do this
      if(!strcmp((char *)ledon.lastread, "ON")){
        digitalWrite(ledPin, HIGH);
      } else {
        digitalWrite(ledPin, LOW);
      }
    }
    //  If the subscription pertains to the servo feed, do this
    if(subscription == &servo){
    myServo.write(atoi((char *)servo.lastread));    
    }
  }   

  //  If buttonCount has increased, publish it
  if(prevButtonCount != buttonCount){
    buttonPress.publish(buttonCount);
  }
  
  //  Save the previous value for next loop
  prevButtonCount = buttonCount;
}

void incrementButtonPresses(){
  delay(10);
  if(digitalRead(butPin) == LOW){
    buttonCount++;
  }
}

void MQTT_connect(){
  //  Declare Variables and constants
  int8_t ret;
  uint8_t timeout = 10;

  //  If not connected, try to connect
  while(WiFi.status() != WL_CONNECTED) {
    //  Print data to the user
    Serial.print(F("Attempting to connect to SSID: "));
    Serial.println(ssid);
    
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    //  Try until connected or timeout
    while(timeout && (WiFi.status() != WL_CONNECTED)) {
      timeout--;
      delay(1000);
    }
  }
  
  //  Return to loop if connected to the network
  if(mqtt.connected()) {
    return;
  }

  //  Print Data to the user
  Serial.println(F("Connecting to MQTT... "));

  //  If the WiFi remains disconnected run this
  while((ret = mqtt.connect()) != 0){ 
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
  }

  //  Print a success message
  Serial.println(F("MQTT Connected!"));
}