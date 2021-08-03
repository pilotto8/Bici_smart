#include "settings.h"
void setup()
{
  Wire.begin();
  Serial.begin(115200);
  
  // Set up the interrupt pin, its set as active high, push-pull
  pinMode(intPin, INPUT);
  digitalWrite(intPin, LOW);
  Serial.println(mpu.readByte(MPU6050_ADDRESS, 0x6b), BIN);
  #if CAL
    MPUcalibration();
  #endif
}

void loop()
{  
  // If data ready bit set, all data registers have new data
  if(mpu.readByte(MPU6050_ADDRESS, INT_STATUS) & 0x01) {  // check if data ready interrupt

    mpu.readAccelData(accelCount);  // Read the x/y/z adc values
    aRes=mpu.getAres();
    
    // Now we'll calculate the accleration value into actual g's
    ax = (float)accelCount[0]*aRes - accelBias[0];  // get actual g value, this depends on scale being set
    ay = (float)accelCount[1]*aRes - accelBias[1];   
    az = (float)accelCount[2]*aRes - accelBias[2];  
   
    mpu.readGyroData(gyroCount);  // Read the x/y/z adc values
    gRes=mpu.getGres();
 
    // Calculate the gyro value into actual degrees per second
    gx = (float)gyroCount[0]*gRes - gyroBias[0];  // get actual gyro value, this depends on scale being set
    gy = (float)gyroCount[1]*gRes - gyroBias[1];  
    gz = (float)gyroCount[2]*gRes - gyroBias[2];   

    tempCount = mpu.readTempData();  // Read the x/y/z adc values
    temperature = ((float) tempCount) / 340. + 36.53; // Temperature in degrees Centigrade
   }  
   
    uint32_t deltat = millis() - count;
    if(deltat > 50) {
 
    // Print acceleration values in milligs!
    Serial.print(1000*ax); Serial.print(", "); 
    Serial.print(1000*ay); Serial.print(", "); 
    Serial.print(1000*az); Serial.print(", "); 

    // Print gyro values in degree/sec
    Serial.print(gx, 1); Serial.print(", "); 
    Serial.print(gy, 1); Serial.print(", "); 
    Serial.print(gz, 1); Serial.println(", "); 
    
    /*
    // Print temperature in degrees Centigrade      
    Serial.print("Temperature is ");  Serial.print(temperature, 2);  Serial.println(" degrees C"); // Print T values to tenths of s degree C
    Serial.println("");
    */  
    count = millis();
    }

}
