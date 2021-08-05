void setLEDstate(bool state){
    if (state){
        for (int i = 0; i < 2; i++){
            LED_power_target[i] = mode_map[LED_mode[i]][i * 2];
        }
    }
    else{
        for (int i = 0; i < 2; i++){
            LED_power_target[i] = 0;
        }
    }
}
void LEDhandle(){
    if (millis() - LEDmillisCounter > 10){
        LEDmillisCounter = millis() / 10 * 10;
        for (int i = 0; i < 2; i++){
            if (LED_mode[i] != loaded_LED_mode[i]){
                loaded_LED_mode[i] = LED_mode[i];
                LED_power_target[i] = mode_map[LED_mode[i]][i * 2];
                if (!LED_mode[i]){
                    LED_blink[i] = mode_map[LED_mode[i]][i * 2 + 1];
                }
                EEPROM.update(i, LED_mode[i]);
            }
            if (LED_blink[i] != 0){
                blink[i]++;
                if (blink[i] == int(LED_blink[i] / 2)){
                    digitalWrite(FRONT_LED_PIN + i, 0);
                }
                if (blink[i] == LED_blink[i]){
                    blink[i] = 0;
                    digitalWrite(FRONT_LED_PIN + i, LED_power[i]);
                }
            }

            if (int(LED_power[i]) != LED_power_target[i]){
                changing = 1;
                LED_power[i] += (FRONT_LED_COEFF * !i + REAR_LED_COEFF * i) * ((LED_power_target[i] - LED_power[i] > 0) ? 1 : -1);
                digitalWrite(FRONT_LED_PIN + i, LED_power[i]);
            }
            else if (changing){
                changing = 0;
            }
        }
    }
}