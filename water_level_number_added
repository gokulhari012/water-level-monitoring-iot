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


#define BLYNK_TEMPLATE_ID "TMPL3BfAm6wG1"
#define BLYNK_TEMPLATE_NAME "Water Level Indicator"
#define BLYNK_AUTH_TOKEN "Lo2Jzk_Iwm8SY3EqlYXRVOjWXrZYXQdB"

// Your WiFi credentials.
// Set password to "" for open networks.

// char ssid[] = "GokulHari012";
// char pass[] = "12345678";

char ssid[] = "Tenda_47BFC0";
char pass[] = "system123";

//Set Water Level pressure in CM
int emptyTankPressure = 430 ;  //pressure when tank is empty low 425
int fullTankPressure =  475 ;  //pressure when tank is full high 480

//Set trigger value in percentage
int triggerPointPer =   5 ;  //alarm will start when water level drop below triggerPoint ***This is percentage tolarance*** 

String call_mobile_number = "9791898999";  //Gopi
String msg_mobile_number_1 = "9791898999"; //Gopi
String msg_mobile_number_2 = "9962601292"; //sampath 
String msg_mobile_number_3 = "9944391393"; //Velu
String msg_mobile_number_4 = "8220339908"; //gokul

unsigned long previousMillis = 0;   // Variable to store the last time the function was run
const long interval = 10000;        // Interval in milliseconds (10 seconds)

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Define connections to sensor
#define wifiLed    2   //D2 build in led blue color
#define BuzzerPin  32  //P32
#define fullLed   25 //P25
#define emptyLed   14  //P14
#define pressureSensor   A0  //A0

//Change the virtual pins according the rooms
#define IOT_WATER_LEVEL_VARIABLE    V1 
#define IOT_SIM_SIGNAL_VARIABLE    V2 
#define IOT_PRESSURE_SENSOR_VARIABLE    V3 

#define IOT_EMPTY_TANK_PRESSURE    V4
#define IOT_FULL_TANK_PRESSURE    V5 

float pressure = 0;
int pressure_sensor_reading_count = 0;
int   waterLevelPer;
int  toggleBuzzer = 0; //Define to remember the toggle state
int  toggleLow = 0; //Low water level toggle for sending msg

char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

void setup() {
  // Set up serial monitor
  Serial.begin(115200);

  // Initialize SIM800L on Serial2
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17

  // Give time to initialize the SIM800L
  delay(1000);
  init_sim_module();
  
  // Set pinmodes for sensor connections
  pinMode(wifiLed, OUTPUT);
  pinMode(fullLed, OUTPUT);
  pinMode(emptyLed, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);

  digitalWrite(wifiLed, LOW);
  digitalWrite(fullLed, LOW);
  digitalWrite(emptyLed, LOW);
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
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Save the last time the function was run
    previousMillis = currentMillis;

    check_signal();
    calculatePressurePercentage();
    off_buzzer();
  }

}

void off_buzzer(){
  if (toggleBuzzer !=0 ){
    if (toggleBuzzer > 60000/interval){
        digitalWrite(BuzzerPin, LOW);
    }
    else{
        toggleBuzzer++;
    }
    
  }
}

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
  Blynk.syncVirtual(IOT_WATER_LEVEL_VARIABLE);
  Blynk.syncVirtual(IOT_SIM_SIGNAL_VARIABLE);
  Blynk.syncVirtual(IOT_PRESSURE_SENSOR_VARIABLE);
}

// Function to be called when data is received from Blynk
BLYNK_WRITE(IOT_EMPTY_TANK_PRESSURE) {
  emptyTankPressure = param.asInt(); // Use asInt(), asFloat(), or asStr() based on your data type
  Serial.print("Received data from Blynk emptyTankPressure: ");
  Serial.println(emptyTankPressure);
}
BLYNK_WRITE(IOT_FULL_TANK_PRESSURE) {
  fullTankPressure = param.asInt(); // Use asInt(), asFloat(), or asStr() based on your data type
  Serial.print("Received data from Blynk fullTankPressure: ");
  Serial.println(fullTankPressure);
}

void measurePressure(){
  
  float local_pressure_reading = analogRead(pressureSensor);
  pressure = local_pressure_reading + pressure;
  pressure_sensor_reading_count++;

  Blynk.virtualWrite(IOT_PRESSURE_SENSOR_VARIABLE, local_pressure_reading);
  
  // Print result to serial monitor
  Serial.print("pressure: ");
  Serial.print(pressure);
  Serial.print(" --  local_pressure_reading: ");
  Serial.println(local_pressure_reading);
  delay(100);
}

