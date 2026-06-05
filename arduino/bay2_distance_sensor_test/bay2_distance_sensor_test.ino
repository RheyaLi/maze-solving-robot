// Bay 2 - Distance Sensors evidence sketch
// Goal:
// 1. Show the front distance sensor changes between "no surface" and
//    "surface present".
// 2. Show the front distance sensor can identify a surface around 5 cm away.
//
// Open Serial Monitor at 9600 baud and record readings for:
// - no surface / far away
// - surface at 5 cm
// - surface at 3 cm and 2 cm, if possible

const int FRONT_SENSOR_PIN = A4;

const int FRONT_LED_PIN = 14;

// Front sensor calibration from Bay 2 photos:
// P1 infinite ~= 32-35, P3 5 cm ~= 205-212, P4 3 cm ~= 227-236.
// For Bay 2 evidence, treat the 4-6 cm range as YES.
const int FRONT_PRESENT_THRESHOLD = 60;

const int FRONT_4_TO_6CM_LOW = 180;
const int FRONT_4_TO_6CM_HIGH = 226;

const unsigned long PRINT_INTERVAL_MS = 200;
unsigned long lastPrintAt = 0;

int readAveragedAnalog(int pin) {
  long total = 0;

  for (int i = 0; i < 10; i++) {
    total += analogRead(pin);
    delay(2);
  }

  return total / 10;
}

float rawToVoltage(int raw) {
  return raw * (5.0 / 1023.0);
}

bool isPresent(int raw, int threshold) {
  return raw >= threshold;
}

bool isIn4To6cmRange(int raw, int lowThreshold, int highThreshold) {
  return raw >= lowThreshold && raw <= highThreshold;
}

void printSensorLine(const char *name, int raw, int presentThreshold, int lowRange, int highRange) {
  Serial.print(name);
  Serial.print("_raw=");
  Serial.print(raw);

  Serial.print(" ");
  Serial.print(name);
  Serial.print("_voltage=");
  Serial.print(rawToVoltage(raw), 2);

  Serial.print(" ");
  Serial.print(name);
  Serial.print("_present=");
  Serial.print(isPresent(raw, presentThreshold) ? "YES" : "NO");

  Serial.print(" ");
  Serial.print(name);
  Serial.print("_range_4_to_6cm=");
  Serial.print(isIn4To6cmRange(raw, lowRange, highRange) ? "YES" : "NO");
}

void setup() {
  Serial.begin(9600);

  pinMode(FRONT_SENSOR_PIN, INPUT);
  pinMode(FRONT_LED_PIN, OUTPUT);

  Serial.println("Bay 2 distance sensor test started.");
  Serial.println("Move a surface to infinite/far, 6 cm, 5 cm, 4 cm, and 3 cm positions.");
  Serial.println("Front 4-6 cm YES range: raw 180 to 226.");
}

void loop() {
  const int frontRaw = readAveragedAnalog(FRONT_SENSOR_PIN);
  const bool frontInRange = isIn4To6cmRange(frontRaw, FRONT_4_TO_6CM_LOW, FRONT_4_TO_6CM_HIGH);

  digitalWrite(FRONT_LED_PIN, frontInRange ? HIGH : LOW);

  if (millis() - lastPrintAt >= PRINT_INTERVAL_MS) {
    lastPrintAt = millis();

    printSensorLine("front", frontRaw, FRONT_PRESENT_THRESHOLD, FRONT_4_TO_6CM_LOW, FRONT_4_TO_6CM_HIGH);
    Serial.println();
  }
}
