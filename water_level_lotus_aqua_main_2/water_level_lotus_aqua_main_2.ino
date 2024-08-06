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
 **********************************************************************************/
/* Fill-in your Template ID (only if using Blynk.Cloud) */


#define BLYNK_TEMPLATE_ID "TMPL3BfAm6wG1"
#define BLYNK_TEMPLATE_NAME "Water Level Indicator"
#define BLYNK_AUTH_TOKEN "Lo2Jzk_Iwm8SY3EqlYXRVOjWXrZYXQdB"

// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = "GokulHari012";
//char pass[] = "12345678";

//char ssid[] = "HF-R1104T-4G";
//char pass[] = "1234567890";

//char ssid[] = "Tenda_47BFC0";
//char pass[] = "system123";

char ssid[] = "LotusAquaIOT";
char pass[] = "12345678";

//first 1.6mpa sensore used. now changed to 0.2 mpa sensor
//old sensor range - 60 analog read
//new sensor range - 600 analog read
//Set Water Level pressure in CM
int emptyTankPressure = 500 ;  //pressure when tank is empty low 510
int fullTankPressure =  970 ;  //pressure when tank is full high 900

//Set trigger value in percentage
int triggerPointPer =   10 ;  //alarm will start when water level drop below triggerPoint **This is percentage tolarance** 


String msg_mobile_number_1 = "9944391393"; //Velu
String msg_mobile_number_2 = "9597307257"; //
String msg_mobile_number_3 = "9962601292"; //sampath 
String msg_mobile_number_4 = "9791898999"; //Gopi
String msg_mobile_number_5 = "9790169629"; //
String msg_mobile_number_test = "8220339908"; //

String call_mobile_number_1 = "9944391393";  //Velu
String call_mobile_number_2 = "9597307257";  //
String call_mobile_number_3 = "9962601292";  //sampath 
String call_mobile_number_4 = "9791898999";  //Gopi
String call_mobile_number_5 = "9790169629";  //
String call_mobile_number_test = "8220339908";  //

int call_delay = 20000;   //call will ring for 20 sec. start call and end call total timing. so around 15 sec the call will ring

boolean sim_test = true;

unsigned long previousMillis = 0;   // Variable to store the last time the function was run
const long interval = 10000;        // Interval in milliseconds (10 seconds)blynkCloudConnectLed

#define TINY_GSM_MODEM_SIM800

#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
//#include <BlynkSimpleEsp32.h>
#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>

//OTA 
#include <ESPmDNS.h>
#include <NetworkUdp.h>
#include <ArduinoOTA.h>

// Define connections to sensor
#define blynkCloudConnectLed 2   //D2 build in led blue color
#define BuzzerPin  32  //P32
#define fullLed  25 //P25
#define emptyLed  14  //P14
#define pressureSensor A0  //A0

float pressure = 0;
int pressure_sensor_reading_count = 0;
int   waterLevelPer;
int  toggleBuzzer = 0; //Define to remember the toggle state
int  toggleLow = 0; //Low water level toggle for sending msg

//***********Blynk***********
char auth[] = BLYNK_AUTH_TOKEN;
BlynkTimer timer;

//Change the virtual pins according the rooms
#define IOT_WATER_LEVEL_VARIABLE    V1 
#define IOT_SIM_SIGNAL_VARIABLE    V2 
#define IOT_PRESSURE_SENSOR_VARIABLE    V3 

// #define IOT_EMPTY_TANK_PRESSURE    V4
#define IOT_SIM_TEST_MODE    V4
// #define IOT_FULL_TANK_PRESSURE    V5 
#define IOT_PRESSURE_RANGE    V5 

//***********gsm Module***********
// Your GPRS credentials (leave empty if not needed)
char apn[]  = "airtelgprs.com";
char user[] = "";
char pass_sim[] = "";

TinyGsm modem(Serial2);


