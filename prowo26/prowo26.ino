#include "INA219.h"

const int IN1 = 2;
const int IN2 = 3;
const int ENA = 4;
const int IN3 = 5;
const int IN4 = 6;
const int ENB = 7;
const int BUTTON = A0;
const int STICK = A1;

INA219 INA(0x40);

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  digitalWrite(ENA, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  Serial.begin(9600);

  while (!Serial)
    ;

  Wire.begin();

  if (!INA.begin()) {
    Serial.println("could not locate ina219 module");
    while (1) { delay(10); }
  }


  INA.setMaxCurrentShunt(5, 0.100);

  Serial.println("ready, waiting for button to start calibration...");

  calibrate(IN1, IN2, ENA);

  calibrate(IN3, IN4, ENB);
}

void forward(int pin1, int pin2, int en,int speed) {
  digitalWrite(en, LOW);
  delay(10);
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
  analogWrite(en, speed);
}

void backward(int pin1, int pin2, int en, int speed) {
  digitalWrite(en, LOW);
  delay(10);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
  analogWrite(en, speed);
}

void stopMotor(int pin1, int pin2, int en) {
  digitalWrite(en, LOW);
  delay(10);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
}

void runSequence() {
  forward(IN1, IN2, ENA, 255);
  delay(300);

  float busVoltage = INA.getBusVoltage();

  // Read shunt voltage (in mV)
  float shuntVoltage = INA.getShuntVoltage_mV();

  // Read current (in Amps)
  float current = INA.getCurrent_mA() / 1000.0;  // Convert mA to A

  // Calculate power (in Watts)
  float power = INA.getPower_mW() / 1000.0;  // Convert mW to W

  // Print all measurements
  Serial.println("------------------------");
  Serial.print("Bus Voltage:   ");
  Serial.print(busVoltage, 3);
  Serial.println(" V");

  Serial.print("Shunt Voltage: ");
  Serial.print(shuntVoltage, 3);
  Serial.println(" mV");

  Serial.print("Current:       ");
  Serial.print(current, 3);
  Serial.println(" A");

  Serial.print("Power:         ");
  Serial.print(power, 3);
  Serial.println(" W");

  stopMotor(IN1, IN2, ENA);
  delay(100);

  busVoltage = INA.getBusVoltage();

  // Read shunt voltage (in mV)
  shuntVoltage = INA.getShuntVoltage_mV();

  // Read current (in Amps)
  current = INA.getCurrent_mA() / 1000.0;  // Convert mA to A

  // Calculate power (in Watts)
  power = INA.getPower_mW() / 1000.0;  // Convert mW to W

  // Print all measurements
  Serial.println("------------------------");
  Serial.print("Bus Voltage:   ");
  Serial.print(busVoltage, 3);
  Serial.println(" V");

  Serial.print("Shunt Voltage: ");
  Serial.print(shuntVoltage, 3);
  Serial.println(" mV");

  Serial.print("Current:       ");
  Serial.print(current, 3);
  Serial.println(" A");

  Serial.print("Power:         ");
  Serial.print(power, 3);
  Serial.println(" W");

  backward(IN1, IN2, ENA, 150);
  delay(150);

  busVoltage = INA.getBusVoltage();

  // Read shunt voltage (in mV)
  shuntVoltage = INA.getShuntVoltage_mV();

  // Read current (in Amps)
  current = INA.getCurrent_mA() / 1000.0;  // Convert mA to A

  // Calculate power (in Watts)
  power = INA.getPower_mW() / 1000.0;  // Convert mW to W

  // Print all measurements
  Serial.println("------------------------");
  Serial.print("Bus Voltage:   ");
  Serial.print(busVoltage, 3);
  Serial.println(" V");

  Serial.print("Shunt Voltage: ");
  Serial.print(shuntVoltage, 3);
  Serial.println(" mV");

  Serial.print("Current:       ");
  Serial.print(current, 3);
  Serial.println(" A");

  Serial.print("Power:         ");
  Serial.print(power, 3);
  Serial.println(" W");

  stopMotor(IN1, IN2, ENA);
  delay(1000);

  busVoltage = INA.getBusVoltage();

  // Read shunt voltage (in mV)
  shuntVoltage = INA.getShuntVoltage_mV();

  // Read current (in Amps)
  current = INA.getCurrent_mA() / 1000.0;  // Convert mA to A

  // Calculate power (in Watts)
  power = INA.getPower_mW() / 1000.0;  // Convert mW to W

  // Print all measurements
  Serial.println("------------------------");
  Serial.print("Bus Voltage:   ");
  Serial.print(busVoltage, 3);
  Serial.println(" V");

  Serial.print("Shunt Voltage: ");
  Serial.print(shuntVoltage, 3);
  Serial.println(" mV");

  Serial.print("Current:       ");
  Serial.print(current, 3);
  Serial.println(" A");

  Serial.print("Power:         ");
  Serial.print(power, 3);
  Serial.println(" W");

  delay(1000);

  forward(IN3, IN4, ENB, 255);

  delay(5000);

  stopMotor(IN3, IN4, ENB);

  delay(100);

  backward(IN3, IN4, ENB, 255);

  delay(5000);

  stopMotor(IN3, IN4, ENB);
}

