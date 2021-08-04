#include "Arduino.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu;

#define INTERRUPT_PIN 2
#define LED_PIN 13

#define MAX_FRONT_LED_POWER 250

// MPU values
int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t pax, pay, paz;
unsigned int noise[100];
unsigned int max_noise;
unsigned int average_noise;
byte noise_index;


// Phases
unsigned int phase = checking;
byte loaded_phase;
enum phases{
    sleeping,
    checking,
    mooving
};


// Timer
unsigned long int millisCounter;