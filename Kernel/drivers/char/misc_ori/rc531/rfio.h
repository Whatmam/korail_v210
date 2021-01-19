//	@(#)rfio.h	0.2f	2014/07/08	(SOLIDTEK)

/* PAGE 0      Command and Status   */
#define RegPage               	0x00    
#define RegCommand            	0x01     
#define RegFIFOData           	0x02 
#define RegPrimaryStatus      	0x03    
#define RegFIFOLength         	0x04   
#define RegSecondaryStatus    	0x05 
#define RegInterruptEn        	0x06     
#define RegInterruptRq        	0x07      
/* PAGE 1      Control and Status   */
#define RegControl            	0x09      
#define RegErrorFlag          	0x0A  
#define RegCollpos            	0x0B
#define RegTimerValue         	0x0C 
#define RegCRCResultLSB       	0x0D 
#define RegCRCResultMSB       	0x0E
#define RegBitFraming         	0x0F
/* PAGE 2      Transmitter and Coder Control    */
#define RegTxControl          	0x11
#define RegCwConductance      	0x12 
#define RegModConductance     	0x13
#define RegCoderControl       	0x14
#define RegModWidth           	0x15 
#define RFU16                 	0x16 
#define RegTypeBFraming       	0x17 
/* PAGE 3      Receiver and Decoder Control     */
#define RegRxControl1         	0x19 
#define RegDecoderControl     	0x1A 
#define RegBitPhase           	0x1B 
#define RegRxThreshold        	0x1C 
#define RegBPSKDemControl     	0x1D 
#define RegRxControl2         	0x1E 
#define RegClockQControl      	0x1F 
/* PAGE 4      RF-Timing and Channel Redundancy */
#define RegRxWait             	0x21 
#define RegChannelRedundancy	0x22 
#define RegCRCPresetLSB       	0x23 
#define RegCRCPresetMSB       	0x24 
#define RFU25                 	0x25 
#define RegMfOutSelect        	0x26 
#define RFU27                 	0x27 
/* PAGE 5      FIFO, Timer and IRQ-Pin Configuration    */
#define RegFIFOLevel          	0x29 
#define RegTimerClock         	0x2A
#define RegTimerControl       	0x2B 
#define RegTimerReload        	0x2C 
#define RegIRqPinConfig       	0x2D
#define RFU2E                 	0x2E
#define RFU2F                 	0x2F
/* PAGE 6      RFU  */
#define RFU31                 	0x31
#define RFU32                 	0x32
#define RFU33                 	0x33
#define RFU34                 	0x34
#define RFU35                 	0x35
#define RFU36                 	0x36
#define RFU37                 	0x37
/* PAGE 7      Test Control     */
#define RFU39                 	0x39 
#define RegTestAnaSelect      	0x3A
#define RFU3B                 	0x3B   
#define RFU3C                 	0x3C   
#define RegTestDigiSelect     	0x3D  
#define RFU3E                 	0x3E   
#define RFU3F		        0x3F

//RegControl	(0x09)
#define	StandBy			5
#define	PowerDown		4
#define	Crypt1On		3
#define	TStopNow		2
#define	TStartNow		1
//#define FlushFIFO		0

//RegBPSKDemControl	(0x1D)
#define	NoRxSOF			7
#define	NoRxEGT			6
#define	NoRxEOF			5
#define	FilterAmpDet		4
#define	TauD1			3
#define	TauD0			2
#define	TauB1			1
#define	TauB0			0

//RegChannelRedundancy	(0x22)
#define	CRC3309			5
#define	CRC8			4
#define	RxCRCEn			3
#define	TxCRCEn			2
#define	ParityOdd		1
#define	ParityEn		0


/* default FIFO length */
#define DEF_FIFO_LENGTH 	64 

/* RC531 PCD - COMMANDS  */
#define PCD_IDLE           	0x00 
#define PCD_WRITEE2        	0x01 
#define PCD_READE2         	0x03 
#define PCD_LOADCONFIG     	0x07 
#define PCD_LOADKEYE2      	0x0B 
#define PCD_AUTHENT1       	0x0C 
#define PCD_CALCCRC        	0x12	
#define PCD_AUTHENT2       	0x14	
#define PCD_RECEIVE        	0x16 
#define PCD_LOADKEY        	0x19 
#define PCD_TRANSMIT       	0x1A 
#define PCD_TRANSCEIVE     	0x1E	
#define PCD_RESETPHASE     	0x3F 

