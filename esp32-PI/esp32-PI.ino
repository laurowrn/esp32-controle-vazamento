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

void IRAM_ATTR sensor1Pulse(){
   sensor1PulseCounter++;
}

void IRAM_ATTR sensor2Pulse(){
   sensor2PulseCounter++;
}

float getVolume(int pulses){
  return 0.00225*pulses;
}

float getFlowRate(int pulses, int sampleTime){
  float volume = getVolume(pulses);
  return(volume/(sampleTime/1000));
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

}

void loop() {
   currentTime = millis();
   if(currentTime >= (lastTime + sampleTime)){
      lastTime = currentTime; 
      
   }

}
