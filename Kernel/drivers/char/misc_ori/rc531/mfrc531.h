/***********************************************************************/
/*      PROJECT         :       RC500                                  */
/*      FUNCTION        :       MFRC500.H                              */
/*      VERSION         :       1.0                                    */
/*      CPU             :       80C51-25MHz                            */
/*      COMPLIER        :       ICC8051 4.20                           */
/*      COMPANY         :       SST CO.,LTD                            */
/*      PROGRAMMER      :       SONG, PYEONGRAE                        */
/*      DATE            :       2000. 10. 08                           */
/***********************************************************************/

#define RC531_BASE		(VA_MISC_BASE+0xC00000) //0xfa800000

/* names */
#define	READER_NAME			"RC531"
#define	READER_NAME_VERBOSE		"RC531 reader driver"

#define A_TYPE	0x0a
#define B_TYPE	0x0b

/* device numbers */
#define	READER_MAJOR			(242)

#define READER_IOCTL_TYPE 	'P'

#define READER_HW_RESET		_SIOW ('P', 10, int) //2004.4.27
#define READER_CARD_DETECT	_SIOW ('P', 11, int)
#define READER_CARD_READ	_SIOR ('P', 12, int)
#define READER_PTI_GET		_SIOR ('P', 13, int)

#define READER_SNG_RSP_SET 	_SIOW ('P', 14, int)
#define READER_PCD_RD_E2 	_SIOR ('P', 15, int)
#define READER_PCD_WR_E2	_SIOW ('P', 16, int)
#define READER_PCD_RF_RST	_SIOW ('P', 17, int)
#define READER_PCD_HALT		_SIOW ('P', 18, int)
#define READER_RST_INFO		_SIOW ('P', 19, int)
#define READER_MAP_KEY_GET	_SIOR ('P', 20, int)

#define TRUE		0x01
#define FALSE		0x00

#define MAX_RF_BUF_SIZE 300
#define  DEF_FIFO_LENGTH    64  // default FIFO length
#define  SND_BUF_LEN       300  // max number of bytes to send
#define  RCV_BUF_LEN       300  // max number of bytes to receive

/* 이 시간을 조절한다.PcdSingleResponseCmd()에서 리턴 시간을 기다린다.
기본 값을 쓰면 type_a_select() 에서 리턴 잘못됨....
 정확한 리턴 값이 valid 되는 시간을 조절한다.*/
#define MAX_CYCLE_CNT 100000L //tjkim modify //10000 // maximum number of cycles to wait for response

#define PHILLIPS_KEY	0x01
#define ATMEL_KEY	0x02

// P I C C - C O M M A N D S
// commands which are handled by the tag
// Each tag command is written to the reader IC and transfered via RF
#define PICC_REQIDL        	0x26         // request idle
#define PICC_REQALL        	0x52         // request all
#define PICC_ANTICOLL1     	0x93         // anticollision level 1
#define PICC_ANTICOLL11    0x92         //!< anticollision level 1 212 kBaud
#define PICC_ANTICOLL12    0x94         //!< anticollision level 1 424 kBaud
#define PICC_ANTICOLL13    0x98         //!< anticollision level 1 848 kBaud

#define PICC_ANTICOLL2     	0x95         // anticollision level 2
#define PICC_ANTICOLL3     	0x97         // anticollision level 3
#define PICC_AUTHENT1A     	0x60         // authentication step 1
#define PICC_AUTHENT1B     	0x61         // authentication step 2
#define PICC_READ          	0x30         // read block
#define PICC_WRITE         	0xA0         // write block
#define PICC_INC           	0xC1         // increment value
#define PICC_DEC           	0xC0         // decrement value
#define PICC_TRANS		   	0xB0
#define PICC_RESTORE	   	0xC2	
#define PICC_HALT          	0x50         // halt

#define PICC_REQBSTD       0x00         // Type B request idle
#define PICC_REQBALL       0x08         // Type B request all

#define KEYA               	0x00
#define KEYB               	0x04

#define	ALL				1
#define IDLE				0

#define KEY_NONE           	0x00
#define KEY_AB             	0xab
#define KEY_FF             	0xff

