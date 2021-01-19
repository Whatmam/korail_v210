/***********************************************************************/  
/*      PROJECT         :       RC500                                  */  
/*      FUNCTION        :       MFREG500.H                             */  
/*      VERSION         :       1.0                                    */  
/*      CPU             :       80C51-25MHz                            */  
/*      COMPLIER        :       ICC8051 4.20                           */  
/*      COMPANY         :       SST CO.,LTD                            */  
/*      PROGRAMMER      :       SONG, PYEONGRAE                        */  
/*      DATE            :       2000. 10. 08                           */  
/***********************************************************************/  

// PAGE 0
#define     RegPage               0x00    // Page Select Register
#define     RegCommand            0x01    // Command Register
#define     RegFIFOData           0x02    // FiFo Register
#define     RegPrimaryStatus      0x03    // Modem State/IRQ/ERR/LoHiAlert Reg
#define     RegFIFOLength         0x04    // Buffer lenght Register
#define     RegSecondaryStatus    0x05
#define     RegInterruptEn        0x06    // IRQ enable Register
#define     RegInterruptRq        0x07    // IRQ bits Register
// PAGE 1      Control and Status
#define     RegControl            0x09    // processor control
#define     RegErrorFlag          0x0A
#define     RegCollPos            0x0B
#define     RegTimerValue         0x0C
#define     RegCRCResultLSB       0x0D
#define     RegCRCResultMSB       0x0E
#define     RegBitFraming         0x0F
// PAGE 2      Transmitter and Coder Control
#define     RegTxControl          0x11
#define     RegCwConductance      0x12
#define     RegModConductance     0x13
#define     RegCoderControl       0x14
#define     RegModWidth           0x15
#define     RegModWidthSOF        0x16
#define     RegTypeBFraming       0x17 
// PAGE 3      Receiver and Decoder Control
#define     RegRxControl1         0x19
#define     RegDecoderControl     0x1A
#define     RegBitPhase           0x1B
#define     RegRxThreshold        0x1C
#define     RegBPSKDemControl     0x1D
#define     RegRxControl2         0x1E
#define     RegClockQControl      0x1F
// PAGE 4      RF-Timing and Channel Redundancy
#define     RegRxWait             0x21
#define     RegChannelRedundancy  0x22
#define     RegCRCPresetLSB       0x23
#define     RegCRCPresetMSB       0x24
#define     RegTimeSlotPeriod     0x25
#define     RegMfOutSelect        0x26
#define     RFU27                 0x27
// PAGE 5      FIFO, Timer and IRQ-Pin Configuration
#define     RegFIFOLevel          0x29
#define     RegTimerClock         0x2A
#define     RegTimerControl       0x2B
#define     RegTimerReload        0x2C
#define     RegIRqPinConfig       0x2D
#define     RFU2E                 0x2E
#define     RFU2F                 0x2F
// PAGE 6      RFU
#define     RFU31                 0x31
#define     RFU32                 0x32
#define     RFU33                 0x33
#define     RFU34                 0x34
#define     RFU35                 0x35
#define     RFU36                 0x36
#define     RFU37                 0x37
// PAGE 7      Test Control
#define     RFU39                 0x39
#define     RegTestAnaSelect      0x3A
#define     RFU3B                 0x3B
#define     RFU3C                 0x3C
#define     RegTestDigiSelect     0x3D
#define     RFU3E                 0x3E
#define     RegTestDigiAccess     0x3F


// PCD - COMMANDS
#define PCD_IDLE           0x00 // No action: cancel current command 
                                // or home state, respectively
#define PCD_WRITEE2        0x01 // Get data from FIFO and write it to the E2PROM
#define PCD_READE2         0x03 // Read data from E2PROM and put it into the 
                                // FIFO
#define PCD_LOADCONFIG     0x07 // Read data from E2PROM and initialise the 
                                // registers
#define PCD_LOADKEYE2      0x0B // Read a master key from the E2PROM and put 
                                // it into the master key buffer
#define PCD_AUTHENT1       0x0C // Perform the first part of the card 
                                // authentication using the Crypto1 algorithm.
                                // Remark: The master key is automatically taken 
                                // from 
                                // the master key buffer. this implies, that the
                                // command LoadKeyE2 has to be executed before 
                                // to use a certain key for card authentication
#define PCD_CALCCRC        0x12	// Activate the CRC-Coprocessor
                                // Remark: The result of the CRC calculation can
                                // be read from the register CRCResultXXX
#define PCD_AUTHENT2       0x14	// Perform the second part of the card 
                                // authentication using the Crypto1 algorithm.
#define PCD_RECEIVE        0x16 // Activate Receiver Circuitry. Before the 
                                // receiver actually starts, the state machine 
                                // waits until the time 
                                // configured in the register RcvWait has 
                                // passed.
                                // Remark: It is possible to read data from the 
                                // FIFO although Receive is active. Thus it is 
                                // possible to receive any number of bytes by 
                                // reading them from the FIFO in time.
#define PCD_LOADKEY        0x19 // Read a master key from the FIFO and put it 
                                // into the master key buffer
                                // Remark: The master key has to be prepared in
                                // a certain format. Thus, 12 byte have to be 
                                // passed to load a 6 byte master key
#define PCD_TRANSMIT       0x1A // Transmit data from FIFO to the card
                                // Remark: If data is already in the FIFO when 
                                // the command is activated, this data is 
                                // transmitted immediately. It is possible to 
                                // write data to the FIFO although Transmit 
                                // is active. Thus it is possible to transmitt 
                                // any number of bytes by writting them to the 
                                // FIFO in time. 
#define PCD_TRANSCEIVE     0x1E	// Transmit data from FIFO to the card and after 
                                // that automatically activates the receiver. 
                                // Before the receiver actually starts, the 
                                // STATE MACHINE waits until the time configured
                                //  in the register RcvWait has passed.
                                // Remark: This command is the combination of 
                                // Transmit and Receive
#define PCD_RESETPHASE     0x3F


