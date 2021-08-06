void setLEDstate(bool state){
    if (state){
        for (int i = 0; i < 2; i++){
            LED_power_target[i] = mode_map[i * 2][LED_mode[i]];
        }
    }
    else {
        for (int i = 0; i < 2; i++){
            LED_power_target[i] = 0;
        }
    }
}
void LEDhandle(){
    if (millis() - LEDmillisCounter > 10){
        LEDmillisCounter = millis() / 10 * 10;
        changing = 0;
        for (int i = 0; i < 2; i++){
            if (LED_mode[i] != loaded_LED_mode[i]){
                loaded_LED_mode[i] = LED_mode[i];
                LED_power_target[i] = mode_map[i * 2][LED_mode[i]];
                if (!LED_mode[i]){
                    LED_blink[i] = mode_map[i * 2 + 1][LED_mode[i]];
                }
                EEPROM.update(i, LED_mode[i]);
                /*Serial.print(LED_power_target[i]);
                Serial.print(", ");
                Serial.println(LED_blink[i]);*/
            }
            if (LED_blink[i] > 0){
                blink[i]++;
                Serial.println(blink[i]);
                if (blink[i] == int(LED_blink[i] / 2)){
                    analogWrite(FRONT_LED_PIN + i, 0);
                }
                if (blink[i] == int(LED_blink[i])){
                    blink[i] = 0;
                    analogWrite(FRONT_LED_PIN + i, (int)LED_power[i]);
                }
            }

            if (abs(LED_power_target[i] - LED_power[i]) >= MIN_STEP){
                changing = 1;
                LED_power[i] += (FRONT_LED_COEFF * !i + REAR_LED_COEFF * i) * ((LED_power_target[i] - LED_power[i] > 0) ? 1 : -1);
                /*Serial.print(LED_power_target[i]);
                Serial.print(", ");
                Serial.println((int)LED_power[i]);*/
                if (LED_blink[i] == 0.0){
                    analogWrite(FRONT_LED_PIN + i, (int)LED_power[i]);
                }
            }
        }
    }
}