#include <Servo.h>

Servo dryServo;
Servo wetServo;

const int moistureSensorPin = A0;
const int dryServoPin = 9;
const int wetServoPin = 10;
const int pirSensorPin = 2;

int dryCal = 300;   // Sensor value when dry (lower value in simulation)
int wetCal = 700;   // Sensor value when wet (higher value in simulation)

bool isDryServoOn = false;
bool isWetServoOn = false;
bool pirAvailable = true;

void setup() {
  Serial.begin(9600);
  dryServo.attach(dryServoPin);
  wetServo.attach(wetServoPin);
  dryServo.write(0);
  wetServo.write(0);

  pinMode(pirSensorPin, INPUT);

  Serial.println("System Ready.");
}

int readMoisturePercent() {
  int sensorValue = analogRead(moistureSensorPin);
  sensorValue = constrain(sensorValue, dryCal, wetCal);
  int percent = map(sensorValue, dryCal, wetCal, 0, 100);  // Correct order
  return constrain(percent, 0, 100);
}

void loop() {
  bool motion = digitalRead(pirSensorPin) == HIGH || !pirAvailable;
  int moisturePercent = readMoisturePercent();

  Serial.print("Moisture: ");
  Serial.print(moisturePercent);
  Serial.print("% | Status: ");
  if (moisturePercent < 50) {
    Serial.print("DRY");
  } else {
    Serial.print("WET");
  }
  Serial.print(" | PIR: ");
  Serial.println(motion ? "Motion Detected" : "No Motion");

  if (motion) {
    if (moisturePercent < 50 && !isDryServoOn) {
      dryServo.write(60);
      isDryServoOn = true;
      Serial.println("Dry Servo ON");
    } else if (moisturePercent >= 50 && isDryServoOn) {
      dryServo.write(0);
      isDryServoOn = false;
      Serial.println("Dry Servo OFF");
    }

    if (moisturePercent >= 50 && !isWetServoOn) {
      wetServo.write(60);
      isWetServoOn = true;
      Serial.println("Wet Servo ON");
    } else if (moisturePercent < 50 && isWetServoOn) {
      wetServo.write(0);
      isWetServoOn = false;
      Serial.println("Wet Servo OFF");
    }
  } else {
    // No motion: turn off both
    if (isDryServoOn) {
      dryServo.write(0);
      isDryServoOn = false;
      Serial.println("Dry Servo OFF - No Motion");
    }
    if (isWetServoOn) {
      wetServo.write(0);
      isWetServoOn = false;
      Serial.println("Wet Servo OFF - No Motion");
    }
  }

  delay(500);
}
