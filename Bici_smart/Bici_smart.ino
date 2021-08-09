#include "settings.h"

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high
void hey()
{
  phase = checking;
}


void setup()
{
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  Serial.begin(115200);
  pinMode(INTERRUPT_PIN, INPUT);
  pinMode(FRONT_LED_PIN, OUTPUT);
  pinMode(REAR_LED_PIN, OUTPUT);
  //pinMode(13, OUTPUT);
  pinMode(B0, INPUT_PULLUP);
  pinMode(B1, INPUT_PULLUP);
  MPUsetUp();
  mpu.setDMPEnabled(true);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), hey, RISING);
  //set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  MPUsetInt();
  //mpu.setIntMotionEnabled(1);

  for (int i = 0; i < 2; i++){
    LED_mode[i] = EEPROM.read(i);
    Serial.println(LED_mode[i]);
  }
}

void loop()
{
  switch (phase){
    case sleeping:{
      if (phase != loaded_phase){
        loaded_phase = phase;
        MPUsetUp();
        MPUsetInt();
        mpu.setIntMotionEnabled(1);
        Serial.println("Going to sleep...");
      }
      if (!changing){
        analogWrite(FRONT_LED_PIN, 0);
        analogWrite(REAR_LED_PIN, 0);
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_enable(); 
        sleep_mode();
        sleep_disable();
      }
      break;
    }
    case checking:{
      if (phase != loaded_phase){
        loaded_phase = phase;
        mpu.setIntMotionEnabled(0);
        millisCheckpoint = millis();
        Serial.println("Checking?");
      }
      if (MPUgetNoise() < NOISE_TRESHOLD && millis() - millisCheckpoint > NOISE_LENGTH * NOISE_SAMPLING_DELAY){ 
        phase = sleeping;
      }
      else if (millis() - millisCheckpoint > 2000){
        phase = mooving;
      }
      break;
    }
    case mooving:{
      if (phase != loaded_phase){
        loaded_phase = phase;
        setLEDstate(1);
        millisCheckpoint = millis();
        Serial.println("Mooving!");
      }
      if (MPUgetNoise() < NOISE_TRESHOLD){
        if (millis() - millisCheckpoint > 200000){
          setLEDstate(0);
          phase = sleeping;
        }
      }
      else {
        millisCheckpoint = millis();
      }
      break;
    }
  }

  if (millis() - millisCounter > 10 || change){
      millisCounter = millis() / 10 * 10;
      LEDhandle();
  }
  buttons();
}