void MPUsetUp(){
	mpu.reset();
	delay(30);
	mpu.setSleepEnabled(false);

	// get MPU hardware revision
	mpu.setMemoryStartAddress(0x06);
	mpu.setMemoryBank(0, false, false);

	mpu.setClockSource(MPU6050_CLOCK_INTERNAL);
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
}

void MPUsetInt(){
    I2Cdev::writeByte(0x68, MPU6050_RA_INT_ENABLE, 0);
    
    mpu.setMotionDetectionThreshold(2);
    mpu.setMotionDetectionDuration(1);
    mpu.setMotionDetectionCounterDecrement(3);

    mpu.setZeroMotionDetectionThreshold(1);
    mpu.setZeroMotionDetectionDuration(1);
    
    mpu.setInterruptLatch(1);
    mpu.setInterruptLatchClear(0);
}