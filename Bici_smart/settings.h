#include "Arduino.h"
#include <avr/sleep.h>
#include <avr/interrupt.h>
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
#define NOISE_LENGTH 100
#define NOISE_TRESHOLD 10
int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t pax, pay, paz;
unsigned int noise[NOISE_LENGTH];
unsigned int max_noise;
float average_noise;
byte noise_index;


// Phases
enum phases{
    sleeping,
    checking,
    mooving
};
unsigned int phase = mooving;
byte loaded_phase;

// Timer
unsigned long int millisCounter;
unsigned long int millisCheckpoint;