#include <Arduino.h>
#include "secrets.h"


Sring uniqDeviceID();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115.200) ;
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(5000) ;
  Serial.println("loop every 5s") ;
}

String uniqDeviceID()(
  uint64_t chipId = ESP.getEfuseMac();
  char id[32];
  sprintf(id,sizeof(id), "esp32-%04X%08X", (uint16_t)(chipId>>32), (uint32_t)chipId);
)