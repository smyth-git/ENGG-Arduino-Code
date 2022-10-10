#include "protothreads.h"
#include <Servo.h> 
#include <FastLED.h>


#define LED_STRIP_CEILING_PIN             2
#define LED_STRIP_FLOOR_PIN               3
#define TRAFFIC_LIGHT_1_PIN               4
#define TRAFFIC_LIGHT_2_PIN               5
#define SENSOR_CEILING_PIN                6
#define SENSOR_FLOOR_PIN                  7
#define SENSOR_FERRIS_WHEEL_1_PIN         8
#define SENSOR_FERRIS_WHEEL_2_PIN         9
#define SERVO_1_PIN                       10
#define SERVO_2_PIN                       11

#define NUM_STRIP_LEDS                    30
#define NUM_TRAFFIC_LIGHT_LEDS            2

pt ptLedsCeiling;
pt ptLedsFloor;
pt ptServos;
Servo Servo1;
Servo Servo2;

CRGB ceilingLeds[NUM_STRIP_LEDS];
CRGB floorLeds[NUM_STRIP_LEDS];
CRGB trafficLight1Leds[NUM_TRAFFIC_LIGHT_LEDS];
CRGB trafficLight2Leds[NUM_TRAFFIC_LIGHT_LEDS];

// Ceiling LEDs thread: checks for passing marbles + lights up ceiling LEDs
// Floor LEDs thread: checks for passing marbles + lights up floor LEDs
// Ferris wheels thread: checks for passing marbles + lights up traffic light LEDs + rotates threadmills following state machine logic

// sensorCheck should be running in a loop in the relative thread
bool sensorCheck(sensorPin) {
  // Returns true when the infrared sensor detects a passing marble
  if (digitalRead(sensorPin) == LOW) {
    return true;
  } else {
    return false;
  }
}

// LED strip animation function
void LEDStripAnimation (struct pt* pt, CRGB[] leds, int NUM_LEDS) {
  // One by one, the LEDs in the strip light up following the strip start to finish
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(50, 255, 0);    // set our current dot to green
    FastLED.show();
    PT_SLEEP(pt, 8);
    leds[i] = CRGB::Black;  // set our current dot to black before we continue
  }
  // One by one, the LEDs in the strip light up following the strip end to beginning
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    leds[i] = CRGB(50, 255, 0);    // set our current dot to green
    FastLED.show();
    PT_SLEEP(pt, 8);
    leds[i] = CRGB::Black;  // set our current dot to black before we continue
  }
  // Turn off all LEDs in the strip at the end of the animation
  turnOffLeds(leds, NUM_LEDS);
}

// Turn off all LEDs in the strip at the end of the animation
void turnOffLeds(CRGB[] leds, int NUM_LEDS) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
    FastLED.show();
  }
}

// Switches the traffic light state based on the boolean value isMoving
void trafficLightAnimation (CRGB[] leds, bool isMoving) {
  if (isMoving) {
    leds[0] = CRGB(50, 255, 0);
    leds[1] = CRGB::Black;
  } else {
    leds[1] = CRGB(255, 0, 0);
    leds[0] = CRGB::Black;
  }
  FastLED.show();
}