// Mifare Error Codes
// Each function returns a status value, which corresponds to the
// mifare error codes.
#define MI_OK                 (   0)
#define MI_NOTAGERR           (   1)
#define MI_CRCERR             (   2)
#define MI_EMPTY              (   3)
#define MI_AUTHERR            (   4)
#define MI_PARITYERR          (   5)
#define MI_CODEERR            (   6)
#define MI_SERNRERR           (   8)
#define MI_NOTAUTHERR         (  10)    //0x0a
#define MI_BITCOUNTERR        (  11)    //0x0b
#define MI_BYTECOUNTERR       (  12)    //0x0c
#define MI_IDLE               (  13)    //0x0d
#define MI_TRANSERR           (  14)    //0x0e
#define MI_WRITEERR           (  15)	//0x0f
#define MI_VALERR             (  16)	//0x10
#define MI_KEYERR             (  17)	//0x11
#define MI_READERR            (  18)	//0x12
#define MI_OVFLERR            (  19)	//0x13
#define MI_POLLING            (  20)	//0x14
#define MI_FRAMINGERR         (  21)	//0x15
#define MI_ACCESSERR          (  22)	//0x16
#define MI_UNKNOWN_COMMAND    (  23)	//0x17
#define MI_COLLERR            (  24)	//0x18
#define MI_RESETERR           (  25)	//0x19
#define MI_INTERFACEERR       (  26)	//0x1a
#define MI_ACCESSTIMEOUT      (  27)	//0x1b
#define MI_NOBITWISEANTICOLL            28//(-28)
#define MI_QUIT                         30//(-30)
#define MI_CODINGERR                    31//(-31)
#define MI_SENDBYTENR                   51//(-51)
#define MI_CASCLEVEX                    52//(-52)    
#define MI_SENDBUF_OVERFLOW             53//(-53)
#define MI_BAUDRATE_NOT_SUPPORTED       ( 54)
#define MI_SAME_BAUDRATE_REQUIRED       ( 55)
#define MI_WRONGPARAM         (  60)	//0x3c
#define MI_WRONG_PARAMETER_VALUE        60//(-60)
#define MI_NYIMPLEMENTED      ( 100)	//0x64
#define MI_RECBUF_OVERFLOW    ( 112)

#define MI_COMM_ABORT         ( 120)	//0x78
#define MI_CALLOPEN           ( 121)	//0x79

#define MI_MIFARECARD		(125)
#define MI_PURCHASERR		(126)
#define MI_NOTCARD			127

#define RC_IOCTL_BASE    0x63
#define RC_GET_INFO		 _IOR(RC_IOCTL_BASE,1,unsigned long)
#define RC_SET_INFO		 _IOW(RC_IOCTL_BASE,2,unsigned long)

#define EXT_ERROR_CODE
#define EXT_OK				0x9000
#define EXT_NOTAGERR			0xa001
#define EXT_MIFARECARD			0xa002
#define EXT_PURCHASERR			0xa003

#define MAX_SEND        16

         
extern  unsigned long   WaitTimer;
extern	int	rf_file; 

struct MfCmdInfo {
	unsigned char  cmd;           //!< command code //tjkim add
	unsigned char  status;          // communication status
    	unsigned char  nBytesSent;      // how many unsigned chars already sent
    	unsigned char  nBytesToSend;    // how many unsigned chars to send
    	unsigned char  nBytesReceived;  // how many unsigned chars received
    	unsigned short nBitsReceived;   // how many bits received
    	unsigned char  irqSource;       // which interrupts have occured
    	unsigned char  collPos;         // at which position occured a collision
	unsigned char  errFlags;      //!< error flags
        unsigned char  saveErrorState;//!< accumulated error flags for    	
        unsigned char  RxAlignWA;     //!< workaround for RxAlign = 7
    	unsigned char  DisableDF;     //!< disable disturbance filter //tjkim add
	unsigned char  waitFor;
};

/*struct reader_st{
	unsigned char pcd_cmd;
	unsigned char byRxBuffer[300];	
	unsigned char byTxBuffer[300];
	volatile MfCmdInfo	MInfo;
	
};*/

