#include "settings.h"

void interrupt()
{
  if (RTC_state){
    if (mpu.getIntMotionStatus()){
      phase = checking;
      return;
    }
  }
  if (rtc.alarmFired(1)){
    setAlarm(!RTC_state);
    return;
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
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  pinMode(FRONT_LED_PIN, OUTPUT);
  pinMode(REAR_LED_PIN, OUTPUT);
  pinMode(B0, INPUT_PULLUP);
  pinMode(B1, INPUT_PULLUP);
  pinMode(B2, INPUT_PULLUP);

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
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), interrupt, FALLING);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  MPUsetInt();
  //mpu.setIntMotionEnabled(1);

  
  for (int i = 0; i < 2; i++){
    LED_mode[i] = EEPROM.read(i);
    Serial.println(LED_mode[i]);
  }
  
  getTime();
  if (now.hour() < ON_HOUR && now.hour() > OFF_HOUR){
    loaded_LED_mode[0] = LED_mode[0];
    loaded_LED_mode[1] = LED_mode[1];
    LED_state = 0;
    setAlarm(0);
  }
  else {
    LED_state = 1;
    setAlarm(1);
  }
  
}

void loop()
{
  LEDhandle();
  buttons();

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
        if (RTC_state && mpu.getSleepEnabled()){
          mpu.setSleepEnabled(0);
        }
        else if (!mpu.getSleepEnabled()){
          mpu.setSleepEnabled(1);
        }
        //set_sleep_mode(SLEEP_MODE_PWR_DOWN);
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
      if (MPUgetNoise() < NOISE_TRESHOLD && millis() - millisCheckpoint > 500){ 
        phase = sleeping;
      }
      else if (millis() - millisCheckpoint > 1000){
        phase = mooving;
      }
      break;
    }
    case mooving:{
      if (phase != loaded_phase){
        if (loaded_phase == sleeping){
          mpu.setSleepEnabled(0);
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