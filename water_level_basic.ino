/**********************************************************************************
 *  TITLE: IoT-based Water Level Indicator using ESP32, Ultrasonic Sensor & Blynk with 0.96" OLED
 *  Click on the following links to learn more. 
 *  YouTube Video: https://youtu.be/9geREeE13jc
 *  Related Blog : https://iotcircuithub.com/esp32-projects/
 *  
 *  This code is provided free for project purpose and fair use only.
 *  Please do mail us to techstudycell@gmail.com if you want to use it commercially.
 *  Copyrighted © by Tech StudyCell
 *  
 *  Preferences--> Aditional boards Manager URLs : 
 *  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  
 *  Download Board ESP32 (2.0.5) : https://github.com/espressif/arduino-esp32
 *
 *  Download the libraries 
 *  Blynk Library (1.1.0):  https://github.com/blynkkk/blynk-library
 *  Adafruit_SSD1306 Library (2.5.7): https://github.com/adafruit/Adafruit_SSD1306
 *  AceButton Library (1.9.2): https://github.com/bxparks/AceButton
 **********************************************************************************/
/* Fill-in your Template ID (only if using Blynk.Cloud) */

#define BLYNK_TEMPLATE_ID "TMPL3Io43jMu7"
#define BLYNK_TEMPLATE_NAME "water level indicator"
#define BLYNK_AUTH_TOKEN "iA9tA7Hnm1n1RtUI7Jj0D5ojf48N226X"

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "GokulHari012";
char pass[] = "12345678";

//Set Water Level pressure in CM
int emptyTankPressure = 100 ;  //pressure when tank is empty
int fullTankPressure =  1000 ;  //pressure when tank is full

//Set trigger value in percentage
int triggerPointPer =   10 ;  //alarm will start when water level drop below triggerPoint

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Define connections to sensor
#define wifiLed    2   //D2
#define BuzzerPin  13  //D13
#define RedLed   14  //D14
#define pressureSensor   A0  //A0

//Change the virtual pins according the rooms
#define VPIN_BUTTON_1    V1 


float pressure;
int   waterLevelPer;
bool  toggleBuzzer = LOW; //Define to remember the toggle state

char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

void checkBlynkStatus() { // called every 3 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  //Serial.println("wifi");
  if (isconnected == false) {
    //Serial.println("Blynk Not Connected");
    //Serial.println("wifi false");
    digitalWrite(wifiLed, LOW);
  }
  if (isconnected == true) {
    digitalWrite(wifiLed, HIGH);
    //Serial.println("wifi true");
    //Serial.println("Blynk Connected");
  }
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(VPIN_BUTTON_1);;
}


void measurePressure(){
  
  pressure = analogRead(pressureSensor);
  waterLevelPer = map(pressure ,emptyTankPressure, fullTankPressure, 0, 100);
  Blynk.virtualWrite(VPIN_BUTTON_1, waterLevelPer);
  
  // Print result to serial monitor
  Serial.print("pressure: ");
  Serial.println(pressure);

  if (100-triggerPointPer < waterLevelPer){
      digitalWrite(RedLed, HIGH);
      if (toggleBuzzer == HIGH){
        digitalWrite(BuzzerPin, HIGH);
      }      
  }
  if (100-triggerPointPer > waterLevelPer){
      digitalWrite(RedLed, LOW);
      if (toggleBuzzer == LOW){
        digitalWrite(BuzzerPin, LOW);
      } 
  }
     
  
  // Delay before repeating measurement
  delay(100);
}

 
void setup() {
  // Set up serial monitor
  Serial.begin(115200);
 
  // Set pinmodes for sensor connections
  pinMode(wifiLed, OUTPUT);
  pinMode(RedLed, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);

  digitalWrite(wifiLed, LOW);
  digitalWrite(RedLed, LOW);
  digitalWrite(BuzzerPin, LOW);

  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  Blynk.config(auth);
  delay(1000);
 
}
 void loop() {

  measurePressure();

  Blynk.run();
  timer.run(); // Initiates SimpleTimer

}

