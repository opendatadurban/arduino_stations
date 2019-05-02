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

const char* ssid     = "Open Data Durban";
const char* password = "CivicTech";

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
  //Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
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

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["pm_25"] = data.pm25;
  root["pm_10"] = data.pm10;
  root["device_ID"] = device_ID;
  String output;
  root.printTo(output);
  Serial.println(output);

  // Send data to web app
  
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
   HTTPClient http;    //Declare object of class HTTPClient
   http.begin("http://citizen-sensors.herokuapp.com/data");      //Specify request destination
   http.addHeader("Content-Type", "application/json","Accept","application/json");
   int httpCode = http.POST(output);   //Send the request
 
   Serial.println(httpCode);   //Print HTTP return code 
   http.end();  //Close connection
 
 }else{
    Serial.println("Error in WiFi connection");   
 }
  Serial.println("Sleeping for 4 minutes..."); 
  delay(1000 * 4 * 60);
  // Turn sensor on
  //sensor.turnOn();  
}






