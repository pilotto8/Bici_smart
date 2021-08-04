#include "settings.h"

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high
void dmpDataReady()
{
  mpuInterrupt = true;
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
  /*mpu.setSleepEnabled(false);
    mpu.setClockSource(MPU6050_CLOCK_INTERNAL);
    mpu.setRate(4);
    mpu.setDLPFMode(MPU6050_DLPF_BW_42);
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
    mpu.setDMPConfig1(0x03);
    mpu.setDMPConfig2(0x00);
    mpu.setFIFOEnabled(true);
    mpu.resetDMP();
	  //mpu.setDMPEnabled(false);
	  mpu.resetFIFO();
	  mpu.getIntStatus();*/

  // supply your own gyro offsets here, scaled for min sensitivity
  //mpu.setXGyroOffset(220);
  //mpu.setYGyroOffset(76);
  //mpu.setZGyroOffset(-85);
  //mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // Calibration Time: generate offsets and calibrate our MPU6050
  //mpu.CalibrateAccel(6);
  //mpu.CalibrateGyro(6);
  //mpu.PrintActiveOffsets();
  mpu.setDMPEnabled(true);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
  mpuIntStatus = mpu.getIntStatus();
  dmpReady = true;
  packetSize = mpu.dmpGetFIFOPacketSize();

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
  if (mpuInterrupt)
  {
    mpuInterrupt = false;
    digitalWrite(LED_PIN, 1);
    if (mpu.getIntMotionStatus())
    {
      Serial.println("Bummmm");
    }
    else if (mpu.getIntZeroMotionStatus())
    {
      Serial.println("Shallissimo");
    }
  }
  else
  {
    digitalWrite(LED_PIN, 0);
  }

  if (!digitalRead(3))
  {
    delay(1000);
    //MPUsetUp();
    MPUsetInt();
    mpu.setIntMotionEnabled(1);
  }
}