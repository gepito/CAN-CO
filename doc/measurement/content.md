[three-frames.jpg ](three-frames.jpg )

Three consecutive frames generated:

```
    // TxAttempts = 0 : no retransmission
    // TxAttempts = 1 : 3 attempts
    // TxAttempts = 2 : unlimited attempts
	txConfig.TxAttempts = 1;

    CAN.sendMsgBuf(0x01, 0, CANFD::len2dlc(MAX_DATA_SIZE), stmp);
    stmp[0]++;
    CAN.sendMsgBuf(0x01, 0, CANFD::len2dlc(MAX_DATA_SIZE), stmp);
    stmp[0]++;
    CAN.sendMsgBuf(0x01, 0, CANFD::len2dlc(MAX_DATA_SIZE), stmp);
``` 

Only one frame is transmitted through retransmission attempts.

[three-frames-noretrans.psdata ](three-frames-noretrans.psdata)

Three consecutive frames generated:

```
    txConfig.TxAttempts = 0;


    CAN.sendMsgBuf(0x01, 0, CANFD::len2dlc(MAX_DATA_SIZE), stmp);
    stmp[0]++;
    CAN.sendMsgBuf(0x01, 0, CANFD::len2dlc(MAX_DATA_SIZE), stmp);
    stmp[0]++;
    CAN.sendMsgBuf(0x01, 0, CANFD::len2dlc(MAX_DATA_SIZE), stmp);
``` 

Only one frame is transmitted through new message retriggers transmission (of the original frame in the que)

