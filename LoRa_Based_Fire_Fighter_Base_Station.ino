#include <SPI.h>
#include <LoRa.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

#define REPORTING_PERIOD_MS     1000
uint32_t tsLastReport = 0;
int counter = 0; 
char cmd = 's';
bool data_f;
void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(433E6)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
     while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print("Distance : "); 
      Serial.println(LoRaData); 
    }

    //print RSSI of packet
    //Serial.print("' with RSSI ");
    //Serial.println(LoRa.packetRssi());
  }
if(Serial.available()){
  cmd = Serial.read();
  }
//---------------------------------------------Sender------------------------------------
if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    LoRa.beginPacket();
    LoRa.print(cmd);
    LoRa.endPacket();
    tsLastReport = millis();
  }
}
