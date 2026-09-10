#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define R_LED 5
#define G_LED 6
const uint64_t pipeOut = 0xE8E8F0F0E1LL;
RF24 radio(9, 10); // CE, CSN

struct Signal {
  byte ch1;
  byte ch2;
  byte ch3;
  byte ch4;
  byte ch5;
  byte ch6;
};

Signal data;

void ResetData() {
  data.ch1 = 127;
  data.ch2 = 127;
  data.ch3 = 0;
  data.ch4 = 127;
  data.ch5 = 0;
  data.ch6 = 0;
}

void setup() {
  Serial.begin(9600);
  Serial.println("=== NRF24L01 Transmitter ===");
  Serial.println("Initializing...");

  if (!radio.begin()) {
    Serial.println("NRF24L01 not detected! Check wiring and power.");
    while (1); // Stop here if not found

  }

  radio.openWritingPipe(pipeOut);
  radio.setChannel(110);
  radio.setAutoAck(true);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();

  ResetData();
  pinMode(R_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);
  Serial.println("NRF24L01 initialized successfully!");
  delay(1000);
}

int Border_Map(int val, int lower, int middle, int upper, bool reverse) {
  val = constrain(val, lower, upper);
  if (val < middle)
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  return (reverse ? 255 - val : val);
}

void loop() {
  data.ch1 = Border_Map(analogRead(A2), 0, 492, 1023, false);
  data.ch2 = Border_Map(analogRead(A3), 0, 521, 1023, false);
  data.ch3 = Border_Map(analogRead(A0), 0, 510, 1023, false);
  data.ch4 = Border_Map(analogRead(A1), 0, 520, 1023, false);
  data.ch5 = digitalRead(4);
  data.ch6 = digitalRead(3);

  bool success = radio.write(&data, sizeof(Signal));

  if (success) {
    Serial.print("Sent -> ");
    Serial.print("CH1: "); Serial.print(data.ch1);
    Serial.print("  CH2: "); Serial.print(data.ch2);
    Serial.print("  CH3: "); Serial.print(data.ch3);
    Serial.print("  CH4: "); Serial.print(data.ch4);
    Serial.print("  CH5: "); Serial.print(data.ch5);
    Serial.print("  CH6: "); Serial.println(data.ch6);
    digitalWrite(G_LED, HIGH);
    digitalWrite(R_LED, LOW);
  } else {
    Serial.println("Transmission failed!");
    digitalWrite(R_LED, HIGH);
    digitalWrite(G_LED, LOW);
  }

  delay(200);
}
