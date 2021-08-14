void MPUsetUp(){
	mpu.reset();
	delay(30);
	mpu.setSleepEnabled(0);
    mpu.setTempSensorEnabled(0);

	// get MPU hardware revision
	mpu.setMemoryStartAddress(0x06);
	mpu.setMemoryBank(0, false, false);

	DEBUG_PRINTLN(F("Setting slave 0 address to 0x7F..."));
	mpu.setSlaveAddress(0, 0x7F);
	DEBUG_PRINTLN(F("Disabling I2C Master mode..."));
	mpu.setI2CMasterModeEnabled(false);
	DEBUG_PRINTLN(F("Setting slave 0 address to 0x68 (self)..."));
	mpu.setSlaveAddress(0, 0x68);
	DEBUG_PRINTLN(F("Resetting I2C Master control..."));
	mpu.resetI2CMaster();
	delay(20);

	mpu.setClockSource(MPU6050_CLOCK_PLL_ZGYRO);
	mpu.setRate(4); // 1khz / (1 + 4) = 200 Hz
    mpu.setDLPFMode(MPU6050_DLPF_BW_42);
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);

	mpu.setExternalFrameSync(MPU6050_EXT_SYNC_TEMP_OUT_L);

	// load DMP code into memory banks
	mpu.writeProgMemoryBlock(dmpMemory, MPU6050_DMP_CODE_SIZE);

	// Set the FIFO Rate Divisor int the DMP Firmware Memory
	unsigned char dmpUpdate[] = {0x00, MPU6050_DMP_FIFO_RATE_DIVISOR};
	mpu.writeMemoryBlock(dmpUpdate, 0x02, 0x02, 0x16); // Lets write the dmpUpdate data to the Firmware image, we have 2 bytes to write in bank 0x02 with the Offset 0x16

	//write start address MSB into register
	mpu.setDMPConfig1(0x03);
	//write start address LSB into register
	mpu.setDMPConfig2(0x00);

	DEBUG_PRINTLN(F("Enabling FIFO..."));
	mpu.setFIFOEnabled(true);

	DEBUG_PRINTLN(F("Resetting DMP..."));
	mpu.resetDMP();

	DEBUG_PRINTLN(F("Disabling DMP (you turn it on later)..."));
	mpu.setDMPEnabled(false);

	DEBUG_PRINTLN(F("Resetting FIFO and clearing INT status one last time..."));
	mpu.resetFIFO();
	mpu.getIntStatus();

	mpu.setStandbyXGyroEnabled(1);
	mpu.setStandbyYGyroEnabled(1);
	mpu.setStandbyZGyroEnabled(1);
}

void MPUsetInt(){
    I2Cdev::writeByte(0x68, MPU6050_RA_INT_ENABLE, 0);
    
    mpu.setMotionDetectionThreshold(15);
    mpu.setMotionDetectionDuration(1);
    mpu.setMotionDetectionCounterDecrement(3);
    
	mpu.setInterruptDrive(1);
	mpu.setInterruptMode(1);
    mpu.setInterruptLatch(0);
    mpu.setInterruptLatchClear(0);

	#if DEBUG
	Serial.println("Motion detection:");
    Serial.println(mpu.getMotionDetectionThreshold());
    Serial.println(mpu.getMotionDetectionDuration());
    Serial.println(mpu.getMotionDetectionCounterDecrement());
    Serial.println(mpu.getIntMotionEnabled());
    Serial.println(mpu.getIntEnabled(), BIN);
	#endif
}

float MPUgetNoise(){
    if (millis() - noiseMillisCounter > NOISE_SAMPLING_DELAY){
      noiseMillisCounter = millis() / 10 * 10;
      pax = ax;
      pay = ay;
      paz = az;
      mpu.getAcceleration(&ax, &ay, &az);
      max_noise = 0;
      if (abs(ax - pax) > max_noise){
        max_noise = abs(ax - pax);
      }
      if (abs(ay - pay) > max_noise){
        max_noise = abs(ay - pay);
      }
      if (abs(az - paz) > max_noise){
        max_noise = abs(az - paz);
      }
      average_noise *= NOISE_LENGTH;
      average_noise -= noise[noise_index];
      noise[noise_index] = max_noise / 10;
      average_noise += noise[noise_index];
	  average_noise /= NOISE_LENGTH;
      noise_index = (noise_index + 1) % NOISE_LENGTH;
	  #if DEBUG
	  	Serial.print(average_noise);
	  	Serial.print(", ");
	  	Serial.println(max_noise / 10);
	  #endif
	}
	return average_noise;
}