#include "settings.h"

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high
void dmpDataReady()
{
  mpuInterrupt = true;
  digitalWrite(LED_PIN, 1);
}

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

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
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);

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

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop()
{
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print(ax);
  Serial.print(", ");
  Serial.print(ay);
  Serial.print(", ");
  Serial.print(az);
  Serial.println("");
  if (mpuInterrupt)
  {
    mpuInterrupt = false;
    
    if (mpu.getIntMotionStatus())
    {
      Serial.println("Bummmm");
    }
  }
  else
  {
    digitalWrite(LED_PIN, 0);
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