void calibrateLeds () {
  trafficLightAnimation(trafficLight1Leds, true);
  trafficLightAnimation(trafficLight2Leds, true);
  delay(100);
  trafficLightAnimation(trafficLight1Leds, false);
  trafficLightAnimation(trafficLight2Leds, false);
  
  // One by one, the LEDs in the strip light up following the strip start to finish
  for (int i = 0; i < NUM_STRIP_LEDS; i++) {
    ceilingLeds[i] = CRGB(0, 0, 255);    // set our current dot to blue
    FastLED.show();
    delay(20);
    ceilingLeds[i] = CRGB::Black;  // set our current dot to black before we continue
  }
  // One by one, the LEDs in the strip light up following the strip end to beginning
  for (int i = NUM_STRIP_LEDS - 1; i >= 0; i--) {
    ceilingLeds[i] = CRGB(0, 0, 255);    // set our current dot to blue
    FastLED.show();
    delay(20);
    ceilingLeds[i] = CRGB::Black;  // set our current dot to black before we continue
  }
  // Turn off all LEDs in the strip at the end of the animation
  turnOffLeds(ceilingLeds, NUM_STRIP_LEDS);

  // One by one, the LEDs in the strip light up following the strip start to finish
  for (int i = 0; i < NUM_STRIP_LEDS; i++) {
    floorLeds[i] = CRGB(0, 0, 255);    // set our current dot to blue
    FastLED.show();
    delay(20);
    floorLeds[i] = CRGB::Black;  // set our current dot to black before we continue
  }
  // One by one, the LEDs in the strip light up following the strip end to beginning
  for (int i = NUM_STRIP_LEDS - 1; i >= 0; i--) {
    floorLeds[i] = CRGB(0, 0, 255);    // set our current dot to blue
    FastLED.show();
    delay(20);
    floorLeds[i] = CRGB::Black;  // set our current dot to black before we continue
  }
  // Turn off all LEDs in the strip at the end of the animation
  turnOffLeds(floorLeds, NUM_STRIP_LEDS);
}

void errorLeds (CRGB[] leds, int NUM_LEDS) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(255, 0, 0);
    FastLED.show();
  }
}

// Checks if the sensors are not working or obstructed before the program starts
void checkSensors() {
  if(sensorCheck(SENSOR_CEILING_PIN) == true){
    errorLeds(ceilingLeds, NUM_STRIP_LEDS);
  }
  if(sensorCheck(SENSOR_FLOOR_PIN) == true){
    errorLeds(floorLeds, NUM_STRIP_LEDS);
  }
  if(sensorCheck(SENSOR_FERRIS_WHEEL_1_PIN) == true){
    errorLeds(trafficLight1Leds, NUM_TRAFFIC_LIGHT_LEDS);
  }
  if(sensorCheck(SENSOR_FERRIS_WHEEL_2_PIN) == true){
    errorLeds(trafficLight2Leds, NUM_TRAFFIC_LIGHT_LEDS);
  }
  delay(2000);
  turnOffLeds(ceilingLeds, NUM_STRIP_LEDS);
  turnOffLeds(floorLeds, NUM_STRIP_LEDS);
  turnOffLeds(trafficLight1Leds, NUM_TRAFFIC_LIGHT_LEDS);
  turnOffLeds(trafficLight2Leds, NUM_TRAFFIC_LIGHT_LEDS);
}

void alignFerrisWheels() {
  // Set both ferris wheels to 0 degrees so that it is alligned properly
  Servo1.write(0);
  Servo2.write(0);
}

void calibrate() {
  calibrateLeds();
  checkSensors();
  alignFerrisWheels();
}

// Thread used for the subsystems triggering the LED strips on the ceiling and on the floor of the box
int ledsStripThread(struct pt* pt, int sensorPin, CRGB[] leds, int NUM_LEDS) {
  PT_BEGIN(pt);

  // Loop forever
  for(;;) {
    if (sensorCheck(sensorPin) == true){
      LEDStripAnimation (pt, leds, NUM_LEDS);
      turnOffLeds(leds, NUM_LEDS);
    }
    PT_SLEEP(pt, 100);
  }

  PT_END(pt);
}

