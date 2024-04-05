/***********************************
R.O.C.K. Protocol
Version: 0.9
Author: Jack Foster and Branson Camp
Date: 4/4/2024
Description: (Add description here)
***********************************/

// States
#define LONG_RANGE 0
#define MEDIUM_RANGE 1
#define SHORT_RANGE 2
#define CALIBRATION 3

// Pin Definitions
#define PIN_ARM 10
#define PIN_FIRE 11
#define PIN_CAR 9   // I have this as 9
#define PIN_WAKE 9  //
#define PIN_LED 13
#define PIN_MIC A2
#define PIN_SHORT_RANGE_ACTIVATION 3

// Capacitor Charging Definitions
#define FULL_CHARGE_TIME 33000
#define TOP_OFF_TIME 6000
#define TIME_BETWEEN_TOP_OFFS (15 * 60 * 1000)

// Capacitor Charging Variables
uint32_t top_timestamp = 0;
uint32_t time_since_top = 0;
uint32_t top_duration = 0;
uint32_t time_of_charging = 0;

// State Variables
int state = CALIBRATION;
bool capacitorCharging = false;

/*
Long Range Calibration Variables
*/

//FFT set up
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
// GUItool: begin automatically generated code
AudioInputAnalog adc1;          //xy=96.00000381469727,147.0000114440918
AudioAnalyzeFFT1024 fft1024_1;  //xy=369.0000114440918,99.00006580352783
AudioConnection patchCord1(adc1, fft1024_1);
// GUItool: end automatically generated code

// Long Range Calibration Variables (20)
const int myInput = AUDIO_INPUT_MIC;
int clipLength = 15000;
int calibrationTrials = 40;
int sum = 0;

float sum23 = 0;
float sum24 = 0;
float sum25 = 0;
float sum26 = 0;
float sum27 = 0;
float sum28 = 0;
float sum29 = 0;
float sum30 = 0;
float sum31 = 0;
float sum32 = 0;
float sum33 = 0;

int mag = 3.5;
int ampmag = 1.2;

float fftthresh23;
float fftthresh24;
float fftthresh25;
float fftthresh26;
float fftthresh27;
float fftthresh28;
float fftthresh29;
float fftthresh30;
float fftthresh31;
float fftthresh32;
float fftthresh33;

int t = 1;
int thresh;

//Recalibration timer // maybe its own function eh
const unsigned long reCalTime = 61000;
unsigned long timeOfMaybe;
unsigned long waitingForJetson;

//Mic Peak
int gatethresh = 30;

//long range postive counter
int Number_of_Long_Positives;

/*
Short Range Setup
*/
#include <SoftwareSerial.h>

SoftwareSerial mySerial(0, 1);  // RX, TX
unsigned char data[4] = {};
float distance;


void chargeCapacitor() {
  top_timestamp = millis();  // Initial
  digitalWrite(PIN_ARM, HIGH);
  //time to reach 79.9 Volts
  delay(33000);
  //Time to reach 79 Volts, Used for more efficient testing
  //delay(15500);
  digitalWrite(PIN_ARM, LOW);
}

void calibrationStage() {
  //Calibration loop
  Serial.println("Calibration start");
  //set vector length for storing change in the sounds magnitude durring each audio clip
  int caldelta[calibrationTrials];
  //establish vectors for each frequency bin to collect the magnitude of the frequency in each trial
  float vec23[calibrationTrials];
  float vec24[calibrationTrials];
  float vec25[calibrationTrials];
  float vec26[calibrationTrials];
  float vec27[calibrationTrials];
  float vec28[calibrationTrials];
  float vec29[calibrationTrials];
  float vec30[calibrationTrials];
  float vec31[calibrationTrials];
  float vec32[calibrationTrials];
  float vec33[calibrationTrials];
  //max and min values of mircophone readings
  int mn = 1024;
  int mx = 0;
}

void longRangeStage() {
}

void mediumRangeStage() {
  // Wake Jetson
}

void shortRangeStage() {
}

void blinkLed() {
  digitalWrite(PIN_LED, 1);
  delay(500);
  digitalWrite(PIN_LED, 0);
}

void debugSend(char code) {
  // Serial 2 code here, write? print?
  // Branson will fill this out later
}

void setup() {
  // Configure GPIO
  pinMode(PIN_ARM, OUTPUT);
  pinMode(PIN_FIRE, OUTPUT);
  pinMode(PIN_CAR, INPUT);
  pinMode(PIN_WAKE, OUTPUT);
  pinMode(PIN_SHORT_RANGE_ACTIVATION, INPUT);
  pinMode(PIN_LED, 13);
  digitalWrite(PIN_ARM, LOW);
  digitalWrite(PIN_FIRE, LOW);

  // Configure Serial
  Serial.begin(57600);   //Ultrasonic sensor uses this speed
  mySerial.begin(9600);  //Ultrasonic uses this too... please don't use this sensor <3
  Serial.println("Initialized.");

  //Fully charge the capacitor upon boot
  chargeCapacitor();

  // Configure FFT
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);
  // Configure the window algorithm to use
  fft1024_1.windowFunction(AudioWindowHanning1024);
}

void topOffHandler() {
  time_since_top = millis() - top_timestamp;
  if (time_since_top > TIME_BETWEEN_TOP_OFFS) {
    if (capacitorCharging) {
      if (millis() - time_of_charging > TOP_OFF_TIME) {
        digitalWrite(PIN_ARM, LOW);
        top_timestamp = millis();
        capacitorCharging = false;
      }
    } else {
      time_of_charging = millis();
      capacitorCharging = true;  // We're starting to charge
      digitalWrite(PIN_ARM, HIGH);
    }
  }
}

void loop() {
  topOffHandler();

  switch (state) {
    case CALIBRATION:
      Serial.println("===Calibration Stage===");
      calibrationStage();
      debugSend('A');
      break;
    case LONG_RANGE:
      Serial.println("===Long Rang Stage===");
      longRangeStage();
      debugSend('B');
      break;
    case MEDIUM_RANGE:
      Serial.println("===Medium Range Stage===");
      mediumRangeStage();
      debugSend('C');
      break;
    case SHORT_RANGE:
      Serial.println("===Short Range Stage===");
      shortRangeStage();
      debugSend('D');
    default:
      Serial.println("CRITICAL ERROR UNKNOWN STATE");
      while (1) {}
  };
}