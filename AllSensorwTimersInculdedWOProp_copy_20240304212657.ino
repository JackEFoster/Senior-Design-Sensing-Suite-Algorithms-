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
double fftthresh = 0.005;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const int myInput = AUDIO_INPUT_MIC;

// my ampluite stuff
//amp
const int microphonePin = A2;
const int irPin = A5;


//calibration set up lines 20 - 69
int clipLength = 10000;
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

int mag = 1.5;

int t = 1;
int thresh;

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



//pins set up
const int jetPin = 9;
const int armPin = 4;
const int firePin = 2;
int scan;

//averaging of IR
#define WINDOW_SIZE 7
int INDEX = 0;
int dist = 0;
int SUM = 0;
int READINGS[WINDOW_SIZE];
int AVERAGED = 0;

//system unification
const int base = 2;
const int startShort = 3;
const int emit = 4;
int previousState = LOW;
bool jetsonSeesCar = false;

bool maybeCar = false;
bool allIsQuite = true;

//detection timer
const unsigned long reCalTime = 60000;
unsigned long timeOfMaybe;
unsigned long waitingForJetson;

//capacitor timer
const unsigned long chargeTime = 3600000;
unsigned long onTime;
unsigned long timePast;
unsigned long capOn;
unsigned long chargingTime;
const unsigned long topOffTime = 10000;
bool charge = false;


//////////////////////////////////////////////////////



void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);
  // Configure the window algorithm to use
  fft1024_1.windowFunction(AudioWindowHanning1024);
  //fft1024_1.windowFunction(NULL);

  //amplutide set up
  //amplitude setup
  Serial.begin(9600);

  ////////////////////////////////////////////////
  //jetson talk set up
  pinMode(startShort, INPUT);
  digitalWrite(base, LOW);
  digitalWrite(startShort, LOW);
  //internal LED
  digitalWrite(13, LOW);
  /////////////////////////////////////////////////
  /*
  //charge capacitor fully
  digitalWrite(_,HIGH);
  delay(_);
  */
  //capacitor top off timer stuff
  onTime = millis();
}

