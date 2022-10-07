
## Purpose
The purpose of the project is to utilize ESP32 or other Arduino series board to gather production data in the machine and bring the production site to digital level, help management to make decision and reduce cost due to down time and material waste
With the available data online, we want the sensors to monitor machine status, part life span, to assist preventive maintenance.
By doing this, we will be able to acquire real-time, clean, digital data, instead of hand-written data that needs to be input to the spreadsheet environment.

## Prequisites:
* Language: C++ (arduino)
* Device: ESP32 (wrover-dev), Sensors (NPN Photoelectric, digital)
* Software: Arduino IDE, Visual Studio Code, PlatformIO
* IoT host: AWS Account
* Hardware: Breadboard, Jumper Wires, USB Cable, Power Supply
* Libraries: WiFiClientSecure, MQTTClient, ArduinoJson, ezButton

## Function:

This is a derivative from the AWS example code for ESP32, the code is modified to fit the project requirement. The original code section is in the reference section.

**This code does a few things:** 
1. Gather Analog signal counts from sensor at Pin 33, every object passing by the sensor will generate a pulse, the pulse will be counted and sent to AWS IoT Core at given topic
2. Calculate RMP (Revolutions Per Minute) based on the pulse counts on Pin 32, the RMP will be sent to AWS IoT Core
3. Gather digital reading on Pin 35, on which pin you can connect a digital sensor that display critical reading that you want to monitor
4. The sensor data would converge to the function publishMessage(), where you can add or take away the data you want to send to AWS IoT Core, but please refer to ESP32 GPIO instruction to make sure you are using the correct pin number, you can't use ADC2 pins because of WIFI module.
5. There are 2 pair of WiFi connection, if the first one failed it will try to connect the second one. 
   
## Caution:
* The Pin 33 and 32 are at Pull up mode and connect to a NPN sensor in this project, change the input mode as you need.
* The Access for AWS and WiFi are in the secret.h, change the settings as you need.

## Reference:
* [Building an AWS IoT Core device using AWS Serverless and an ESP32](https://aws.amazon.com/blogs/compute/building-an-aws-iot-core-device-using-aws-serverless-and-an-esp32) 
* [Build Your First IoT App](https://aws.amazon.com/developer/learning/lets-build-iot/)
