# COM.LTE Cellular Module

This Arduino project utilizes the M5CoreS3 platform and SIM7600 cellular modem to publish sensor data (temperature and humidity) to an MQTT broker. It's designed for IoT applications requiring remote data collection and monitoring.

## Hardware Required
- M5Stack Core S3
- SIM7600 GSM Module
- Appropriate SIM card with a data plan

## Software Dependencies
- Arduino IDE
- TinyGsm library (`TinyGsmClient.h`)
- PubSub MQTT client (`PubSubClient.h`)
- M5CoreS3 library

## Setup
1. Install the Arduino IDE and ensure your M5CoreS3 board is supported.
2. Include the TinyGsm and PubSubClient libraries in your sketch. These can be installed via the Arduino Library Manager.
3. Connect your SIM7600 module to the M5CoreS3 as specified in the hardware documentation, usually via serial connection.

## Configuration
Fill in your cellular network and MQTT broker details in the following variables in the sketch:
- `apn`, `gprsUser`, `gprsPass`: Your network's APN and optional GPRS username and password.
- `mqttServer`, `mqttPort`, `mqttUser`, `mqttPassword`: MQTT broker information.
- `simPIN`: SIM card PIN number, if applicable.

## Usage
Upload the code to your M5CoreS3 board. The device will automatically:
- Initialize the display and modem.
- Connect to the cellular network.
- Connect to the MQTT broker.
- Publish temperature and humidity data to the `sensordata/topic` topic every second.

## MQTT Broker
The default configuration uses `broker.hivemq.com` for MQTT communications. You can change this to any other broker by modifying the `mqttServer` and related settings.