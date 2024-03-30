[three-frames.jpg ](three-frames.jpg )

Three consecutive frames generated:

```
    CAN.sendMsgBuf(0x01, 0, CANFD::len2dlc(MAX_DATA_SIZE), stmp);
    stmp[0]++;
    CAN.sendMsgBuf(0x01, 0, CANFD::len2dlc(MAX_DATA_SIZE), stmp);
    stmp[0]++;
    CAN.sendMsgBuf(0x01, 0, CANFD::len2dlc(MAX_DATA_SIZE), stmp);
``` 

Only one frame is transmitted through retransmission attempts.
