#include <LedControl.h>
#include <Servo.h> // include servo library to use its related functions

#define MAX_CLOCK_PIN 3
#define MAX_LOAD_PIN 4
#define MAX_DATA_PIN 2

int LaunchButtonPin = 13;
int ClearButtonPin = 8;

int AngleDownButtonPin = 6;
int AngleUpButtonPin = 7;

int SolenoidPin = 5;

LedControl lc = LedControl(MAX_DATA_PIN, MAX_CLOCK_PIN, MAX_LOAD_PIN, 1);

int SERVO_PIN = 9;

int SERVO_OFFSET = 3;
int SERVO_INCREMENT = 5;
int SERVO_MIN = 0 + SERVO_OFFSET;
int SERVO_MAX = 95;

int servo_angle = SERVO_MIN;

int launchCount = 0;

int FIRING_DELAY = 2000;
int FIRING_SOLENOID_TIME = 15;

bool firingEnabled = false;
unsigned long lastFiringTime = 0;
unsigned long lastSensorReadTime = 0;  
unsigned long lastAngleTime = 0;

Servo MG995_Servo;  // Define an instance of of Servo with the name of "MG995_Servo"

void setup() 
{

  pinMode(LaunchButtonPin, INPUT);
  pinMode(ClearButtonPin, INPUT);
  pinMode(AngleDownButtonPin, INPUT);
  pinMode(AngleUpButtonPin, INPUT);
  pinMode(SolenoidPin, OUTPUT);

  Serial.begin(9600); 

  lc.shutdown(0,false);   // The MAX72XX is in power-saving mode on startup, we have to do a wakeup call
  lc.setIntensity(0,5);
  lc.clearDisplay(0);

  MG995_Servo.attach(SERVO_PIN);  // Connect D6 of Arduino with PWM signal pin of servo motor
  MG995_Servo.write(servo_angle);

  updateCountDisplay();
}

unsigned long currentTime = millis();

char s[3];
void updateCountDisplay() {
  sprintf(s, "%3d", launchCount);
  for (int i=0; i<3; i++) lc.setChar(0, i, s[i], false);
}

void loop() {

  currentTime = millis();

  if (currentTime - lastAngleTime > 350) {
    
    if (digitalRead(AngleDownButtonPin) == HIGH) {
      Serial.println("DOWN");
      servo_angle = servo_angle + SERVO_INCREMENT;
      servo_angle = min(SERVO_MAX, max(SERVO_MIN, servo_angle));
      MG995_Servo.write(servo_angle);
      lastAngleTime = currentTime;
    }

    if (digitalRead(AngleUpButtonPin) == HIGH) {
      Serial.println("UP");
      servo_angle = servo_angle - SERVO_INCREMENT;
      servo_angle = min(SERVO_MAX, max(SERVO_MIN, servo_angle));
      MG995_Servo.write(servo_angle);
      lastAngleTime = currentTime;
    }
  }

  if (currentTime - lastFiringTime >  FIRING_SOLENOID_TIME && !digitalRead(ClearButtonPin)) {
    digitalWrite(SolenoidPin, LOW);
  }

  if (currentTime - lastFiringTime >  FIRING_DELAY) {
    firingEnabled = true;
  }
  
  if (firingEnabled && digitalRead(LaunchButtonPin) == HIGH) {

    firingEnabled = false;

    launchCount = launchCount + 1;
    
    lastFiringTime = currentTime;
    
    digitalWrite(SolenoidPin, HIGH);
    Serial.println("Button Pressed");
    updateCountDisplay();
  }

  if (digitalRead(ClearButtonPin) == HIGH && firingEnabled) {
    launchCount = 0;
    updateCountDisplay();
    digitalWrite(SolenoidPin, HIGH);
  }
}