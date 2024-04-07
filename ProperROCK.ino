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

int count;

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

/*
Medium Range set up
*/
//system unification

int previousState = HIGH;

void chargeCapacitor() {
  top_timestamp = millis();  // Initial
  digitalWrite(PIN_ARM, HIGH);
  //time to reach 79.9 Volts
  //delay(33000);
  //Time to reach 79 Volts, Used for more efficient testing
  delay(15500);
  digitalWrite(PIN_ARM, LOW);
}

void calibrationStage() {
  //calibration loop
  Serial.println("Calibration start");
  int caldelta[calibrationTrials];

  for (int i = 0; i < calibrationTrials; ++i) {
    int calmn = 1024;  // Reset min for each iteration
    int calmx = 0;     // Reset max for each iteration
    //fft cal
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

    for (int j = 0; j < clipLength; ++j) {
      int calVal = analogRead(PIN_MIC);

      calmn = min(calmn, calVal);
      calmx = max(calmx, calVal);
    }

    caldelta[i] = calmx - calmn;  // Store the delta in the array
    //adding freq to array
    vec23[i] = fft1024_1.read(23);
    vec24[i] = fft1024_1.read(24);
    vec25[i] = fft1024_1.read(25);
    vec26[i] = fft1024_1.read(26);
    vec27[i] = fft1024_1.read(27);
    vec28[i] = fft1024_1.read(28);
    vec29[i] = fft1024_1.read(29);
    vec30[i] = fft1024_1.read(30);
    vec31[i] = fft1024_1.read(31);
    vec32[i] = fft1024_1.read(32);
    vec33[i] = fft1024_1.read(33);
    //sum to find average
    sum23 = sum23 + vec23[i];
    sum24 = sum24 + vec24[i];
    sum25 = sum25 + vec25[i];
    sum26 = sum26 + vec26[i];
    sum27 = sum27 + vec27[i];
    sum28 = sum28 + vec28[i];
    sum29 = sum29 + vec29[i];
    sum30 = sum30 + vec30[i];
    sum31 = sum31 + vec31[i];
    sum32 = sum32 + vec32[i];
    sum33 = sum33 + vec33[i];

    // Print the results for each iteration to see sound variation of each sound clip
    /*
      Serial.print("Iteration ");
      Serial.print(i + 1);
      Serial.print(": Min=");
      Serial.print(calmn);
      Serial.print(" Max=");
      Serial.print(calmx);
      Serial.print(" Delta=");
      Serial.println(caldelta[i]);
      */

    sum = sum + caldelta[i];
    //average sound variation in the environment
    int avg = sum / calibrationTrials;
    //threshold for sound variation
    thresh = avg * ampmag;
    //average frequencys in the environment
    float avg23 = sum23 / calibrationTrials;
    float avg24 = sum24 / calibrationTrials;
    float avg25 = sum25 / calibrationTrials;
    float avg26 = sum26 / calibrationTrials;
    float avg27 = sum27 / calibrationTrials;
    float avg28 = sum28 / calibrationTrials;
    float avg29 = sum29 / calibrationTrials;
    float avg30 = sum30 / calibrationTrials;
    float avg31 = sum31 / calibrationTrials;
    float avg32 = sum32 / calibrationTrials;
    float avg33 = sum33 / calibrationTrials;

    //threshold of frequency
    fftthresh23 = avg23 * mag;
    fftthresh24 = avg24 * mag;
    fftthresh25 = avg25 * mag;
    fftthresh26 = avg26 * mag;
    fftthresh27 = avg27 * mag;
    fftthresh28 = avg28 * mag;
    fftthresh29 = avg29 * mag;
    fftthresh30 = avg30 * mag;
    fftthresh31 = avg31 * mag;
    fftthresh32 = avg32 * mag;
    fftthresh33 = avg33 * mag;

    //see frequency thresholds
    Serial.println(fftthresh23, 4);
    Serial.println(fftthresh24, 4);
    Serial.println(fftthresh25, 4);
    Serial.println(fftthresh26, 4);
    Serial.println(fftthresh27, 4);
    Serial.println(fftthresh28, 4);
    Serial.println(fftthresh29, 4);
    Serial.println(fftthresh30, 4);
    Serial.println(fftthresh31, 4);
    Serial.println(fftthresh32, 4);
    Serial.println(fftthresh33, 4);
  }
  Serial.println("cal complete");
  sum = 0;
  sum23 = 0;
  sum24 = 0;
  sum25 = 0;
  sum26 = 0;
  sum27 = 0;
  sum28 = 0;
  sum29 = 0;
  sum30 = 0;
  sum31 = 0;
  sum32 = 0;
  sum33 = 0;
  //transmit('C');
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
}

