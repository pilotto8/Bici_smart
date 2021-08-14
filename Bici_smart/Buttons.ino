bool stop;
void buttons(){
    if (!digitalRead(B0) || !digitalRead(B1) || !digitalRead(B2)){
        if (millis() - millisButtonPress > 30){
            if (!stop){
                if (LED_state){
                    if(!digitalRead(B0)){
                        LED_mode[0] = (LED_mode[0] + 1) % 4;
                        Serial.print("Front: ");
                        Serial.println(LED_mode[0]);
                    }
                    if(!digitalRead(B2)){
                        LED_mode[1] = (LED_mode[1] + 1) % 4;
                        Serial.print("Rear: ");
                        Serial.println(LED_mode[1]);
                    }
                }
                
                if (!digitalRead(B1)){
                    if (phase != mooving){
                        phase = mooving;
                    }
                    else{
                        setLEDstate(!LED_state);
                    }
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