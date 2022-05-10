void getTime(){
    now = rtc.now();
}
void setAlarm(bool state){
    rtc.clearAlarm(1);
    if (!state){
        rtc.setAlarm1(alarm_ON, DS3231_A1_Hour);
    }
    else{
        rtc.setAlarm1(alarm_OFF, DS3231_A1_Hour);
    }
    RTC_state = 1; //state;
}