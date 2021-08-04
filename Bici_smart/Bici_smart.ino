#include "settings.h"

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high
void hey()
{
  if (mpu.getIntMotionStatus()){
    phase = checking;
  }
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
  LowPower.attachInterruptWakeup(digitalPinToInterrupt(INTERRUPT_PIN), hey, RISING);

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
        phase = loaded_phase;
        MPUsetUp();
        MPUsetInt();
        mpu.setIntMotionEnabled(1);
      }
      LowPower.deepSleep();
      break;
    }
    case checking:{
      
      if (phase != loaded_phase){
        phase = loaded_phase;
        mpu.setIntMotionEnabled(0);
      }
      MPUgetNoise();
      break;
    }
    case mooving:{
      if (phase != loaded_phase){
        phase = loaded_phase;
      }
      if (millis() - millisCounter > 10){
        millisCounter = millis() / 10 * 10;
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        Serial.print(ax);
        Serial.print(", ");
        Serial.print(ay);
        Serial.print(", ");
        Serial.print(az);
        Serial.println("");
      }
      break;
    }
  }

  if (!digitalRead(3))
  {
    mpu.setIntMotionEnabled(0);
    delay(1000);
    MPUsetUp();
    MPUsetInt();
    mpu.setIntMotionEnabled(1);
  }
}