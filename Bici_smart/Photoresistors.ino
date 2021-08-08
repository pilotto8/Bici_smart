int PHget(){
  check_PH++;
  if (check_PH >= PH_SAMPLING_TIME){
    check_PH = 0;
    int i;
    double m;
    bool done = 0;
    PH_value[0] = analogRead(PH0);
    PH_value[1] = analogRead(PH1);
    for (i = 0; i < 32 && !done; i++){
      if (PH_value[0] <= i * 32){
          done = 1;
      }
    }
    i--;
    m = (PH_calibration[i] - PH_calibration[i - 1]) / 32.0;
    PH_value[0] -= m * (PH_value[0] - (i - 1) * 32) + PH_calibration[i - 1];
    /*Serial.print(PH_value[0]);
    Serial.print(", ");
    Serial.print(i);
    Serial.print(", ");
    Serial.print(m);
    Serial.print(", ");
    Serial.print(m * (PH_value[0] - (i - 1) * 32) + PH_calibration[i - 1]);
    Serial.print(", ");
    Serial.println(PH_value[0] - PH_value[1]);*/
    Serial.print(PH_value[0]);
    Serial.print(", ");
    Serial.println(PH_value[1]);
  }
}

void PHcalibrate(){
  if (!skip_calibration){
    skip_calibration = true;
    PH_value[0] = analogRead(PH0);
    PH_value[1] = analogRead(PH1);
    for (int i = 1; i < 32; i++){
      Serial.print(PH_calibration_done[i]);
      Serial.print(", ");
      if (PH_value[0] == i * 32 && !PH_calibration_done[i]){
        PH_calibration[i] = PH_value[0] - PH_value[1];
        PH_calibration_done[i] = 1;
      }
      if (PH_calibration_done[i] == 0){
        skip_calibration = false;
      }
    }
    Serial.println("");
  }
  else {
    Serial.println("Done calibration:");
    for (int i = 1; i < 32; i++){
      PH_calibration_done[i] = 0;
      Serial.print(PH_calibration[i]);
      Serial.print(", ");
    }
    skip_calibration = false;
    Serial.println("");
    delay(5000);
  }
}