//***********web server***********
// Define the static IP address and subnet mask
IPAddress local_IP(192, 168, 1, 1);  // Static IP address for ESP32
IPAddress gateway(192, 168, 1, 1);   // Gateway (should be the same as local_IP in AP mode)
IPAddress subnet(255, 255, 255, 0);  // Subnet mask

// Create an instance of the web server on port 80
WebServer server(80);

void setup() {
  // Set up serial monitor
  Serial.begin(115200);

  // Initialize SIM800L on Serial2
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17

  // Give time to initialize the SIM800L
  delay(1000);
  init_sim_module();
  
  // Set pinmodes for sensor connections
  pinMode(blynkCloudConnectLed, OUTPUT);
  pinMode(fullLed, OUTPUT);
  pinMode(emptyLed, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);

  digitalWrite(blynkCloudConnectLed, LOW);
  digitalWrite(fullLed, LOW);
  digitalWrite(emptyLed, LOW);
  digitalWrite(BuzzerPin, LOW);

  // Set up the WiFi Access Point with static IP address
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, pass);

  // Print the IP address
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  delay(2000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();
  //modem.init();

  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  
  //Blynk.config(auth);
  Blynk.begin(auth, modem, apn, user, pass_sim);
  delay(1000);

  // Setup a function to check the GSM connection every 3*60 3 mins
  timer.setInterval(180000L, checkGSMConnection);

  // Initialize EEPROM with size of 512 bytes
  EEPROM.begin(512);

  // Set up the web server routes
  server.on("/", handleRoot);
  server.on("/setting", handleSetting);
  server.on("/submit", HTTP_POST, handleSubmit);
  server.begin();

  readEEPROMandSetToVariables();

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  //OTA
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });

  ArduinoOTA.begin();
}
 void loop() {
 
  measurePressure();

  Blynk.run();
  timer.run(); // Initiates SimpleTimer
  server.handleClient();
  ArduinoOTA.handle();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Save the last time the function was run
    previousMillis = currentMillis;

    check_signal();
    calculatePressurePercentage();
    off_buzzer();
    readSMS();
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
    digitalWrite(blynkCloudConnectLed, LOW);
  }
  if (isconnected == true) {
    digitalWrite(blynkCloudConnectLed, HIGH);
    //Serial.println("wifi true");
    //Serial.println("Blynk Connected");
  }
}

// Function to check the GSM connection and reconnect if needed
void checkGSMConnection() {
  if (!modem.isGprsConnected()) {
    Serial.println("GSM not connected. Reconnecting...");
    modem.gprsDisconnect();
    if (modem.gprsConnect(apn, user, pass_sim)) {
      Serial.println("GSM reconnected successfully.");
      Blynk.connect();
    } else {
      Serial.println("GSM reconnection failed.");
    } 
  } else {
    Serial.println("GSM connection is stable.");
  }
}

String sendATcommand(const char* command, const char* expected_response, unsigned long timeout) {
  Serial2.println(command);
  unsigned long start = millis();
  String response = "";
  while (millis() - start < timeout) {
    while (Serial2.available()) {
      char c = Serial2.read();
      response += c;
      if (response.indexOf(expected_response) != -1) {
        return response;
      }
    }
  }
  return response;
}

