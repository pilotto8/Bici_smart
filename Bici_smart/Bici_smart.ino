#include "settings.h"

void setup()
{
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  Serial.begin(1000000);
  Serial.println("\n----------------------------------------------------------------");
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  pinMode(FRONT_LED_PIN, OUTPUT);
  pinMode(REAR_LED_PIN, OUTPUT);
  pinMode(B0, INPUT_PULLUP);
  pinMode(B1, INPUT_PULLUP);
  pinMode(B2, INPUT_PULLUP);
  //pinMode(13, OUTPUT);
  //digitalWrite(13, LOW);

  rtc.begin();
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.disable32K();
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.writeSqwPinMode(DS3231_OFF);
  rtc.disableAlarm(2);

  MPUsetUp();
  mpu.setDMPEnabled(true);
  MPUsetInt();
  //mpu.setIntMotionEnabled(1);

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), interrupt, FALLING);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  
  for (int i = 0; i < 2; i++){
    LED_mode[i] = EEPROM.read(i);
  }
  
  getTime();
  if (now.hour() < ON_HOUR && now.hour() > OFF_HOUR) {
    setAlarm(0);
    Serial.println("RTC low");
  }
  else {
    setAlarm(1);
    Serial.println("RTC high");
  }

///// For debugging
  //rtc.setAlarm1(now + TimeSpan(30), DS3231_A1_Hour);
  
}

void loop()
{
  buttons();
  checkInterrupt();
  LEDhandle();

  switch (phase){
    case sleeping:{
      if (!changing){
        analogWrite(FRONT_LED_PIN, 0);
        analogWrite(REAR_LED_PIN, 0);

        if (phase != loaded_phase){
          loaded_phase = phase;
          MPUsetUp();
          MPUsetInt();
          mpu.setIntMotionEnabled(1);
          Serial.println("Going to sleep...");
        }

        
        if (RTC_state){
          if (mpu.getSleepEnabled()){
            MPUsetUp();
            MPUsetInt();
            mpu.setIntMotionEnabled(1);
            Serial.println("MPU on");
          }
        }
        else if (!mpu.getSleepEnabled()){
          mpu.setSleepEnabled(1);
          Serial.println("MPU off");
        }
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
      else if (millis() - millisCheckpoint > 1500){
        phase = mooving;
      }
      break;
    }


    case mooving:{
      if (phase != loaded_phase){
        if (loaded_phase == sleeping){
          if (mpu.getSleepEnabled()){
            mpu.setSleepEnabled(0);
            Serial.println("MPU on");
          }
          mpu.setIntMotionEnabled(0);
        }
        loaded_phase = phase;
        setLEDstate(1);
        millisCheckpoint = millis();

        Serial.println("Mooving!");
      }
      if (MPUgetNoise() < NOISE_TRESHOLD){
        if (millis() - millisCheckpoint > 10000){
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
}