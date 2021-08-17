void interrupt(){
    interrupt_fired = 1;
}

void checkInterrupt(){
    if (interrupt_fired){
        Serial.print("Interrupt: ");
        interrupt_fired = 0;
        if (!digitalRead(B1)){
            Serial.println("button");
            return;
        }

        if (RTC_state){
            if (mpu.getIntMotionStatus()){
                phase = checking;
                Serial.println("MPU");
                return;
            }
        }
    
        if (rtc.alarmFired(1)){
            setAlarm(!RTC_state);
            Serial.println("RTC");
            return;
        }

        Serial.println("null");
    }
}