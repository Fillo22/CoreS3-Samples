#include "M5CoreS3.h"
#define TINY_GSM_MODEM_SIM7600 // Use SIM7600 modem
#include <TinyGsmClient.h>
#include <PubSubClient.h>

// Cellular network information
const char apn[] = "TM"; // APN of your network provider
const char gprsUser[] = ""; // If required
const char gprsPass[] = ""; // If required

// MQTT Broker
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttUser = ""; // If required
const char* mqttPassword = ""; // If required

// SIM Card PIN (leave empty, if not defined)
const char simPIN[] = ""; 

TinyGsm modem(Serial2);
TinyGsmClient gsmClient(modem);
PubSubClient mqttClient(gsmClient);

void setup() {
  auto cfg = M5.config();
  CoreS3.begin(cfg);
  CoreS3.Display.setTextSize(2);
  Serial2.begin(115200, SERIAL_8N1, 18, 17); // Set serial port for modem communication

  // Start modem and connect to network
  setupModem();

  // Connect to MQTT Broker
  mqttClient.setServer(mqttServer, mqttPort);
  connectMQTT();
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  // Create a JSON message
  String message = "{\"temperature\":24,\"humidity\":60}";
  
  // Publish the message
  if(mqttClient.publish("sensordata/topic", message.c_str())) {
    CoreS3.Display.println("Messaggio inviato");
  } else {
    CoreS3.Display.println("Errore nell'invio del messaggio");
  }

  delay(1000); // Wait between messages
}

void setupModem() {
  // RESET the modem
  modem.restart();
  CoreS3.Display.println("Configurazione modem...");

  // Set SIM card PIN if necessary
  if (strlen(simPIN) && !modem.simUnlock(simPIN)) {
    CoreS3.Display.println("Impossibile sbloccare la SIM");
    return;
  }

  // Connect to the network
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    CoreS3.Display.println("Connessione GPRS fallita");
    return;
  }

  CoreS3.Display.println("Connesso alla rete cellulare!");
}

void connectMQTT() {
  // Connect to MQTT Broker
  CoreS3.Display.println("Connessione a MQTT...");

  while (!mqttClient.connected()) {
    if (mqttClient.connect("M5StackClient", mqttUser, mqttPassword)) {
      CoreS3.Display.println("Connesso a MQTT!");
    } else {
      CoreS3.Display.print("Connessione a MQTT fallita, rc=");
      CoreS3.Display.println(mqttClient.state());
      delay(5000);
    }
  }
}