/* P I C C - C O M M A N D S        */
#define PICC_REQSTD        	0x26	/* request idle             */
#define PICC_REQALL        	0x52	/* request all              */

#define PICC_ANTICOLL1     	0x93	/* anticollision level 1    */
#define PICC_ANTICOLL2     	0x95	/* anticollision level 2    */
#define PICC_ANTICOLL3     	0x97	/* anticollision level 3    */
#define PICC_AUTHENT1A     	0x60	/* authentication step 1    */
#define PICC_AUTHENT1B     	0x61	/* authentication step 2    */
#define PICC_READ          	0x30	/* read block               */
#define PICC_WRITE         	0xA0	/* write block              */
#define PICC_DECREMENT     	0xC0    /* decrement value          */
#define PICC_INCREMENT     	0xC1    /* increment value          */
#define PICC_RESTORE       	0xC2    /* restore command code     */
#define PICC_TRANSFER      	0xB0    /* transfer command code    */
#define PICC_HALT          	0x50	/* halt                     */

/* T Y P E  B  -  C O M M A N D S   */
#define PICC_REQBSTD       	0x00	/* request normal   	*/
#define PICC_REQBWUP       	0x08	/* request wakeup   	*/

/* ISO14443     */
#define TYPEA			0x41	/* ISO14443A('A')		*/
#define TYPEB			0x42	/* ISO14443B('B'		*/
#define TYPEAB			0x43	/* ISO14443A & ISO14443B ('C')	*/
#define MIFARE			0x4D	/* Mifare('M')			*/
#define TYPES			0x53	/* SRT512('S')			*/ 
#define TYPEAS			0x44	/* ISO14443A & SRT512 ('D')	*/ 
#define TYPEBS			0x45	/* ISO14443B & SRT512 ('E')	*/ 

#define B_TYPEA			0x01	/* Mifare & ISO14443A		*/
#define B_TYPEB			0x02	/* ISO14443B			*/
#define B_TYPES			0x04	/* SRT512			*/
#define B_TYPEAB		(B_TYPEA | B_TYPEB)	/* 0x03	- Type-A  & Type-B        */
#define B_TYPEAS		(B_TYPEA | B_TYPES)	/* 0x05	- Type-A &  SRT512        */ 
#define B_TYPEBS		(B_TYPEB | B_TYPES)	/* 0x06	- Type-B & SRT512         */
#define B_TYPEABS		(B_TYPEA | B_TYPEB | B_TYPES) /* 0x07 - Type-A&B & SRT512 */ 

#define T_TYPEA			0x10	/* Type-A			*/
#define T_TYPEB			0x20	/* Type-B			*/
#define T_TYPES			0x40	/* SRT512			*/
#define T_TYPEAB		(T_TYPEA | T_TYPEB)	/* 0x03	- Type-A & Type-B        */
#define T_TYPEAS		(T_TYPEA | T_TYPES)	/* 0x05	- Type-A & SRT512 	  */ 
#define T_TYPEBS		(T_TYPEB | T_TYPES)	/* 0x06	- Type-B & SRT512 	  */
#define T_TYPEABS		(T_TYPEA | T_TYPEB | T_TYPES) /* 0x07 - Type-A&B & SRT512 */ 

/* SRT512		*/
#define SRT512_INITIATE_L	0x06	
#define SRT512_INITIATE_H	0x00	
#define SRT512_READ_BLOCK	0x08	
#define SRT512_WRITE_BLOCK	0x09	
#define SRT512_GET_UID		0x0B	
#define SRT512_SELECT		0x0E	

/* Mifare Error Codes   */
#define MI_OK                  	0
#define MI_CHK_OK               0
#define MI_CRC_ZERO             0
#define MI_CRC_NOTZERO          1

