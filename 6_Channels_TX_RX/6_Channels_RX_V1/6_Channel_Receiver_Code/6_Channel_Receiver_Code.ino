#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

const uint64_t pipeIn = 0xE8E8F0F0E1LL;

int ch_width_1 = 0, ch_width_2 = 0, ch_width_3 = 0;
int ch_width_4 = 0, ch_width_5 = 0, ch_width_6 = 0;

Servo ch1, ch2, ch3, ch4, ch5, ch6;

struct Signal {
  byte ch1;
  byte ch2;
  byte ch3;
  byte ch4;
  byte ch5;
  byte ch6;
};

Signal data;

RF24 radio(9, 10);
unsigned long lastRecvTime = 0;

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
  Serial.println("=== NRF24L01 Receiver ===");
  Serial.println("Initializing...");

  ch1.attach(3);
  ch2.attach(4);
  ch3.attach(5);
  ch4.attach(6);
  ch5.attach(7);
  ch6.attach(8);
  pinMode(2, OUTPUT);

  ResetData();

  if (!radio.begin()) {
    Serial.println("NRF24L01 not detected! Check wiring and power.");
    while (1);
  }

  radio.openReadingPipe(1, pipeIn);
  radio.setChannel(110);
  radio.setAutoAck(true);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();

  Serial.println("NRF24L01 initialized successfully!");
  delay(1000);
}

void recvData() {
  while (radio.available()) {
    radio.read(&data, sizeof(Signal));
    lastRecvTime = millis();
    Serial.print("Received -> ");
    Serial.print("CH1: "); Serial.print(data.ch1);
    Serial.print("  CH2: "); Serial.print(data.ch2);
    Serial.print("  CH3: "); Serial.print(data.ch3);
    Serial.print("  CH4: "); Serial.print(data.ch4);
    Serial.print("  CH5: "); Serial.print(data.ch5);
    Serial.print("  CH6: "); Serial.println(data.ch6);
  }
}

void loop() {
  recvData();

  unsigned long now = millis();
  if (now - lastRecvTime > 1000) {
    ResetData();
    Serial.println("Signal lost! Resetting data...");
  }

  ch_width_1 = map(data.ch1, 0, 255, 700, 2400);
  ch_width_2 = map(data.ch2, 0, 255, 700, 2400);
  ch_width_3 = map(data.ch3, 0, 255, 1000, 2000);
  ch_width_4 = map(data.ch4, 0, 255, 700, 2400);
  ch_width_5 = map(data.ch5, 0, 1, 1000, 2000);
  ch_width_6 = map(data.ch6, 0, 1, 1000, 2000);

  ch1.writeMicroseconds(ch_width_1);
  ch2.writeMicroseconds(ch_width_2);
  ch3.writeMicroseconds(ch_width_3);
  ch4.writeMicroseconds(ch_width_4);
  ch5.writeMicroseconds(ch_width_5);
  ch6.writeMicroseconds(ch_width_6);

  (ch_width_5 >= 1500) ? digitalWrite(2, HIGH) : digitalWrite(2, LOW);
}
