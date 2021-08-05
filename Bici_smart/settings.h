#include "Arduino.h"
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif
MPU6050 mpu;

#define DEBUG 0

// Pin definitions
#define INTERRUPT_PIN 2
#define FRONT_LED_PIN 6
#define REAR_LED_PIN 7


// MPU values
#define NOISE_LENGTH 20
#define NOISE_TRESHOLD 10
#define NOISE_SAMPLING_DELAY 50
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
unsigned long int noiseMillisCounter;
unsigned long int LEDmillisCounter;
unsigned long int millisCheckpoint;

// LEDs
#define MAX_FRONT_LED_POWER 250
#define MAX_REAR_LED_POWER 1024
#define LED_TRANSITION_TIME 100
#define FRONT_LED_COEFF = LED_TRANSITION_TIME / MAX_FRONT_LED_POWER
#define REAR_LED_COEFF = LED_TRANSITION_TIME / MAX_REAR_LED_POWER
float LED_power[2];
unsigned int LED_power_target[2];
unsigned int LED_blink[2];
unsigned int blink[2];
static const unsigned int mode_map[4][4] PROGMEM = {
    {0, MAX_FRONT_LED_POWER, MAX_FRONT_LED_POWER / 2, MAX_FRONT_LED_POWER},
    {0, 0, 0, 500},
    {0, MAX_REAR_LED_POWER, MAX_REAR_LED_POWER / 2, MAX_REAR_LED_POWER},
    {0, 0, 0, 500}
};
byte LED_mode[2];
byte loaded_LED_mode[2];

LED_configurations LED_state[3];
bool forced;