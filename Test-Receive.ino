#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <Transfer.h>
Transfer transfer;    //tao doi tuong (giu nguyen)
ESP8266WiFiMulti WiFiMulti;

void handleEvent(WStype_t type, uint8_t * payload, size_t lenght)
{
  transfer.webSocketEvent(type, payload, lenght);
}


void setup() {
  Serial.begin(9600);
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(50);
  }


  WiFiMulti.addAP("DESKTOP-QCT2CAN 5897", "1234567890");
  //WiFi.disconnect();
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(50);
    Serial.println("Can't connect to WIFI. Try again!!");
  }


  transfer.init("192.168.137.119", 8080, "/Server_java/endpoint", ROLE_BOTH);
  transfer.initEvent(handleEvent); //khoi tao ham bat su kien (giu nguyen)
  delay(1000);

}

int type;
int goc_huong;
int goc_tam;
int culy ;

void loop() {
  transfer.loop();
  
  if (transfer.flagReceive) {
    type = transfer.getInt("type");
    goc_huong = transfer.getInt("tiaquet_huong");
    goc_tam = transfer.getInt("tiaquet_tam");
    culy = transfer.getInt("tiaquet_culy");
    transfer.resetFlagReceive();
  }


}
