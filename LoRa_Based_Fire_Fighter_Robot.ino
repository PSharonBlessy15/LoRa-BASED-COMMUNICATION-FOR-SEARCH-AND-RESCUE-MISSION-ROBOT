#include <SPI.h>
#include <LoRa.h>

#define ss 5
#define rst 14
#define dio0 2
#define RELAY 13

#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33

#define REPORTING_PERIOD_MS     1000
uint32_t tsLastReport = 0;
int counter = 0;
char cmd = 's';
bool data_f;
int distance;

void getTFminiData(int* distance, int* strength)
{
  static char i = 0;
  char j = 0;
  int checksum = 0;
  static int rx[9];
  if (Serial2.available())
  {
    rx[i] = Serial2.read();
    if (rx[0] != 0x59)
    {
      i = 0;
    }
    else if (i == 1 && rx[1] != 0x59)
    {
      i = 0;
    }
    else if (i == 8)
    {
      for (j = 0; j < 8; j++)
      {
        checksum += rx[j];
      }
      if (rx[8] == (checksum % 256))
      {
        *distance = rx[2] + rx[3] * 256;
        *strength = rx[4] + rx[5] * 256;
      }
      i = 0;
    }
    else
    {
      i++;
    }
  }
}

void setup() {
  pinMode(RELAY, OUTPUT);
  pinMode(IN1, OUTPUT);
  digitalWrite(IN1,LOW);
  pinMode(IN2, OUTPUT);
  digitalWrite(IN2,LOW);
  pinMode(IN3, OUTPUT);
  digitalWrite(IN3,LOW);
  pinMode(IN4, OUTPUT);
  digitalWrite(IN4,LOW);
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(433E6)) {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received packet '");
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData);

      if(LoRaData == "f" && distance>20){
        digitalWrite(IN1,HIGH);
        digitalWrite(IN2,LOW);
        digitalWrite(IN3,LOW);
        digitalWrite(IN4,HIGH);
        
        
        Serial.println("Forward");
        }
      else if(LoRaData == "b"){
        digitalWrite(IN1,LOW);
        digitalWrite(IN2,HIGH);
        digitalWrite(IN3,HIGH);
        digitalWrite(IN4,LOW);        
        Serial.println("Back");
        }
      else if(LoRaData == "l"){
        digitalWrite(IN1,HIGH);
        digitalWrite(IN2,LOW);
        digitalWrite(IN3,HIGH);
        digitalWrite(IN4,LOW);
        Serial.println("Right");
        }
      else if(LoRaData == "r"){
        Serial.println("Left");
        digitalWrite(IN1,LOW);
        digitalWrite(IN2,HIGH);
        digitalWrite(IN3,LOW);
        digitalWrite(IN4,HIGH);

        }
      else{
        Serial.println("Stop");
        digitalWrite(IN1,LOW);
        digitalWrite(IN2,LOW);
        digitalWrite(IN3,LOW);
        digitalWrite(IN4,LOW);
        }  
     
    }
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
  if (Serial.available()) {
    cmd = Serial.read();
  }
  //---------------------------------------------Sender------------------------------------
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    Serial.println(analogRead(34));
    if (analogRead(34) < 1000) {
      digitalWrite(RELAY, HIGH);
    }
    else {
      digitalWrite(RELAY, LOW);
    }

    distance = 0;
    int strength = 0;
    Serial2.begin(115200);
    getTFminiData(&distance, &strength);
    while (!distance)
    {
      getTFminiData(&distance, &strength);
      if (distance)
      {
        Serial.println(distance);
        //delay(100);

      }
    }
    Serial2.end();
    Serial.print("Sending packet: ");
    Serial.println(distance);

    LoRa.beginPacket();
    LoRa.print(distance);
    LoRa.endPacket();
    tsLastReport = millis();
  }
}
