#include <FirebaseESP32.h>
#include <WiFi.h>

// PORTAS
int valve1Output = 0;
int valve2Output = 1;
int sensor1Input = 2;
int sensor2Input = 3;

// CONTADORES DE PULSOS
int sensor1PulseCounter = 0;
int sensor2PulseCounter = 0;

// VARIAVEIS DE TEMPO
int currentTime = 0;
int lastTime = 0;
int sampleTime = 1000;

// CONEXAO WIFI
const char *ssid = "NOME DA REDE";
const char *password = "SENHA DA REDE";

// CONFIGURACAO FIREBASE
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

void IRAM_ATTR sensor1Pulse() { sensor1PulseCounter++; }

void IRAM_ATTR sensor2Pulse() { sensor2PulseCounter++; }

float getVolume(int pulses) { return 0.00225 * pulses; }

float getFlowRate(int pulses, int sampleTime) {
  float volume = getVolume(pulses);
  return (volume / (sampleTime / 1000));
}

void setup() {
  pinMode(valve1Output, OUTPUT);
  pinMode(valve2Output, OUTPUT);
  pinMode(sensor1Input, INPUT);
  pinMode(sensor2Input, INPUT);

  attachInterrupt(sensor1Input, pulse, RISING);
  attachInterrupt(sensor2Input, pulse, RISING);

  currentTime = millis();
  lastTime = currentTime;

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  // INICIALIZACAO DO FIREBASE

  config.host = FIREBASE_HOST;
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  currentTime = millis();
  if (currentTime >= (lastTime + sampleTime)) {
    lastTime = currentTime;
  }

  if (Firebase.ready() && signupOK && (currentTime >= (lastTime + sampleTime)){
    sendDataPrevMillis = millis();

    if (Firebase.RTDB.setInt(&fbdo, "test/int", count)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;

    if (Firebase.RTDB.setFloat(&fbdo, "test/float", 0.01 + random(0, 100))) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}