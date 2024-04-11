#include "M5CoreS3.h"
#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "M5UnitENV.h"
#include <ArduinoJson.h>

const char apn[] = "TM";  // Sostituisci con il tuo APN
const char gprsUser[] = "";  // Sostituisci con il tuo user, se necessario
const char gprsPass[] = "";  // Sostituisci con la tua password, se necessario
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttUser = "";  // Se necessario
const char* mqttPassword = "";  // Se necessario
const char simPIN[] = "";  // PIN per la SIM card

TinyGsm modem(Serial2);
TinyGsmClient gsmClient(modem);
PubSubClient mqttClient(gsmClient);
SCD4X scd4x;

void setup() {
    auto cfg = M5.config();
    CoreS3.begin(cfg);
    CoreS3.Display.setTextSize(2);
    Serial2.begin(115200, SERIAL_8N1, 18, 17);

    setupModem();
    mqttClient.setServer(mqttServer, mqttPort);
    connectMQTT();
    setupCO2Sensor();
}

void loop() {
    if (!mqttClient.connected()) {
        connectMQTT();
    }
    mqttClient.loop();

    CoreS3.Display.println("Analyzing environment...");
    String message = analyzeEnvironment();

    if (!message.isEmpty() && mqttClient.publish("sensordata/topic", message.c_str())) {
        CoreS3.Display.println("Message sent");
    } else {
        CoreS3.Display.println("Error sending message");
    }

    delay(5000);
}

void setupCO2Sensor() {
    if (!scd4x.begin(&Wire, SCD4X_I2C_ADDR, 9, 8, 400000U)) {
        CoreS3.Display.println("Could not find SCD4X");
        while (1);
    }

    if (!scd4x.startPeriodicMeasurement()) {
        CoreS3.Display.println("Failed to start measurement");
        while (1);
    }
    CoreS3.Display.println("Waiting for first measurement... (5 sec)");
}

void setupModem() {
    modem.restart();
    CoreS3.Display.println("Configuring modem...");

    if (strlen(simPIN) && !modem.simUnlock(simPIN)) {
        CoreS3.Display.println("Failed to unlock SIM");
        while (1);
    }

    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        CoreS3.Display.println("GPRS connection failed");
        while (1);
    }

    CoreS3.Display.println("Connected to cellular network!");
}

void connectMQTT() {
    CoreS3.Display.println("Connecting to MQTT...");
    while (!mqttClient.connected()) {
        if (mqttClient.connect("M5StackClient", mqttUser, mqttPassword)) {
            CoreS3.Display.println("Connected to MQTT!");
        } else {
            CoreS3.Display.print("MQTT connection failed, rc=");
            CoreS3.Display.println(mqttClient.state());
            delay(5000);
        }
    }
}

String analyzeEnvironment() {
    if (scd4x.update()) {
        float temperature = scd4x.getTemperature();
        float humidity = scd4x.getHumidity();
        int co2 = scd4x.getCO2();

        CoreS3.Display.printf("CO2: %d ppm\n", co2);
        CoreS3.Display.printf("Temperature: %.2f C\n", temperature);
        CoreS3.Display.printf("Humidity: %.2f %%\n", humidity);

        DynamicJsonDocument doc(256);
        doc["temperature"] = temperature;
        doc["humidity"] = humidity;
        doc["co2"] = co2;
        String output;
        serializeJson(doc, output);
        return output;
    } else {
        CoreS3.Display.println("Waiting for new data...");
        return "";
    }
}
