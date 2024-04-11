#include "M5CoreS3.h"
#define TINY_GSM_MODEM_SIM7600
// Include le librerie necessarie per la comunicazione cellulare e MQTT
#include <TinyGsmClient.h>
#include <PubSubClient.h>

// Dati per la connessione cellulare
const char apn[] = "TM"; // Sostituisci con il tuo APN
const char gprsUser[] = ""; // Sostituisci con il tuo user, se necessario
const char gprsPass[] = ""; // Sostituisci con la tua password, se necessario

// Dati server MQTT
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttUser = ""; // Se necessario
const char* mqttPassword = ""; // Se necessario

// PIN per la SIM card
const char simPIN[] = ""; 

// Inizializza l'oggetto modem e client
TinyGsm modem(Serial2);
TinyGsmClient gsmClient(modem);
PubSubClient mqttClient(gsmClient);

void setup() {
  auto cfg = M5.config();
  CoreS3.begin(cfg);
  CoreS3.Display.setTextSize(2);
  Serial2.begin(115200, SERIAL_8N1, 18, 17); // Configura la seriale per il modulo SIM7600G

  // Avvia il modem e stabilisci la connessione GPRS
  setupModem();

  // Connessione al server MQTT
  mqttClient.setServer(mqttServer, mqttPort);
  connectMQTT();
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  // Costruisci il tuo messaggio JSON qui
  String message = "{\"temperature\":24,\"humidity\":60}";
  
  // Pubblica il messaggio JSON
  if(mqttClient.publish("sensordata/topic", message.c_str())) {
    CoreS3.Display.println("Messaggio inviato");
  } else {
    CoreS3.Display.println("Errore nell'invio del messaggio");
  }

  delay(1000); // Attendere un secondo tra i messaggi
}

void setupModem() {
  // Resetta e inizializza il modem
  modem.restart();
  CoreS3.Display.println("Configurazione modem...");

  // Imposta il PIN della SIM, se necessario
  if (strlen(simPIN) && !modem.simUnlock(simPIN)) {
    CoreS3.Display.println("Impossibile sbloccare la SIM");
    return;
  }

  // Connetti alla rete cellulare
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    CoreS3.Display.println("Connessione GPRS fallita");
    return;
  }

  CoreS3.Display.println("Connesso alla rete cellulare!");
}

void connectMQTT() {
  // Connettiti al server MQTT
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
