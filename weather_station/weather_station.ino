/* switch dust sensor on and off 
 * take average readings 
 * using hackair code
 * connect to wifi
 * package data as json
 * send data to python to be recorded
 * post data to website
*/

// Libraries
#include <hackair.h>;
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Define device identification 
const char* device_ID = "ad101";

// Define Wifi requirements

//const char* ssid_ODD     = "HUAWEI P10 lite";
//const char* password_ODD = "1d1867be0513";

const char* ssid     = "ScwifityB_EXT";
const char* password = "0741893105";

const char* ssid_ODD     = "Open Data Durban";
const char* password_ODD = "CivicTech";

// Define sensor
hackAIR sensor(SENSOR_SDS011);

// Structure for data storage
struct hackAirData data;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  // Initialize the sensor
  sensor.enablePowerControl();
  sensor.turnOn();
  sensor.begin();
  sensor.clearData(data); 
  Serial.begin(115200);
  delay(100);
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.println("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  for (int i = 1; i<= 200; i++){
    if (WiFi.status() != WL_CONNECTED){
      Serial.print("Attempt ");
      Serial.println(i);
      delay(500);
    }
    else {
      break;
    }
  }

  if (WiFi.status() != WL_CONNECTED){
    Serial.println("Failed to connect to ");
    Serial.println(ssid);
    Serial.println("Connecting to ODD");
    WiFi.begin(ssid_ODD, password_ODD);
    for (int i = 1; i<= 60; i++){
      if (WiFi.status() != WL_CONNECTED){
        Serial.print("Attempt ");
        Serial.println(i);
        delay(500);
      }
      else {
        break;
      }
    }
  }
  
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  
  Serial.println("This is unit ");
  Serial.println(device_ID);
  // Turn sensor on
  //sensor.turnOn();
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on while sensor is on
  // Wait for sensor to settle
  Serial.println("Waiting for sensor to settle... ");
  delay(1000 * 3);
  Serial.println("Taking measurements... ");
  // Make 60 measurements and return the mean values
  sensor.readAverageData(data, 60);
  
  // Turn off the sensor
  //sensor.turnOff();
  digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off while sensor is off
  delay(1000 * 3);

  Serial.println("Posting data... ");

  StaticJsonDocument<200> root;
  root["pm_25"] = data.pm25;
  root["pm_10"] = data.pm10;
  root["device_ID"] = device_ID;
  String output;
  serializeJson(root, output);
  Serial.println(output);

  // Send data to web app
  
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
   HTTPClient http;    //Declare object of class HTTPClient
   http.begin("http://citizen-sensors.herokuapp.com/data");      //Specify request destination
   http.addHeader("Content-Type", "application/json","Accept","application/json");
   int httpCode = http.POST(output);   //Send the request
   String payload = http.getString();
   //Serial.println(httpCode);   //Print HTTP return code
   //Serial.println(payload);    //Print request response 
   if(httpCode==200){
    Serial.println("successfully sent data to website!");
   }else{
    Serial.println("failed to send data to website!");
    Serial.println("HTTP return code:");
    Serial.println(httpCode);
    Serial.println("Request response:");
    Serial.println(payload);
   }
   http.end();  //Close connection
 
 }else{
    Serial.println("Error in WiFi connection");   
 }
  Serial.println("Sleeping for 4 minutes...");
  delay(1000 * 4 * 60);
  // Turn sensor on
  //sensor.turnOn();  
}
