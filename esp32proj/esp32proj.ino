#include <HardwareSerial.h>

// --- Serial console bridge (UART2) ---
HardwareSerial ConsoleSerial(2);   // TX2=17, RX2=16

// --- Battery data link (UART1) ---
HardwareSerial BatterySerial(1);   // TX1=4, RX1=5

const unsigned long BAUD = 115200;

// Battery monitor
const int battPin = 34;
const float R1 = 10000.0, R2 = 10000.0, vRef = 3.3, maxADC = 4095.0;

void setup() {
  Serial.begin(BAUD);                               // USB – phone/PC
  ConsoleSerial.begin(BAUD, SERIAL_8N1, 16, 17);    // UART2 – Pi console
  BatterySerial.begin(BAUD, SERIAL_8N1, 5, 4);      // UART1 – battery data
  analogReadResolution(12);
}

void loop() {
  // ---- Console bridge (phone ↔ Pi) ----
  while (Serial.available()) {
    ConsoleSerial.write(Serial.read());
  }
  while (ConsoleSerial.available()) {
    Serial.write(ConsoleSerial.read());
  }

  // ---- Battery request handler ----
  if (BatterySerial.available()) {
    String cmd = BatterySerial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "BAT?") {
      sendBattery();
    }
  }
}

void sendBattery() {
  int raw = analogRead(battPin);
  float vAdc = (raw / maxADC) * vRef;
  float vBat = vAdc / (R2 / (R1 + R2));
  float pct = (vBat - 3.0) / (4.2 - 3.0) * 100;
  if (pct > 100) pct = 100;
  if (pct < 0)   pct = 0;

  BatterySerial.print("BAT:");
  BatterySerial.print(vBat, 2);
  BatterySerial.print(",");
  BatterySerial.println(pct, 0);
}
