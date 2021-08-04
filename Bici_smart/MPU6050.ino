void MPUsetUp(){
	mpu.reset();
	delay(30);
	mpu.setSleepEnabled(0);
    mpu.setTempSensorEnabled(0);

	// get MPU hardware revision
	mpu.setMemoryStartAddress(0x06);
	mpu.setMemoryBank(0, false, false);

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

	//mpu.setWakeFrequency(1);
    //mpu.setWakeCycleEnabled(2);
}

void MPUsetInt(){
    I2Cdev::writeByte(0x68, MPU6050_RA_INT_ENABLE, 0);
    
    mpu.setMotionDetectionThreshold(2);
    mpu.setMotionDetectionDuration(1);
    mpu.setMotionDetectionCounterDecrement(3);
    
    mpu.setInterruptLatch(1);
    mpu.setInterruptLatchClear(0);
}

int MPUgetNoise(){
    if (millis() - millisCounter > 10){
      millisCounter = millis() / 10 * 10;
      pax = ax;
      pay = ay;
      paz = az;
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
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
      average_noise *= 100;
      average_noise -= noise[noise_index];
      noise[noise_index] = max_noise;
      average_noise += noise[noise_index];
      average_noise /= 100;
      noise[noise_index] = (noise_index + 1) % 100;
	  Serial.print(average_noise);
	  Serial.print(", ");
	  Serial.println(max_noise);
	}
	return average_noise;
}