void calibrate(int pin1, int pin2, int en) {
  Serial.println("\n=== STARTING CALIBRATION ===");
  Serial.println("Motor will run forward until it hits resistance (800mA threshold)");
  Serial.println("Make sure there's an obstacle/wall ahead!");
  delay(2000);  // Give user time to read
  
  unsigned long startTime = millis();
  unsigned long calibrationTime = 0;
  float maxCurrent = 0;
  bool resistanceDetected = false;
  
  const float RESISTANCE_THRESHOLD = 300.0;  // mA
  const unsigned long TIMEOUT = 10000;  // 10 second safety timeout
  const unsigned long RESISTANCE_DURATION = 100;  // Must see high current for 100ms
  
  unsigned long resistanceStartTime = 0;
  
  Serial.println("Starting motor...");
  forward(pin1, pin2, en, 255);  // Full speed
  delay(100);  // Let motor spin up
  
  startTime = millis();  // Start timing AFTER spin-up
  
  while (!resistanceDetected) {
    float current = INA.getCurrent_mA();
    
    // Track max current seen
    if (current > maxCurrent) {
      maxCurrent = current;
    }
    
    // Print current every 100ms for debugging
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 100) {
      Serial.print("Current: ");
      Serial.print(current, 1);
      Serial.print(" mA  |  Time: ");
      Serial.print(millis() - startTime);
      Serial.println(" ms");
      lastPrint = millis();
    }
    
    // Check if we've hit resistance
    if (current >= RESISTANCE_THRESHOLD) {
      if (resistanceStartTime == 0) {
        resistanceStartTime = millis();
        Serial.println(">>> Resistance detected! Confirming...");
      }
      else if (millis() - resistanceStartTime >= RESISTANCE_DURATION) {
        // Sustained high current - we've definitely hit something
        resistanceDetected = true;
        calibrationTime = millis() - startTime;
      }
    }
    else {
      // Current dropped below threshold - reset
      resistanceStartTime = 0;
    }
    
    // Safety timeout
    if (millis() - startTime > TIMEOUT) {
      Serial.println("\n!!! CALIBRATION TIMEOUT !!!");
      Serial.println("Motor never hit resistance");
      Serial.print("Max current seen: ");
      Serial.print(maxCurrent, 1);
      Serial.println(" mA");
      Serial.println("Check: Is there an obstacle? Is threshold too high?");
      stopMotor(pin1, pin2, en);
      return;
    }
    
    delay(10);  // Small delay
  }
  
  stopMotor(pin1, pin2, en);
  
  // Print calibration results
  Serial.println("\n=== CALIBRATION COMPLETE ===");
  Serial.print("Time to obstacle: ");
  Serial.print(calibrationTime);
  Serial.println(" ms");
  Serial.print("Peak current at impact: ");
  Serial.print(maxCurrent, 1);
  Serial.println(" mA");
  Serial.print("Distance at full speed (255 PWM): ");
  Serial.print(calibrationTime);
  Serial.println(" ms of travel");
  
  // You can store this value globally
  // calibratedTravelTime = calibrationTime;
  
  Serial.println("\nNow you can use this to calculate travel distances!");
  Serial.println("=============================\n");
  
  delay(2000);
}

// Optional: Add a global variable to store the calibrated time
unsigned long calibratedTravelTime = 0;

// Function to travel a percentage of calibrated distance
void travelDistance(float percentage) {
  if (calibratedTravelTime == 0) {
    Serial.println("ERROR: Not calibrated yet!");
    return;
  }
  
  unsigned long travelTime = (unsigned long)(calibratedTravelTime * percentage / 100.0);
  
  Serial.print("Traveling ");
  Serial.print(percentage);
  Serial.print("% of calibrated distance (");
  Serial.print(travelTime);
  Serial.println(" ms)");
  
  forward(IN1, IN2, ENA, 255);
  delay(travelTime);
  stopMotor(IN1, IN2, ENA);
}

void loop() {
  int val = analogRead(STICK);
  if (digitalRead(BUTTON) == LOW || val < 200 || val > 800) {
    delay(20);

    int val = analogRead(STICK);

    if (digitalRead(BUTTON) == LOW) {
      runSequence();
      while (digitalRead(BUTTON) == LOW)
        delay(20);
    }
    if (val < 200) {
      forward(IN3, IN4, ENB, 255);

      while (analogRead(STICK) < 200)
        delay(20);
    } else if (val > 800) {
      backward(IN3, IN4, ENB, 255);

      while (analogRead(STICK) > 800)
        delay(20);
    } else {
      stopMotor(IN3, IN4, ENB);
    }
  }
}