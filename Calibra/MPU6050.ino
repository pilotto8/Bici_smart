void MPUcalibration() {
  mpu.MPU6050SelfTest(SelfTest); // Start by performing self test and reporting values
  if (SelfTest[0] < 1.0f && SelfTest[1] < 1.0f && SelfTest[2] < 1.0f && SelfTest[3] < 1.0f && SelfTest[4] < 1.0f && SelfTest[5] < 1.0f) {
    mpu.calibrateMPU6050(gyroBias, accelBias); // Calibrate gyro and accelerometers, load biases in bias registers
    mpu.initMPU6050();

  }
}
