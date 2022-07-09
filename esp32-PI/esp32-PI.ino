#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert Firebase project API Key
#define API_KEY ""

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "" 

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

// PORTAS
int valve1Output = 33;
int valve2Output = 4;
int sensor1Input = 32;
int sensor2Input = 35;

// CONTADORES DE PULSOS
int sensor1PulseCounter = 0;
int sensor2PulseCounter = 0;

// ESTADOS DAS VÁLVULAS
int valve1State = LOW;
int valve2State = LOW;

// VOLUMES E VAZÕES
float sensor1Volume = 0;
float sensor2Volume = 0;
float sensor1FlowRate = 0;
float sensor2FlowRate = 0;

// VARIAVEIS DE TEMPO
int currentTime = 0;
int lastTime = 0;
int sampleTime = 5000;

// CONEXAO WIFI
const char *WIFI_SSID = "";
const char *WIFI_PASSWORD = "";

int counter = 0;

void IRAM_ATTR sensor1Pulse() { sensor1PulseCounter++; }

void IRAM_ATTR sensor2Pulse() { sensor2PulseCounter++; }

float getVolume(int pulses) { return 0.00225 * pulses; }

float getFlowRate(int pulses, int sampleTime) {
  float volume = getVolume(pulses);
  return (volume / (sampleTime / 1000));
}


void setup() {
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin("Lauro", "10203040");
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());

    pinMode(valve1Output, OUTPUT);
    pinMode(valve2Output, OUTPUT);
    pinMode(sensor1Input, INPUT);
    pinMode(sensor2Input, INPUT);
    pinMode(2, OUTPUT);

    attachInterrupt(sensor1Input, sensor1Pulse, RISING);
    attachInterrupt(sensor2Input, sensor2Pulse, RISING);

    currentTime = millis();
    lastTime = currentTime;

    config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

}

void loop() {
  currentTime = millis();
  
  
  
  if (Firebase.ready() && (currentTime >= (lastTime + sampleTime))){
    lastTime = currentTime;

    sensor1Volume = getVolume(sensor1PulseCounter);
    sensor2Volume = getVolume(sensor2PulseCounter);
    sensor1FlowRate = getFlowRate(sensor1PulseCounter, sampleTime);
    sensor2FlowRate = getFlowRate(sensor2PulseCounter, sampleTime);

    
    if (Firebase.RTDB.pushFloat(&fbdo, "/sensores/sensor1/volume", sensor1Volume)) {
      Serial.println("Volume 1 pushed.");
    }
    else{
      ESP.restart();
    }
    if (Firebase.RTDB.pushFloat(&fbdo, "/sensores/sensor2/volume", sensor2Volume)) {
      Serial.println("Volume 2 pushed.");
    }
    else{
      ESP.restart();
    }
    if (Firebase.RTDB.pushFloat(&fbdo, "/sensores/sensor1/flowRate", sensor1FlowRate)) {
      Serial.println("Flow Rate 1 pushed");
    }
    else{
      ESP.restart();
    }
    if (Firebase.RTDB.pushFloat(&fbdo, "/sensores/sensor2/flowRate", sensor2FlowRate)) {
      Serial.println("Flow Rate 2 pushed");
    }
    else{
      ESP.restart();
    }
    if (Firebase.RTDB.getBool(&fbdo, "valve1")) {
      Serial.println("Valve 1 state");
      fbdo.boolData() == true ? valve1State = HIGH: valve1State = LOW;
      
    }
    else{
      ESP.restart();
    }
    if (Firebase.RTDB.getBool(&fbdo, "valve2")) {
      Serial.println("Valve 2 state");
      fbdo.boolData() == true ? valve2State = HIGH: valve2State = LOW;
    }
    else{
      ESP.restart();
    }
    
    counter++;
    sensor1PulseCounter = 0;
    sensor2PulseCounter = 0;
    
  }
}