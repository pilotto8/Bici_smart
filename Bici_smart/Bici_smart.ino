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
  pinMode(LED_PIN, OUTPUT);
  pinMode(3, INPUT_PULLUP);
  MPUsetUp();
  mpu.setDMPEnabled(true);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), hey, RISING);
  //set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  MPUsetInt();
  mpu.setIntMotionEnabled(1);

  /*Serial.println("Motion detection:");
    Serial.println(mpu.getMotionDetectionThreshold());
    Serial.println(mpu.getMotionDetectionDuration());
    Serial.println(mpu.getMotionDetectionCounterDecrement());
    Serial.println(mpu.getIntMotionEnabled());
    Serial.println(mpu.getIntEnabled(), BIN);
    Serial.println(mpu.getDMPEnabled());*/
  //Serial.println(mpu.getIntDMPEnabled());
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
        set_sleep_mode(SLEEP_MODE_IDLE);
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
      if (MPUgetNoise() < NOISE_TRESHOLD && millis() - millisCheckpoint > 1000){ 
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
        Serial.println("Mooving!");
      }
      MPUgetNoise();
      break;
    }
  }

  if (!digitalRead(3))
  {
    phase = sleeping;
    delay(1000  );
  }
}