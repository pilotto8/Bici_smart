int PHget(){
  check_light++;
  if (check_light >= LIGHT_SAMPLING_TIME){
    check_light = 0;
    PH_value = analogRead(PH);
  }
}