void loop() {

  //calibration loop
  int calmn = 1024;
  int calmx = 0;
  int mn = 1024;
  int mx = 0;

  if (t == 1) {
    // Declare an array to store delta values
    Serial.println("Calibration start");
    int caldelta[calibrationTrials];

    for (int i = 0; i < calibrationTrials; ++i) {
      calmn = 1024;  // Reset min for each iteration
      calmx = 0;     // Reset max for each iteration
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
        int calVal = analogRead(microphonePin);

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
      thresh = avg + 3;
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
      /*
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
      */
      // if statement to stop the nest calibration for loop inorder to set threshold
      if (i >= 39) {
        Serial.println("cal complete");
        t = 2;
      }
    }
    /*
    // Print the entire delta array
    Serial.print("Delta Array: ");
    for (int i = 0; i < calibrationTrials; ++i) {

      Serial.print(caldelta[i]);
      Serial.print(" ");
      Serial.println();
      Serial.print(thresh);
      Serial.print(" ");
      Serial.println();
    }
    */
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  //constant vigilance - Madeye Moody
  //constant scan of magntiude od sound change in the enviornment 
  for (int i = 0; i < 5000; ++i) {
    int val = analogRead(microphonePin);
    mn = min(mn, val);
    mx = max(mx, val);
  }
  int delta = mx - mn;

  //time since last charge
  timePast = millis();

  if (timePast - onTime > chargeTime) {
    // set new start time to reset top off clock
    Serial.println("capacitor top off begin");
    onTime = millis();

    // set start for the top off
    chargingTime = millis();
    //activate charging loop
    charge = true;
    if (charge == true) {
      //track time off charge
      capOn = millis();
      //top off the capacitor for _ time
      while ((capOn - chargingTime) <= topOffTime) {
        Serial.println("capacitor topping off");
        //turn on the capactior charging pin
        //digitalWrite(_,HIGH);
      }
      // if it exceeds _ time stop charging
      if (capOn - chargingTime > topOffTime) {
        Serial.println("Stop of capacitor top off");
        charge = false;
      }
    }
  }

  Serial.println("Waiting for a maybe");
  /*
  Serial.print("Threshold:");
  Serial.print(thresh);
  Serial.print(" 23:");
  Serial.print(fftthresh23);
  */

/*
  //visualisation of sound happenings on serial monitor 
  Serial.print(" Min:");
  Serial.print(mn);
  Serial.print(" Max:");
  Serial.print(mx);
  Serial.print(" Delta:");
  Serial.println(delta);

  //fft set up
  float n;
  int i;
  if (fft1024_1.available()) {
    // each time new FFT data is available
    // print it all to the Arduino Serial Monitor
    Serial.print("FFT: ");
    for (i = 0; i < 40; i++) {
      n = fft1024_1.read(i);
      if (n >= fftthresh) {
        Serial.print(n);
        Serial.print(" ");
      } else {
        Serial.print("  -  ");  // don't print "0.00"
      }
    }
    Serial.println();
  }
*/
  if (allIsQuite == true) {
    //if there is an increase in noise of _ magnitufe set by thresh and increase of magnitide of _x in frequencys from 1000 - 1500
    if (((delta > thresh) && (fft1024_1.read(23) > fftthresh23)) || ((delta > thresh) && (fft1024_1.read(24) > fftthresh24)) || ((delta > thresh) && (fft1024_1.read(25) > fftthresh25)) || ((delta > thresh) && (fft1024_1.read(26) > fftthresh26)) || ((delta > thresh) && (fft1024_1.read(27) > fftthresh27)) || ((delta > thresh) && (fft1024_1.read(28) > fftthresh28)) || ((delta > thresh) && (fft1024_1.read(29) > fftthresh29)) || ((delta > thresh) && (fft1024_1.read(30) > fftthresh30)) || ((delta > thresh) && (fft1024_1.read(31) > fftthresh31)) || ((delta > thresh) && (fft1024_1.read(32) > fftthresh32)) || ((delta > thresh) && (fft1024_1.read(33) > fftthresh33))) {
      maybeCar = true;
      if (maybeCar == true) {
        Serial.println("car possibly detected");
        //wake jetson
        digitalWrite(base, HIGH);
        delay(50);
        //turn wake pin off, jetson stays on
        digitalWrite(base, LOW);
        Serial.println("waiting for jetson confirmation");
        //timer start
        timeOfMaybe = millis();
        maybeCar = false;
        allIsQuite = false;
      }
    }
  } else if (allIsQuite == false) {
    Serial.println("car maybe approaching waiting for jetson confirmation");
    //looking for car affermation from jetson
    int currentState = digitalRead(startShort);
    //timer start once a car has possibly been detected to allow for a reset if a car doesn't drive over it
    waitingForJetson = millis();
    //if jetson sends postive car signal
    if (currentState != previousState) {
      Serial.println("recieved jetson confirmation");
      jetsonSeesCar = true;
    }

    if (jetsonSeesCar == true) {
      int dist = analogRead(irPin);
      Serial.println("IR begin");
      if (dist > 500) {
        //set the fire pin to fire to fire the R.O.C.K.
        //delay to ensure full discharge
        Serial.println("Launch");
        digitalWrite(13, HIGH);
        delay(5000);
        digitalWrite(13, LOW);
        Serial.println("Reset");
        //car has passed
        jetsonSeesCar = false;
        //should be terminate code but return to long range search
        t = 1;
        allIsQuite = true;
      }
    }
    //if the jetson takes to long/ not car is found reset and re calibrate.
    if (waitingForJetson - timeOfMaybe > reCalTime) {
      //recal
      Serial.println("Jetson did not find car");
      Serial.println("Rest and Recal");
      t = 1;
      //ensure set back to inital conditions
      jetsonSeesCar = false;
      allIsQuite = true;
      maybeCar = false;
    }
  }
}
