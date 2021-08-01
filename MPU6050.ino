void MPUcalibration() {
  mpu.MPU6050SelfTest(SelfTest); // Start by performing self test and reporting values
  Serial.print("x-axis self test: acceleration trim within : "); Serial.print(SelfTest[0], 1); Serial.println("% of factory value");
  Serial.print("y-axis self test: acceleration trim within : "); Serial.print(SelfTest[1], 1); Serial.println("% of factory value");
  Serial.print("z-axis self test: acceleration trim within : "); Serial.print(SelfTest[2], 1); Serial.println("% of factory value");
  Serial.print("x-axis self test: gyration trim within : "); Serial.print(SelfTest[3], 1); Serial.println("% of factory value");
  Serial.print("y-axis self test: gyration trim within : "); Serial.print(SelfTest[4], 1); Serial.println("% of factory value");
  Serial.print("z-axis self test: gyration trim within : "); Serial.print(SelfTest[5], 1); Serial.println("% of factory value");

  if (SelfTest[0] < 1.0f && SelfTest[1] < 1.0f && SelfTest[2] < 1.0f && SelfTest[3] < 1.0f && SelfTest[4] < 1.0f && SelfTest[5] < 1.0f) {
    Serial.println("Pass Selftest!");

    mpu.calibrateMPU6050(gyroBias, accelBias); // Calibrate gyro and accelerometers, load biases in bias registers
    mpu.initMPU6050(); Serial.println("MPU6050 initialized for active data mode...."); // Initialize device for active mode read of acclerometer, gyroscope, and temperature

  }
}

void wakeUp(){
  Serial.println("Fanculo medde");
}