void readSMS() {
  String response;
  response = sendATcommand("AT+CMGF=1", "OK", 1000); // Set SMS mode to text
  response = sendATcommand("AT+CMGL=\"REC UNREAD\"", "OK", 5000); // List unread messages

  if (response.indexOf("+CMGL:") != -1) {
    Serial.println("SMS received:");
    Serial.println(response);

    // Parse the response to get the SMS content
    int msgIndex = response.indexOf("+CMGL: ");
    if (msgIndex != -1) {
      int msgStart = response.indexOf('\n', msgIndex) + 1;
      int msgEnd = response.indexOf('\n', msgStart);
      String message = response.substring(msgStart, msgEnd);
      
      Serial.print("Message: ");
      Serial.println(message);

      // Check if the message contains "restart"
      if (message.indexOf("restart_esp") != -1) {
        Serial.println("Restart command received. Restarting...");
        delay(1000); // Optional delay before restart
        ESP.restart();
      }
      if (message.indexOf("test_mode_on") != -1) {
        Serial.println("Test Mode on");
        sim_test = true;
        storeNumberInEEPROM(13, "true");
        Blynk.virtualWrite(IOT_SIM_TEST_MODE, 1);
      }
      if (message.indexOf("test_mode_off") != -1) {
        Serial.println("Test Mode off");
        sim_test = false;
        storeNumberInEEPROM(13, "false");
        Blynk.virtualWrite(IOT_SIM_TEST_MODE, 0);
      }
    }

    // Optionally, delete the SMS after reading
    int indexStart = response.indexOf(" ", msgIndex) + 1;
    int indexEnd = response.indexOf(",", indexStart);
    String index = response.substring(indexStart, indexEnd);
    sendATcommand(("AT+CMGD=" + index).c_str(), "OK", 1000);
  } else {
    Serial.println("No new SMS.");
  }
}




BLYNK_CONNECTED() {
  Blynk.syncVirtual(IOT_WATER_LEVEL_VARIABLE, IOT_SIM_SIGNAL_VARIABLE, IOT_PRESSURE_SENSOR_VARIABLE, IOT_SIM_TEST_MODE);
}

// Function to be called when data is received from Blynk
// BLYNK_WRITE(IOT_EMPTY_TANK_PRESSURE) {
//   emptyTankPressure = param.asInt(); // Use asInt(), asFloat(), or asStr() based on your data type
//   Serial.print("Received data from Blynk emptyTankPressure: ");
//   Serial.println(emptyTankPressure);
// }
// BLYNK_WRITE(IOT_FULL_TANK_PRESSURE) {
//   fullTankPressure = param.asInt(); // Use asInt(), asFloat(), or asStr() based on your data type
//   Serial.print("Received data from Blynk fullTankPressure: ");
//   Serial.println(fullTankPressure);
// }
BLYNK_WRITE(IOT_SIM_TEST_MODE) {
  int sim_mode = param.asInt(); // Use asInt(), asFloat(), or asStr() based on your data type
  if(sim_mode==1){
    sim_test = true;
    storeNumberInEEPROM(13, "true");
  }
  else{
    sim_test = false;
    storeNumberInEEPROM(13, "false");
  }
  Serial.print("Received data from Blynk SIM_MODE: ");
  Serial.println(sim_mode);
}

void measurePressure(){
  
  float local_pressure_reading = analogRead(pressureSensor);
  pressure = local_pressure_reading + pressure;
  pressure_sensor_reading_count++;

  //Blynk.virtualWrite(IOT_PRESSURE_SENSOR_VARIABLE, local_pressure_reading);
  
  // Print result to serial monitor
  //Serial.print("pressure: ");
  //Serial.print(pressure);
  //Serial.print(" --  ");
  Serial.print("local_pressure_reading: ");
  Serial.println(local_pressure_reading);
  // if(msg_mobile_number_1==""){
  //   Serial.print("null");
  // }
  // else{
  //   Serial.print(msg_mobile_number_1);
  // }
  delay(100);
}