int ferrisWheelsThread(struct pt* pt, int ferrisWheel1Sensor, int ferrisWheel2Sensor, CRGB[] trafficLight1Leds, CRGB[] trafficLight2Leds, Servo Servo1, Servo Servo2) {
  PT_BEGIN(pt);
  int servo_State;
  bool hasMarble1 = false;
  bool hasMarble2 = false;
  // Loop forever
  for(;;) {
    switch(servo_State) {
      case 0:
        Servo1.write(0); // Servo 1 go to 0 degrees
        Servo2.write(0);
        hasMarble1 = false; //marble slots are now assumed empty
        hasMarble2 = false;
        break;
      case 1:
        Servo1.write(45); // Servo 1 go to 90 degrees
        Servo2.write(45);
        hasMarble1 = false;
        hasMarble2 = false;
        break;
      case 2:
        Servo1.write(90); // Servo 1 go to 180 degrees
        Servo2.write(90);
        hasMarble1 = false;
        hasMarble2 = false;
        break;
      case 3:
        Servo1.write(135); // Servo 1 go to 270 degrees
        Servo2.write(135);
        hasMarble1 = false;
        hasMarble2 = false;
        break;
      
      if(sensorCheck(ferrisWheel1Sensor)){
        hasMarble1 = true;
      }
      if(sensorCheck(ferrisWheel2Sensor)){
        hasMarble2 = true;
      }

      if (hasMarble1 == true && hasMarble2 == true) { //both sensors have detected marbles
        trafficLightAnimation(trafficLight1Leds, true);
        trafficLightAnimation(trafficLight2Leds, true);
        servo_State++; //increment servo state
        if (servo_State > 3) { 
          servo_State= 0; //reset servo state
        }
      } else if (hasMarble1 == false && hasMarble2 == true) {
        trafficLightAnimation(trafficLight1Leds, false);
        trafficLightAnimation(trafficLight2Leds, true);
      } else if (hasMarble1 == true && hasMarble2 == false) {
        trafficLightAnimation(trafficLight1Leds, true);
        trafficLightAnimation(trafficLight2Leds, false);
      } else {
        trafficLightAnimation(trafficLight1Leds, false);
        trafficLightAnimation(trafficLight2Leds, false);
      }
      PT_SLEEP(pt, 200);
    }
  PT_END(pt);
}

// the setup function runs once when you press reset or power the board
void setup() {
  PT_INIT(&ptLedsCeiling);
  PT_INIT(&ptLedsFloor);
  PT_INIT(&ptServos);

  FastLED.addLeds<WS2812, LED_STRIP_CEILING_PIN, GRB>(ceilingLeds, NUM_STRIP_LEDS);
  FastLED.addLeds<WS2812, LED_STRIP_FLOOR_PIN, GRB>(floorLeds, NUM_STRIP_LEDS);
  FastLED.addLeds<WS2812, TRAFFIC_LIGHT_1_PIN, GRB>(trafficLight1Leds, NUM_TRAFFIC_LIGHT_LEDS);
  FastLED.addLeds<WS2812, TRAFFIC_LIGHT_2_PIN, GRB>(trafficLight2Leds, NUM_TRAFFIC_LIGHT_LEDS);

  // Initialize digital pins
  pinMode(SENSOR_CEILING_PIN, INPUT);                
  pinMode(SENSOR_FLOOR_PIN, INPUT);                  
  pinMode(SENSOR_FERRIS_WHEEL_1_PIN, INPUT);        
  pinMode(SENSOR_FERRIS_WHEEL_2_PIN, INPUT);        
  Servo1.attach(SERVO_1_PIN);                       
  Servo2.attach(SERVO_2_PIN);   

  // Include initial setup of the box below: all led strips should be turned off, both traffic lights should have one red LED (use the animation with false as parameter), and the ferris wheel should be alligned properly.
  calibrate();
}

// the loop function runs over and over again forever
void loop() {
  PT_SCHEDULE(ledsStripThread(&ptLedsCeiling, SENSOR_CEILING_PIN, ceilingLeds, NUM_STRIP_LEDS));
  PT_SCHEDULE(ledsStripThread(&ptLedsFloor, LED_STRIP_FLOOR_PIN, floorLeds, NUM_STRIP_LEDS));
  PT_SCHEDULE(ferrisWheelsThread(&ptServos, SENSOR_FERRIS_WHEEL_1_PIN, SENSOR_FERRIS_WHEEL_2_PIN, trafficLight1Leds, trafficLight2Leds, Servo1, Servo2));
}
