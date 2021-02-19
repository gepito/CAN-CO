// MCP2517/8 receive a CAN frame with interrupt

#include <SPI.h>
#include "mcp2518fd_can.h"

// pin for CAN-FD Shield
//const int SPI_CS_PIN = 9;
//const int CAN_INT_PIN = 2;

// pin for CANBed FD
const int SPI_CS_PIN = 17;
const int CAN_INT_PIN = 7;

mcp2518fd CAN(SPI_CS_PIN); // Set CS pin


unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), CAN_ISR, FALLING); // start interrupt
    while (CAN_OK != CAN.begin(CAN_500KBPS)) {             // init can bus : baudrate = 500k
        Serial.println("CAN init fail, retry...");
        delay(100);
    }
    Serial.println("CAN init ok!");
}

void CAN_ISR() {
    flagRecv = 1;
}

void loop() {
    if (flagRecv) {
        // check if get data

        flagRecv = 0;                   // clear flag
        Serial.println("into loop");
        // iterate over all pending messages
        // If either the bus is saturated or the MCU is busy,
        // both RX buffers may be in use and reading a single
        // message does not clear the IRQ conditon.
        while (CAN_MSGAVAIL == CAN.checkReceive()) {
            // read data,  len: data length, buf: data buf
            Serial.println("checkReceive");
            CAN.readMsgBuf(&len, buf);

            // print the data
            for (int i = 0; i < len; i++) {
                Serial.print(buf[i]); Serial.print("\t");
            }
            Serial.println();
        }
    }
}

/*********************************************************************************************************
    END FILE
*********************************************************************************************************/