void longRangeStage() {
  Serial.println("Waiting for a maybe");
  Serial.print(" maybe detected:");
  Serial.println(count);

  int mn = 1024;
  int mx = 0;

  for (int i = 0; i < 5000; ++i) {
    int val = analogRead(PIN_MIC);
    mn = min(mn, val);
    mx = max(mx, val);
  }
  int delta = mx - mn;
  int gate = analogRead(A3);
  //if there is an increase in noise of _ magnitufe set by thresh and increase of magnitide of _x in frequencys from 1000 - 1500
  if (((delta > thresh) && (fft1024_1.read(23) > fftthresh23) && (gate < gatethresh)) || ((delta > thresh) && (fft1024_1.read(24) > fftthresh24) && (gate < gatethresh)) || ((delta > thresh) && (fft1024_1.read(25) > fftthresh25) && (gate < gatethresh)) || ((delta > thresh) && (fft1024_1.read(26) > fftthresh26) && (gate < gatethresh)) || ((delta > thresh) && (fft1024_1.read(27) > fftthresh27) && (gate < gatethresh)) || ((delta > thresh) && (fft1024_1.read(28) > fftthresh28) && (gate < gatethresh)) || ((delta > thresh) && (fft1024_1.read(29) > fftthresh29) && (gate < gatethresh)) || ((delta > thresh) && (fft1024_1.read(30) > fftthresh30) && (gate < gatethresh)) || ((delta > thresh) && (fft1024_1.read(31) > fftthresh31) && (gate < gatethresh)) || ((delta > thresh) && (fft1024_1.read(32) > fftthresh32) && (gate < gatethresh)) || ((delta > thresh) && (fft1024_1.read(33) > fftthresh33) && (gate < gatethresh))) {
    Serial.println("car possibly detected");
    //wake jetson
    digitalWrite(PIN_WAKE, HIGH);
    digitalWrite(13, HIGH);
    delay(1500);
    //turn wake pin off, jetson stays on
    digitalWrite(PIN_WAKE, LOW);
    digitalWrite(13, LOW);
    Serial.println("waiting for jetson confirmation");
    //timer start
    timeOfMaybe = millis();
    state = MEDIUM_RANGE;
    //////////////////////////////////
  }
}

void mediumRangeStage() {
  Serial.println("car maybe approaching waiting for jetson confirmation");
  //looking for car affermation from jetson
  int currentState = digitalRead(PIN_SHORT_RANGE_ACTIVATION);
  //timer start once a car has possibly been detected to allow for a reset if a car doesn't drive over it
  waitingForJetson = millis();
  //if jetson sends postive car signal
  if (currentState != previousState) {
    Serial.println("recieved jetson confirmation");
    state = SHORT_RANGE;
  }
}

void shortRangeStage() {
  do {
    for (int i = 0; i < 4; i++) {
      data[i] = mySerial.read();
    }
  } while (mySerial.read() == 0xff);

  mySerial.flush();

  if (data[0] == 0xff) {
    int sum;
    sum = (data[0] + data[1] + data[2]) & 0x00FF;

    if (sum == data[3]) {
      distance = (data[1] << 8) + data[2];

      if (distance > 30) {
        Serial.print("distance=");
        Serial.print(distance / 10);
        Serial.println("cm");

        if ((distance / 10) > 4 && (distance / 10) < 20) {
          //set the fire pin to fire to fire the R.O.C.K.
          //delay to ensure full discharge
          Serial.println("Launch");
          digitalWrite(PIN_FIRE, HIGH);
          delay(500);
          digitalWrite(PIN_FIRE, LOW);
          Serial.println("Reset");
          delay(1500);
          chargeCapacitor();
          state = CALIBRATION;
        }

      } else {
        Serial.println("Below the lower limit");
        
      }
    } else Serial.println("ERROR");
  }
  delay(100);
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