void calculatePressurePercentage(){
  pressure = pressure/pressure_sensor_reading_count ;
  waterLevelPer = map(pressure ,emptyTankPressure, fullTankPressure, 0, 100);

  pressure = 0;
  pressure_sensor_reading_count = 0;

  Blynk.virtualWrite(IOT_WATER_LEVEL_VARIABLE, waterLevelPer);
  // Print result to serial monitor
  Serial.print("waterLevelPer: ");
  Serial.println(waterLevelPer);

  //buzzer and send msg and make call
  if (100-triggerPointPer < waterLevelPer && waterLevelPer < 150 ){
      if (toggleBuzzer == 0){
        toggleBuzzer = 1;
        digitalWrite(BuzzerPin, HIGH);
        send_msg(msg_mobile_number_1,"TanK Full");
        send_msg(msg_mobile_number_2,"TanK Full");
        send_msg(msg_mobile_number_3,"TanK Full");
        send_msg(msg_mobile_number_4,"TanK Full");
        make_call(call_mobile_number);
      }      
  }
  else if(100-(triggerPointPer*2) > waterLevelPer){
    if (toggleBuzzer != 0){
        toggleBuzzer = 0;
        digitalWrite(BuzzerPin, LOW);
    }
  }

  //Low message send
  if (0+triggerPointPer > waterLevelPer && waterLevelPer > -50 ){
      if (toggleLow == 0){
        toggleLow  = 1;
        send_msg(msg_mobile_number_1,"Empty Tank");
        send_msg(msg_mobile_number_2,"Empty Tank");
        send_msg(msg_mobile_number_3,"Empty Tank");
        send_msg(msg_mobile_number_4,"Empty Tank");
      }      
  }
  else if(0+(triggerPointPer*2) < waterLevelPer){
    if (toggleLow != 0){
        toggleLow = 0;
    }
  }

  //LED
  //full
  if (100-triggerPointPer < waterLevelPer){
      digitalWrite(fullLed, HIGH);
  }
  else {
      digitalWrite(fullLed, LOW);
  }
  //low
  if (0+triggerPointPer > waterLevelPer){
      digitalWrite(emptyLed, HIGH);
  }
  else {
      digitalWrite(emptyLed, LOW);
  }

  // Delay before repeating measurement

}

void init_sim_module(){
  // Test AT command
  Serial2.println("AT");
  update();
}

void update(){
  delay(500);
  while (Serial2.available()) {
    Serial.write(Serial2.read());
  }
}

void send_msg(String number, String msg){
  Serial.println("Sending message");
    // Test AT command
  //Serial2.println("AT");
  //update();
  // Set SMS to text mode
  Serial2.println("AT+CMGF=1"); 
  update();
  // Send SMS command
  Serial2.println("AT+CMGS=\"+91"+ number +"\""); // Replace with the recipient's phone number
  update();
  // Message content
  Serial2.print(msg);
  update();
  // End message with CTRL+Z
  Serial2.write(26); 
  update();
}

void make_call(String number){

  // Make a call
  Serial.println("Dialing...");
  //Serial2.println("AT");
  //update();
  Serial2.println("ATD+91"+number+";"); // Replace with the recipient's phone number
  update();
  // Optionally wait for some time and then hang up
  //delay(30000); // Wait for 30 seconds (or any desired duration)
  //Serial1.println("Hanging up...");
  //Serial2.println("ATH"); // Hang up the call
  //update();
}

void check_signal(){
  Serial.println("Checking signal strength...");
  Serial2.println("AT+CSQ");
  delay(500);
  String response = "";
  while (Serial2.available()) {
    response += (char)Serial2.read();
  }
  Serial.println(response);

  // Parse the signal strength from the response
  int index = response.indexOf("+CSQ: ");
  if (index != -1) {
    int commaIndex = response.indexOf(",", index);
    if (commaIndex != -1) {
      String signal = response.substring(index + 6, commaIndex);
      int signalStrength = signal.toInt();
      Serial.print("Signal strength: ");
      Serial.println(signalStrength);
      Blynk.virtualWrite(IOT_SIM_SIGNAL_VARIABLE, signalStrength);
    }
    
}

}
