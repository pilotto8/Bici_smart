#include "Arduino.h"
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <EEPROM.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif
MPU6050 mpu;

#define DEBUG 0

// Pin definitions
#define INTERRUPT_PIN 2
#define FRONT_LED_PIN 5
#define REAR_LED_PIN 6


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
unsigned long int millisCounter;
byte check_PH; 
unsigned long int millisCheckpoint;

// LEDs
#define MAX_FRONT_LED_POWER 200.0
#define MAX_REAR_LED_POWER 1024.0
#define LED_TRANSITION_TIME 100.0
const float FRONT_LED_COEFF =  MAX_FRONT_LED_POWER / LED_TRANSITION_TIME;
const float REAR_LED_COEFF = MAX_REAR_LED_POWER / LED_TRANSITION_TIME;
#if FRONT_LED_COEFF > REAR_LED_COEFF
#define MIN_STEP REAR_LED_COEFF
#else
#define MIN_STEP FRONT_LED_COEFF
#endif

float LED_power[2];
float LED_power_target[2];
unsigned int LED_blink[2];
unsigned int blink[2];
const float mode_map[4][4] = {
    {0.0, MAX_FRONT_LED_POWER, MAX_FRONT_LED_POWER / 10.0, MAX_FRONT_LED_POWER},
    {0.0, 0.0, 0.0, 50.0},
    {0.0, MAX_REAR_LED_POWER, MAX_REAR_LED_POWER / 10.0, MAX_REAR_LED_POWER},
    {0.0, 0.0, 0.0, 50.0}
};
byte LED_mode[2];
byte loaded_LED_mode[2];

bool changing;
bool change;

// Photoresistors
#define PH_CONTRAST_TRESTHOLD 10
int PH_value[2];
unsigned int PH_difference[10];
unsigned int PH_difference_average;
int PH_calibration[32] = {
    10, 15, 18, 27, 38, 45, 57, 66, 74, 82, 
    89, 95, 102, 106, 112, 116, 120, 122, 125, 127, 
    126, 126, 118, 121, 117, 120, 104, 83, 84, 85, 
    75, 57};
bool PH_calibration_done[32];
bool skip_calibration;