#define MI_NOTAGERR             (2)
#define MI_CHK_FAILED           (3)
#define MI_CRCERR               (4)
#define MI_CHK_COMPERR          (5)
#define MI_EMPTY                (6)
#define MI_AUTHERR              (7)
#define MI_PARITYERR            (8)
#define MI_CODEERR              (9)
#define MI_SERNRERR             (10)
#define MI_KEYERR               (11)
#define MI_NOTAUTHERR           (12)
#define MI_BITCOUNTERR          (13)
#define MI_BYTECOUNTERR         (14)
#define MI_IDLE                 (15)
#define MI_TRANSERR             (16)
#define MI_WRITEERR             (17)
#define MI_INCRERR              (18)
#define MI_DECRERR              (19)
#define MI_READERR              (20)
#define MI_OVFLERR              (21)
#define MI_POLLING              (22)
#define MI_FRAMINGERR           (23)
#define MI_ACCESSERR            (24)
#define MI_UNKNOWN_COMMAND      (25)
#define MI_COLLERR              (26)
#define MI_RESETERR             (27)
#define MI_INITERR              (28)
#define MI_INTERFACEERR         (29)
#define MI_ACCESSTIMEOUT        (30)
#define MI_NOBITWISEANTICOLL    (31)
#define MI_QUIT                 (32)
#define MI_RECBUF_OVERFLOW      (33)
#define MI_SENDBYTENR           (34)
#define MI_SENDBUF_OVERFLOW     (35)
#define MI_BAUD_NOT_SUPPORTED   (36)
#define MI_SAME_BAUD_REQUIRED   (37)
#define MI_WRONG_PARA_VALUE     (38)
#define MI_BREAK                (39)
#define MI_NY_IMPLEMENTED       (40)
#define MI_NO_MFRC              (41)
#define MI_MFRC_NOTAUTH         (42)
#define MI_WRONG_DES_MODE       (43)
#define MI_HOST_AUTH_FAILED     (44)
#define MI_WRONG_LOAD_MODE      (45)
#define MI_WRONG_DESKEY         (46)
#define MI_MKLOAD_FAILED        (47)
#define MI_FIFOERR              (48)
#define MI_WRONG_ADDR           (49)
#define MI_DESKEYLOAD_FAILED    (50)
#define MI_WRONG_SEL_CNT        (51)
#define MI_WRONG_TEST_MODE      (52)
#define MI_TEST_FAILED          (53)
#define MI_TOC_ERROR            (54)
#define MI_COMM_ABORT           (55)
#define MI_INVALID_BASE         (56)
#define MI_MFRC_RESET           (57)
#define MI_WRONG_VALUE          (58)
#define MI_VALERR               (59)
#define MI_COMPAREERR		(60)
#define MI_WRSIZERR		(61)

/* General Error */
#define ERR_LEN			(70)
#define ERR_PAR			(71)
#define ERR_TYP			(72)
#define ERR_CMD 		(73)
#define ERR_SUP  		(74)
#define ERR_VER            	(75)
#define ERR_NRF            	(76)

#define	ERR_FHEAD		(77)
#define ERR_FDATA		(78)
#define ERR_FTAIL		(79)
#define ERR_FCTRL		(80)
// 81 ~ 84 파일 다운로드 에러코드 

#define	READER_NAME		"RC531"
#define	READER_NAME_VERBOSE	"RC531 reader driver"

/* device numbers */
#define	READER_MAJOR		(242)

#define __IOCTL_RC531_INT_ENA__	 1000
#define __IOCTL_RC531_INT_DIS__	 2000
#define __IOCTL_RC531_ISR_RST__	 3000
#define __IOCTL_RC531_INF_RST__	 4000
#define __IOCTL_RC531_DLY_TEST__ 5000
#define __IOCTL_RC531_IRQ_TEST__ 6000
#define __IOCTL_RC531_RST_HIGH__ 7000
#define __IOCTL_RC531_RST_LOW__	 8000
#define __IOCTL_RC531_CMD_TRX__  9000  

#define RFBUF_SIZE 		 248	

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned int   dword;

typedef struct {  
	byte cmd; 
	byte err;
	byte piccmd;
	byte errstat;
	byte *sendbuf;  
	byte *recvbuf;
	byte infobit;
	byte bytesend;
	byte bytesent; 
	byte byterecv; 
	word bitsrecv;
	word timeout;
	byte collpos;
	byte irqsrc;
	byte isrcnt;
	byte isrpos;
       dword retpos;
} rfinf_t;


