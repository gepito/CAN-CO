/*************************************************************************************************
    OBD-II_PIDs TEST CODE
    LOOVEE @ JUN24, 2017

    Query
    send id: 0x7df
      dta: 0x02, 0x01, PID_CODE, 0, 0, 0, 0, 0

    Response
    From id: 0x7E9 or 0x7EA or 0x7EB
      dta: len, 0x41, PID_CODE, byte0, byte1(option), byte2(option), byte3(option), byte4(option)

    https://en.wikipedia.org/wiki/OBD-II_PIDs

    Input a PID, then you will get reponse from vehicle, the input should be end with '\n'
***************************************************************************************************/

#include <SPI.h>
#include "mcp2518fd_can.h"

// pin for CAN-FD Shield
const int SPI_CS_PIN = 9;
const int CAN_INT_PIN = 2;

// pin for CANBed FD
//const int SPI_CS_PIN = 17;
//const int CAN_INT_PIN = 7;

mcp2518fd CAN(SPI_CS_PIN); // Set CS pin

#define PID_ENGIN_PRM       0x0C
#define PID_VEHICLE_SPEED   0x0D
#define PID_COOLANT_TEMP    0x05

#define CAN_ID_PID          0x7DF

unsigned char PID_INPUT;
unsigned char getPid    = 0;

void set_mask_filt() {

    // init_Filt_Mask(filter number, ext, filter, mask)
    // There're 32 set of filter/mask for MCP2517FD, filter number can be set to 0~31
    CAN.init_Filt_Mask(0, 0, 0x7E8, 0x7FC);  
    
}

void sendPid(unsigned char __pid) {
    unsigned char tmp[8] = {0x02, 0x01, __pid, 0, 0, 0, 0, 0};
    Serial.print("SEND PID: 0x");
    Serial.println(__pid, HEX);
    CAN.sendMsgBuf(CAN_ID_PID, 0, 8, tmp);
}

void setup() {
    Serial.begin(115200);
    while(!Serial){};

    while (CAN_OK != CAN.begin(CANFD_500KBPS)) {             // init can bus : baudrate = 500k
        Serial.println("CAN init fail, retry...");
        delay(100);
    }
    Serial.println("CAN init ok!");
    set_mask_filt();
}


void loop() {
    taskCanRecv();
    taskDbg();

    if (getPid) {       // GET A PID
        getPid = 0;
        sendPid(PID_INPUT);
        PID_INPUT = 0;
    }
}

void taskCanRecv() {
    unsigned char len = 0;
    unsigned char buf[8];

    if (CAN_MSGAVAIL == CAN.checkReceive()) {                // check if get data
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        Serial.println("\r\n------------------------------------------------------------------");
        Serial.print("Get Data From id: 0x");
        Serial.println(CAN.getCanId(), HEX);
        for (int i = 0; i < len; i++) { // print the data
            Serial.print("0x");
            Serial.print(buf[i], HEX);
            Serial.print("\t");
        }
        Serial.println();
    }
}

void taskDbg() {
    while (Serial.available()) {
        char c = Serial.read();

        if (c >= '0' && c <= '9') {
            PID_INPUT *= 0x10;
            PID_INPUT += c - '0';

        } else if (c >= 'A' && c <= 'F') {
            PID_INPUT *= 0x10;
            PID_INPUT += 10 + c - 'A';
        } else if (c >= 'a' && c <= 'f') {
            PID_INPUT *= 0x10;
            PID_INPUT += 10 + c - 'a';
        } else if (c == '\n') { // END
            getPid = 1;
        }
    }
}

// END FILE
