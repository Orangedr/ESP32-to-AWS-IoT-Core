#include <Arduino.h>
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <ezButton.h>

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
#define Machine_name "C1"

// The pins that is used, please refer to ESP32 GPIO pins descriptions for details
#define Pin1 32
#define Pin2 35
#define Pin3 33


WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

ezButton CounterSensor(Pin1);
ezButton RPMSensor(Pin3);

// the time variables for upload duration
volatile unsigned long lastuploadTime = 0; // Records last count time for debounce

// the time variables for calc RPM
volatile unsigned long RPMTime1 = 1; // Calc RPM=60000/(t1-t2)
volatile unsigned long RPMTime2 = 0; 
double RPM1;


void connectAWS()
{
// connect wifi use WIFI_SSID_1 and WIFI_PASSWORD_1, if not connect use WIFI_SSID_2 and WIFI_PASSWORD_2 
 
  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) {
    // WiFi.begin(WIFI_SSID_1, WIFI_PASSWORD_1);
    // delay(1000);
  
  WiFi.begin(WIFI_SSID_1, WIFI_PASSWORD_1);

  Serial.println("Connecting to WiFi-1");
    delay(2500);

  if (WiFi.status() != WL_CONNECTED){
    Serial.print("Connecting to WIFI_2");
    
    WiFi.begin(WIFI_SSID_2, WIFI_PASSWORD_2);
    delay(2500);
   }
  }
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  // client.onMessage(messageHandler);

  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage()
// THe core of the script, assign values to "doc" and publish to AWS
{
  StaticJsonDocument<200> doc;


   /* Assigning values to the JSON document. */
    doc["Run_time"] = millis();
    // doc["sensor_33"] = analogRead(Pin1);
    // doc["sensor_23"] = analogRead(12);
    doc["Machine_name"] = Machine_name;
    doc["productionCount"] = CounterSensor.getCount();
    doc["analog"]=analogRead(Pin2);

    // calculate RPM
    doc["RPM1"] = RPM1;
    RPM1=0;




  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

// void messageHandler(String &topic, String &payload) {
//   Serial.println("incoming: " + topic + " - " + payload);

// //  StaticJsonDocument<200> doc;
// //  deserializeJson(doc, payload);
// //  const char* message = doc["message"];
// }

void setup() {
  Serial.begin(9600);
  Serial.println("1");
  connectAWS();
  pinMode(Pin1, INPUT_PULLUP);
  pinMode(Pin3, INPUT_PULLUP);

  /* Setting the debounce time to 20ms. */
  CounterSensor.setDebounceTime(20);
  CounterSensor.setCountMode(COUNT_FALLING);


  RPMSensor.setDebounceTime(20);
  // RPMSensor.setCountMode(COUNT_FALLING);
}

void loop() {
  CounterSensor.loop();
  RPMSensor.loop();

  // Serial.print("button 2 state: ");
  // Serial.println(RPMSensor.getState());


  /* Calculating the RPM. */
  if (RPMSensor.isPressed()) {

// Calculate RPM, used in publishMessage()
    RPMTime2=RPMTime1;
    RPMTime1 = millis();
    RPM1=60000/(RPMTime1-RPMTime2);

  // Serial.println(RPMTime1);
  // Serial.println(RPMTime2);

  }

  /* A function that is required to be called in the loop() function. It is used to check for incoming
  messages and to keep the connection to AWS IoT alive. */
  client.loop();

/* A timer that will publish the message every 2 seconds. */
if (millis()-lastuploadTime > 2000) {
    publishMessage();
    lastuploadTime = millis();
  }

}
