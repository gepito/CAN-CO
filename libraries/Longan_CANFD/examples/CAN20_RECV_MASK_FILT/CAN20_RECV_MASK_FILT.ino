/* 
  MCP2517/8 receive a CAN2.0 fram with Mask and Filter setting
  
  CAN Bus baudrare for CAN2.0 as below,
  
  CAN20_5KBPS
  CAN20_10KBPS
  CAN20_20KBPS
  CAN20_25KBPS
  CAN20_31K25BPS
  CAN20_33KBPS
  CAN20_40KBPS
  CAN20_50KBPS
  CAN20_80KBPS
  CAN20_83K3BPS
  CAN20_95KBPS
  CAN20_100KBPS
  CAN20_125KBPS
  CAN20_200KBPS
  CAN20_250KBPS
  CAN20_500KBPS
  CAN20_666KBPS
  CAN20_800KBPS
  CAN20_1000KBPS
  
  CAN FD Shield - https://www.longan-labs.cc/1030012.html
  CANBed FD - https://www.longan-labs.cc/1030009.html
*/

#include <SPI.h>
#include "mcp2518fd_can.h"

// pinS for CAN-FD Shield, 
//const int SPI_CS_PIN = 9;
//const int CAN_INT_PIN = 2;

// pinS for CANBed FD
const int SPI_CS_PIN = 17;
const int CAN_INT_PIN = 7;

mcp2518fd CAN(SPI_CS_PIN); // Set CS pin
                         
void setup() {
    Serial.begin(115200);
    while(!Serial);
    while (CAN_OK != CAN.begin(CAN20_500KBPS)) {             // init can bus : baudrate = 500k
        Serial.println("CAN init fail, retry...");
        delay(100);
    }
    Serial.println("CAN init ok!");
    
    // init_Filt_Mask(filter number, ext, filter, mask)
    // There're 32 set of filter/mask for MCP2517FD, filter number can be set to 0~31
    CAN.init_Filt_Mask(0, 0, 0, 0);     // get all standard frame 
    CAN.init_Filt_Mask(1, 1, 0, 0);     // get all extended frame
}


void loop() {
    unsigned char len = 0;
    unsigned char buf[8];

    if (CAN_MSGAVAIL == CAN.checkReceive())         // // check if data coming
    {         
    
        CAN.readMsgBuf(&len, buf);            // You should call readMsgBuff before getCanId
        unsigned long id = CAN.getCanId();
        unsigned char ext = CAN.isExtendedFrame();
        
        Serial.print(ext ? "GET EXTENDED FRAME FROM ID: 0X" : "GET STANDARD FRAME FROM ID: 0X");
        Serial.println(id, HEX);
        
        Serial.print("Len = ");
        Serial.println(len);
            // print the data
        for (int i = 0; i < len; i++) {
            Serial.print(buf[i]); 
            Serial.print("\t");
        }
        Serial.println();
    }
}

// END FILE
