/*
  ModbusRTU ESP8266/ESP32
  Simple slave example

  (c)2019 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266

  modified 13 May 2020
  by brainelectronics

  This code is licensed under the BSD New License. See LICENSE.txt for more info.
*/
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#include <ModbusRTU.h>
#define MODBUSRTU_DEBUG
const uint16_t receiver = 2;

ModbusRTU mb;

#if defined(ESP8266)
 #include <SoftwareSerial.h>
 // SoftwareSerial S(D1, D2, false, 256);

 // receivePin, transmitPin, inverse_logic, bufSize, isrBufSize
 // connect RX to D2 (GPIO4, Arduino pin 4), TX to D1 (GPIO5, Arduino pin 4)
 SoftwareSerial S(4, 5);
#endif

IRrecv irrecv(receiver);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'
int produccion;
int bateria;
int consumo;

void translateIR(){
  switch(results.value)
  {
  case 0xFFA25D: Serial.println("POWER"); break;
  case 0xFFE21D: Serial.println("FUNC/STOP"); break;
  case 0xFF629D: Serial.println("VOL+"); break;
  case 0xFF22DD: Serial.println("FAST BACK");    break;
  case 0xFF02FD: Serial.println("PAUSE");    break;
  case 0xFFC23D: Serial.println("FAST FORWARD");   break;
  case 0xFFE01F: Serial.println("DOWN");    break;
  case 0xFFA857: Serial.println("VOL-");    break;
  case 0xFF906F: Serial.println("UP");    break;
  case 0xFF9867: Serial.println("EQ");    break;
  case 0xFFB04F: Serial.println("ST/REPT");    break;
  case 0xFF6897: Serial.println("0");    break;
  case 0xFF30CF: produccion=produccion+200;    break;
  case 0xFF18E7: bateria=bateria+5; if(bateria>100) bateria=100;    break;
  case 0xFF7A85: consumo=consumo+200;    break;
  case 0xFF10EF: produccion=produccion-200; if(produccion<0) produccion=0;    break;
  case 0xFF38C7: bateria=bateria-5; if(bateria<0) bateria=0;    break;
  case 0xFF5AA5: consumo=consumo-200; if(consumo<0) consumo=0;    break;
  case 0xFF42BD: Serial.println("7");    break;
  case 0xFF4AB5: Serial.println("8");    break;
  case 0xFF52AD: Serial.println("9");    break;
  case 0xFFFFFFFF: Serial.println(" REPEAT");break;  
 
  default: 
    Serial.println(" other button   ");
  }// End Case

  mb.Hreg(1,produccion);
  mb.Hreg(2,bateria);
  mb.Hreg(3,consumo);
  
  printParam();
  delay(50); // Do not get immediate repeat
} //END translateIR

void printParam(){
  Serial.print("Producción actual: ");
  Serial.print(produccion);
  Serial.println("Wh");
  
  Serial.print("Estado de la batería: ");
  Serial.print(bateria);
  Serial.println("%");
  
  Serial.print("Consumo actual: ");
  Serial.print(consumo);
  Serial.println("Wh");
  
  Serial.println("-------------------------------------------------");
  Serial.println();
}

void setup() {
  pinMode(2,INPUT);
  pinMode(4,INPUT);
  Serial.begin(9600);
  Serial.println("IR Receiver Button Decode"); 
  irrecv.enableIRIn(); // Start the receiver
  produccion=1800;
  bateria=80;
  consumo=2300; 
  
 #if defined(ESP8266)
  S.begin(9600, SWSERIAL_8N1);
  mb.begin(&S);
 #elif defined(ESP32)
  Serial1.begin(9600, SERIAL_8N1);
  mb.begin(&Serial1);
 #else
  Serial1.begin(9600, SERIAL_8N1);
  mb.begin(&Serial1);
  mb.setBaudrate(9600);
 #endif
  mb.slave(1);

  mb.addHreg(1);
  mb.Hreg(1,produccion);
  mb.addHreg(2);
  mb.Hreg(2,bateria);
  mb.addHreg(3);
  mb.Hreg(3,consumo);
}

void loop() {
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    translateIR(); 
    irrecv.resume(); // receive the next value
  }  
  
  mb.task();
  yield();
}