void calculatePressurePercentage(){
  pressure = pressure/pressure_sensor_reading_count ;
  waterLevelPer = map(pressure ,emptyTankPressure, fullTankPressure, 0, 100);
  
  // Print result to serial monitor
  Serial.print("Average pressure: ");
  Serial.print(pressure);

  Serial.print("   ---   waterLevelPer: ");
  Serial.println(waterLevelPer);
  //delay(500);

  Blynk.virtualWrite(IOT_WATER_LEVEL_VARIABLE, waterLevelPer);
  delay(1000);
  Blynk.virtualWrite(IOT_PRESSURE_SENSOR_VARIABLE, pressure);
  delay(1000);
  Blynk.virtualWrite(IOT_PRESSURE_RANGE, "Low: "+String(emptyTankPressure)+" | High: "+String(fullTankPressure)+" | trigger: "+String(triggerPointPer));

  pressure = 0;
  pressure_sensor_reading_count = 0;

  //buzzer and send msg and make call
  if (100-triggerPointPer < waterLevelPer && waterLevelPer < 150 ){
      if (toggleBuzzer == 0){
        toggleBuzzer = 1;
        digitalWrite(BuzzerPin, HIGH);
        if(sim_test){
          send_msg(msg_mobile_number_test,"TanK Full - water level percentage: "+String(waterLevelPer));
          delay(1000);
          make_call(call_mobile_number_test);
        }
        else{
          if(msg_mobile_number_1!=""){
            send_msg(msg_mobile_number_1,"TanK Full - water level percentage: "+String(waterLevelPer));
          }
          if(msg_mobile_number_2!=""){
            send_msg(msg_mobile_number_2,"TanK Full - water level percentage: "+String(waterLevelPer));
          }
          if(msg_mobile_number_3!=""){
            send_msg(msg_mobile_number_3,"TanK Full - water level percentage: "+String(waterLevelPer));
          }
          if(msg_mobile_number_4!=""){
            send_msg(msg_mobile_number_4,"TanK Full - water level percentage: "+String(waterLevelPer));
          }
          if(msg_mobile_number_5!=""){
            send_msg(msg_mobile_number_5,"TanK Full - water level percentage: "+String(waterLevelPer));   
          }
          delay(2000);
          if(call_mobile_number_1!=""){
            make_call(call_mobile_number_1);
          }
          if(call_mobile_number_2!=""){
            make_call(call_mobile_number_2);
          }
          if(call_mobile_number_3!=""){
            make_call(call_mobile_number_3);
          }
          if(call_mobile_number_4!=""){
            make_call(call_mobile_number_4);
          }
          if(call_mobile_number_5!=""){
            make_call(call_mobile_number_5);
          }
        }
        
      }     
  }
  else if(100-(triggerPointPer*3) > waterLevelPer){
    if (toggleBuzzer != 0){
        toggleBuzzer = 0;
        digitalWrite(BuzzerPin, LOW);
    }
  }

  //Low message send
  if (0-triggerPointPer > waterLevelPer && waterLevelPer > -50 ){
      if (toggleLow == 0){
        toggleLow  = 1;
        if(sim_test){ 
          send_msg(msg_mobile_number_test,"Empty Tank  - water level percentage: "+String(waterLevelPer));
        }
        else{
          if(msg_mobile_number_1!=""){
            send_msg(msg_mobile_number_1,"Empty Tank - water level percentage: "+String(waterLevelPer));
          }
          if(msg_mobile_number_2!=""){
            send_msg(msg_mobile_number_2,"Empty Tank - water level percentage: "+String(waterLevelPer));
          }
          if(msg_mobile_number_3!=""){
            send_msg(msg_mobile_number_3,"Empty Tank - water level percentage: "+String(waterLevelPer));
          }
          if(msg_mobile_number_4!=""){
            send_msg(msg_mobile_number_4,"Empty Tank  - water level percentage: "+String(waterLevelPer));
          }
          if(msg_mobile_number_5!=""){
            send_msg(msg_mobile_number_5,"Empty Tank  - water level percentage: "+String(waterLevelPer));
          }
        }

      }      
  }
  else if(0-(triggerPointPer*3) < waterLevelPer){
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
  delay(5000);
  update();
}

// void send_msg_sim(String number, String message) {
//   modem.sendSMS(number, message);
// }

void make_call(String number){
  delay(500);
  // Make a call
  Serial.println("Dialing...");
  //Serial2.println("AT");
  //update();
  Serial2.println("ATD+91"+number+";"); // Replace with the recipient's phone number
  update();
  // Optionally wait for some time and then hang up
  delay(call_delay); // Wait for 15 seconds (or any desired duration)
  Serial.println("Hanging up...");
  Serial2.println("ATH"); // Hang up the call
  update();
}

// void make_call_sim(String number) {
//   modem.callNumber(number);
//   delay(10000); // Wait for 10 seconds
//   modem.sendAT(GF("ATH"));
//   //modem.hangupCall();
// }

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

//web server Code


// Function to handle the root URL
void handleRoot() {
  String html = "<html><body><h1>Lotus Aqua - water Level indicator IOT</h1><h2>Enter Mobile Numbers</h2><h3>For Message</h3>";
  html += "<form action='/submit' method='post'>";
  for (int i = 0; i < 5; i++) {
    String number = readNumberFromEEPROM(i);
    html += "Mobile Number " + String(i + 1) + ": <input type='number' name='number" + String(i) + "'"+"value='"+number+"'><br>";
  }
  html+="<h3>For Call</h3>";
  for (int i = 5; i < 10; i++) {
    String number = readNumberFromEEPROM(i);
    html += "Mobile Number " + String(i + 1 - 5) + ": <input type='number' name='number" + String(i) + "'"+"value='"+number+"'><br>";
  }
  html += "<br><h2><input type='submit' name='submit' value='Submit'></form></h2>";
  html+="<a href='/setting'><button>Setting</button></a>";
  html += "</body></html>";
  html+=returnCss();
  server.send(200, "text/html", html);
}

// Function to handle form submission
void handleSubmit() {
  if(server.arg("submit")=="Submit"){
    for (int i = 0; i < 10; i++) {
      String number = server.arg("number" + String(i));
      storeNumberInEEPROM(i, number);
    }
    server.send(200, "text/html", "<br><br><h2>Numbers saved. <a style='margin-left:10px' href='/'>Go back</a></h2>"+returnCss());
  }
  else{
      String lowLevelReading = server.arg("lowLevelReading");
      String highLevelReading = server.arg("highLevelReading");
      String triggerPointPercentage = server.arg("triggerPointPercentage");

      storeNumberInEEPROM(10, lowLevelReading);
      storeNumberInEEPROM(11, highLevelReading);
      storeNumberInEEPROM(12, triggerPointPercentage);

      if (server.hasArg("toggleBox")) {
        sim_test=true;
        storeNumberInEEPROM(13, "true");
        Blynk.virtualWrite(IOT_SIM_TEST_MODE, 1);
      } else {
        sim_test=false;
        storeNumberInEEPROM(13, "false");
        Blynk.virtualWrite(IOT_SIM_TEST_MODE, 0);
      }

      server.send(200, "text/html", "<br><br><h2>Settings saved. <br> <a style='margin-left:10px' href='/setting'>Go back</a> <br> <a style='margin-left:10px' href='/'>Go Home</a></h2>"+returnCss());
  }
  readEEPROMandSetToVariables();
}
void handleSetting(){
  String html = "<html><body><h1>Lotus Aqua - water Level indicator IOT</h1><h2>Setting</h2>";
  html += "<form action='/submit' method='post'>";
  String lowLevelReading = readNumberFromEEPROM(10);
  String highLevelReading = readNumberFromEEPROM(11);
  String triggerPointPercentage = readNumberFromEEPROM(12);
  String sim_test_temp = readNumberFromEEPROM(13);
  html += "Low Level Reading : <input type='number' name='lowLevelReading' value='"+lowLevelReading+"'><br>";
  html += "High Level Reading : <input type='number' name='highLevelReading' value='"+highLevelReading+"'><br>";
  html += "Trigger Point Percentage : <input type='number' name='triggerPointPercentage' value='"+triggerPointPercentage+"'><br>";
  if(sim_test_temp=="true"){
    html += "Sim Test Mode: <label class='switch'><input type='checkbox' name='toggleBox' value='on' checked><span class='slider round'></span></label><br>";
  }
  else{
    html += "Sim Test Mode: <label class='switch'><input type='checkbox' name='toggleBox' value='on'><span class='slider round'></span></label><br>";
  }
  html += "<br><h2><input type='submit' name='submit' value='Submit Setting'></form></h2>";
  html+="<a href='/'><button>Back</button></a>";
  html += "</body></html>";
  html+=returnCss();
  html+=toggleCss();
  server.send(200, "text/html", html);
}

// Function to store a number in EEPROM
void storeNumberInEEPROM(int index, String number) {
  int startAddress = index * 20; // Reserve 20 bytes for each number
  for (int i = 0; i < 20; i++) {
    if (i < number.length()) {
      EEPROM.write(startAddress + i, number[i]);
    } else {
      EEPROM.write(startAddress + i, 0); // Null character for unused bytes
    }
  }
  EEPROM.commit();
}

// Function to read a number from EEPROM
String readNumberFromEEPROM(int index) {
  int startAddress = index * 20;
  String number = "";
  for (int i = 0; i < 20; i++) {
    char c = EEPROM.read(startAddress + i);
    if (c != 0) {
      number += c;
    }
  }
  return number;
}

String returnCss(){
  return "<style>body{background-color:#d1d8e0;font-family: \"Times New Roman\",Times,serif;}input[type=number]{width: 30%;height:40px;padding:12px20px;margin: 8px 10px;box-sizing: border-box;border:1pxsolidblack;border-radius:8px;}input[type=submit],button{width:20%;height:40px;padding:12px20px;margin:20px100px;box-sizing:border-box;border:1pxsolidblack;border-radius:10px;background-color:#3CBC8D;color:white;}h1{color: #3867d6;}button{background-color: #fc5c65;}</style>";
}
String toggleCss(){
  return "<style>.switch{position:relative;display:inline-block;width:60px;height:34px;margin-left:20px;}.switch input{opacity:0;width:0;height:0;}.slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#ccc;-webkit-transition:.4s;transition:.4s;}.slider:before{position:absolute;content:\"\";height:26px;width:26px;left:4px;bottom:4px;background-color:white;-webkit-transition:.4s;transition:.4s;}input:checked + .slider{background-color:#2196F3;}input:focus + .slider{box-shadow:001px#2196F3;}input:checked + .slider:before{-webkit-transform:translateX(26px);-ms-transform:translateX(26px);transform:translateX(26px);}.slider.round{border-radius:34px;}.slider.round:before{border-radius:50%;}</style>";
}

void readEEPROMandSetToVariables(){

  msg_mobile_number_1 = readNumberFromEEPROM(0); //Velu
  msg_mobile_number_2 = readNumberFromEEPROM(1); //
  msg_mobile_number_3 = readNumberFromEEPROM(2); //sampath 
  msg_mobile_number_4 = readNumberFromEEPROM(3); //Gopi
  msg_mobile_number_5 = readNumberFromEEPROM(4); //

  call_mobile_number_1 = readNumberFromEEPROM(5);  //Velu
  call_mobile_number_2 = readNumberFromEEPROM(6);  //
  call_mobile_number_3 = readNumberFromEEPROM(7);  //sampath 
  call_mobile_number_4 = readNumberFromEEPROM(8);  //Gopi
  call_mobile_number_5 = readNumberFromEEPROM(9);  //

  emptyTankPressure = readNumberFromEEPROM(10).toInt();  
  fullTankPressure =  readNumberFromEEPROM(11).toInt();  
  triggerPointPer =   readNumberFromEEPROM(12).toInt(); 
  if(readNumberFromEEPROM(13)=="true"){
    sim_test = true;
  }
  else{
    sim_test = false;
  }
}