struct reader_st{
	unsigned char pcd_cmd;
	unsigned char byRxBuffer[300];	
	unsigned char byTxBuffer[300];
	struct MfCmdInfo	MInfo;
	
};

struct card_key {
	unsigned char card_type;
	unsigned char key_type;
	unsigned char sector_num;
	unsigned char keyVal[6];
	unsigned char snrVal[4];
};

struct card_data {
	unsigned char block_num;
	unsigned char data_ptr[16];
};

typedef struct {
	unsigned char type;
	unsigned char fifo;
	unsigned char s_dat[300];
	unsigned int s_len;
	unsigned char r_dat[300];
	unsigned int r_len;
} RF_BUF;

struct pcd_st{
	unsigned short startaddr;
        unsigned char length;
       // unsigned char snr_data[4];	
        //unsigned char pti_data[5];	
        unsigned char data[0x200];
        //unsigned char *data;
};

extern	unsigned char MSndBuffer[SND_BUF_LEN];
extern	unsigned char MRcvBuffer[RCV_BUF_LEN];

//MfCmdInfo     MInfo;                // info struct for general use
extern	unsigned long	WaitTimer,SetTimer;
extern	void rf_reset1(void);
extern	void rf_reset2(void);
extern	void FlushFIFO(void);
extern	void SetBitMask(unsigned char reg,unsigned char mask);
extern	void ClearBitMask(unsigned char reg,unsigned char mask);
extern	void PcdSetTmo(unsigned long tmoLength);
extern	unsigned char PcdSingleResponseCmd(unsigned char cmd,unsigned char *send,unsigned char *rcv, struct MfCmdInfo *info);
extern  void PcdRfReset(unsigned short ms);
extern  unsigned char PcdReset(void);
extern  unsigned char PcdReadE2(unsigned short addr,int len,unsigned char *E2Data);
static  unsigned char PcdWriteE2(unsigned short addr,int len,unsigned char *E2Data);
extern  unsigned char Mf500PcdConfig(void);
extern 	void rf_switch_on(unsigned char picc_type);
extern  unsigned char Mf500PiccRequest(unsigned char req_code,unsigned char *atq);
extern  unsigned char Mf500PiccCascAnticoll(unsigned char select_code,unsigned char bcnt,unsigned char *snr);
extern	unsigned char Mf500PiccCascSelect(unsigned char select_code,unsigned char *snr,unsigned char *sak);
extern  unsigned char Mf500PiccAuth(unsigned char auth_mode, // KEYA, KEYB
                            unsigned char key_sector);   // 0 <= key_sector <= 15
extern  unsigned char mif_authectication2(  unsigned char auth_mode,  // KEYA or KEYB
                            unsigned char key_sector);     // key address in reader storage
extern  unsigned char Mf500PiccRead(unsigned char addr,unsigned char datalen,unsigned char *Rdata);
extern  unsigned char Mf500PiccWrite( unsigned char addr,unsigned char *Wdata);

extern  unsigned char Mf500PiccInc(unsigned char addr,unsigned long value);
extern  unsigned char Mf500PiccDec(unsigned char addr,unsigned long value);
extern  unsigned char Mf500PiccTrans(unsigned char addr);
extern  unsigned char Mf500PiccRest(unsigned char addr);


extern  unsigned char Mf500PcdLoadKey2( unsigned char  auth_mode,    	// KEYA or KEYB
                            unsigned char  key_sector,     // 0 <= key_addr <= 15
                            unsigned char  *mk);
extern	unsigned char Mf500PcdLoadKeyE2(unsigned char  auth_mode,       // KEYA or KEYB
                        	unsigned char  key_sector,   // 0 <= key_addr <= 15
                            unsigned char  *mk);         // 6 unsigned chars uncoded master key

extern	unsigned char Mf500PiccHalt(void);
extern  unsigned char Mf500PcdInit(unsigned char KeyType);
unsigned char inmcm(unsigned char addr);
void outmcm(unsigned char addr, unsigned char data);

