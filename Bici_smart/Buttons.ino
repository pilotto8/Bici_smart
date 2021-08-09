bool stop;
void buttons(){
    if (!digitalRead(B0) || !digitalRead(B1)){
        if (millis() - millisButtonPress > 30){
            if (!stop && LED_state){
                if(!digitalRead(B0)){
                    LED_mode[0] = (LED_mode[0] + 1) % 4;
                    phase = mooving;
                    Serial.print("Front: ");
                    Serial.println(LED_mode[0]);
                }
                else if(!digitalRead(B1)){
                    LED_mode[1] = (LED_mode[1] + 1) % 4;
                    phase = mooving;
                    Serial.print("Rear: ");
                    Serial.println(LED_mode[1]);
                }
            }
            else {
                setLEDstate(1);
            }
            millisButtonPress = millis();
            stop = 1;
        }
    }
    else {
        stop = 0;
    }
}