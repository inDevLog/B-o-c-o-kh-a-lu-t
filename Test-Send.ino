#include <Transfer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <SoftwareSerial.h>
#include "Wire.h"
#include <MPU6050_light.h>
#include <SimpleKalmanFilter.h>
MPU6050 mpu(Wire);

Transfer transfer;    //tao doi tuong (giu nguyen)
ESP8266WiFiMulti WiFiMulti;

void handleEvent(WStype_t type, uint8_t * payload, size_t lenght)
{
  transfer.webSocketEvent(type, payload, lenght);
}
SoftwareSerial Serial2_ESP(D7, D8); // RX, TX

void setup() {
  Serial.begin(9600);
  Serial2_ESP.begin(9600);

  for (uint8_t t = 4; t > 0; t--) {
//    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFiMulti.addAP("VNPT_TIEN", "dinhnhatquang");

  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(1000);
//    Serial.println("Can't connect to WIFI. Try again!!");
  }


  // khoi tao cho ket noi:
//  transfer.init("134.209.101.152", 8080, "/Server_java/endpoint", ROLE_RECEIVER);
  transfer.init("192.168.1.2", 8080, "/Server_java/endpoint", ROLE_SENDER);    //ROLE_BOTH the hien thiet bi 2 vai tro, vua nhan vua gui,
  // con ROLE_SENDER cho thiet bi gui, ROLE_RECEIVER cho thiet bi nhan
  transfer.initEvent(handleEvent); //khoi tao ham bat su kien (giu nguyen)
  delay(1000);


{
  Serial.begin(9600);
  Wire.begin();
  
  byte status = mpu.begin();
//  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
//  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
//  Serial.println("Done!\n");
}
}




SimpleKalmanFilter simpleKalmanFilter(0 , 1 , 0.01);
unsigned long timer = 0;


void loop() {
  mpu.update();
  
  if((millis()-timer)>10){ // print data every 10ms
  float vantocgoc_x = simpleKalmanFilter.updateEstimate(mpu.getGyroX());
  float vantocgoc_y = simpleKalmanFilter.updateEstimate(mpu.getGyroY());
  float vantocgoc_z = simpleKalmanFilter.updateEstimate(mpu.getGyroZ());
  
  float goc_x = simpleKalmanFilter.updateEstimate(mpu.getAngleX());
  float goc_y = simpleKalmanFilter.updateEstimate(mpu.getAngleY());
  float goc_z = simpleKalmanFilter.updateEstimate(mpu.getAngleZ());
  Serial.println(goc_x);
  transfer.loop();
  transfer.clearData();
  transfer.insertData("key_goc_x",goc_x);
  transfer.insertData("key_vtg_x",vantocgoc_x);
  transfer.sendData();
//  timer = millis(); 
  }
}
