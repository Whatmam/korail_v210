
/***********************************************************************/
/*
 *	RF Reader(MF RC531) interface for Linux on INTEC
 *
 *	Copyright (c) 2002 TJ Kim
 *
 *	1.00	2003-12-10	TJ Kim <prodalma@astonlinux.com>
 *	- initial release
 */
/***********************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/delay.h>

#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/types.h>
#include <linux/ioport.h>

#include <linux/interrupt.h>
#include <linux/input.h>

#include <asm/io.h>
#include <asm/delay.h>
#include <asm/irq.h>
#include <asm/gpio.h>
#include <asm/uaccess.h>

#include <mach/map.h>
#include <mach/gpio.h>

#include <asm/gpio.h>
#include <mach/regs-gpio.h>

#include <plat/gpio-cfg.h>

#include <linux/soundcard.h>

#include "mfreg531.h"
#include "mfrc531.h"


#define RC531_INTER_GPIO				S5PV210_GPH1(3)

// inline structure in order to reset the communication channel between 
// function and ISR  


/*#define ResetInfo(info) info.status = MI_OK;
			info.irqSource = 0;
			info.nbytesSent = 0; 
			info.nbytesToSend = 0; 
			info.nbytesReceived = 0;
			info.nBitsReceived  = 0; 
			info.collPos = 0;  */
int rf_file;  
// actual FIFO length  
static   unsigned char MFIFOLength = DEF_FIFO_LENGTH;   

// storage for authentication keys  
// in order to provide a calling  
// compatible interface to old libraries  
// Other reader modules keep several sets  
// of keys in an E2PROM. In this case,  
// these keys are stored in the uC and  
// transfered to the reader module  
// before authentication  
// Infomation concerning data processing  
static   unsigned char MKeys[16][3][12];                   

// send buffer for general use  
unsigned char MSndBuffer[SND_BUF_LEN];         

// receive buffer for general use  
unsigned char MRcvBuffer[RCV_BUF_LEN];         
  
// Interrupt service routine  
// Variable in order to exchange data between function and ISR  

static   struct MfCmdInfo     *MpIsrInfo = 0;
static   unsigned char *MpIsrOut  = 0; 	// ISR send buffer  
static   unsigned char *MpIsrIn   = 0; 	// ISR receive buffer  

static int rf_DeviceOpen = 0;
  
unsigned long   WaitTimer,SetTimer;  
struct MfCmdInfo     MInfo;					// info struct for general use  
unsigned char	MIF_CSNR[5];

u16 *hw_rf_rc531_cs_base;

//#define RC500_ALE_ADD  		0xFE800000	//ALE°¡ ¿¢Æ¼ºêµÇ´Â ÁÖ¼ÒÀÌ´Ù.
//#define RC500_DATA_DATA 	0xe083e800	//DATAÀÐ°í ¾²±â ÁÖ¼ÒÀÌ´Ù.
#define RC500_DATA_DATA 	0xe0842800

#define GetRegPage(adr)         (0x80 | ((adr>>3) & 7))//Page enabel,ÇØ´ç·¹Áö½ºÅÍÀÇ Page¹øÈ£¸¦ ¾Ë¾Æ³½´Ù.
/*-------------------------------------------------------------------*/
/*----------------------------Àú¼öÁØ ÇÔ¼öµé ½ÃÀÛ---------------------*/
/*-------------------------------------------------------------------*/
/*ÁøÂ¥ Àú¼öÁØÀÌ´Ù. È£½ºÆ®¿ÍÀÇ ÀÎÅÍÆäÀÌ½º¿¡ µû¶ó inmcm,outmcm¸¸ ¼öÁ¤ÇÏ¸é µÈ´Ù*/
#define REG_RC531_WRITE(reg, val) 	(*(reg) = (val))
#define REG_RC531_READ(reg, pVal) 	*(pVal) = (*(reg))

#define RC531_HW_RESET_IO	87
#define RC531_READ_WRITE_DIR	86

  
unsigned char Mf500PiccAuthState( unsigned char auth_mode,unsigned char *snr, unsigned char block);  
unsigned char Mf500HostCodeKey(unsigned char *uncoded,unsigned char *coded);  
unsigned char Mf500PiccAuthKey(unsigned char auth_mode,unsigned char *snr,unsigned char *keys,unsigned char block);  

static void ResetInfo(struct MfCmdInfo info)
{
	MInfo.cmd            = 0; 
	MInfo.status         = MI_OK; 
	MInfo.irqSource      = 0; 
	MInfo.nBytesSent     = 0; 
	MInfo.nBytesToSend   = 0; 
	MInfo.nBytesReceived = 0; 
	MInfo.nBitsReceived  = 0; 
	MInfo.collPos        = 0; 
	MInfo.errFlags       = 0; 
        MInfo.saveErrorState = 0; 
        MInfo.RxAlignWA      = 0; 
	MInfo.DisableDF      = 0;  
}

unsigned char inmcm(unsigned char addr)
{
	unsigned char data;
	unsigned long flags;

	unsigned char temp;

	//addr = (addr >> 2 & 1) | ((addr >> 1 &1 ) << 1) | ((addr >> 0 & 1 ) << 2);
	//temp = addr << 5;
	temp = (addr & 0x07);
	
	//gpio_direction_output(RC531_READ_WRITE_DIR, 0);
	local_irq_save(flags);
	REG_RC531_WRITE( (unsigned char *)(RC500_DATA_DATA), temp);

	//gpio_direction_output(RC531_READ_WRITE_DIR, 1);
	REG_RC531_READ( (unsigned char *)(RC500_DATA_DATA), &data);
	//gpio_direction_output(RC531_READ_WRITE_DIR, 0);
	local_irq_restore(flags);

	return (data);
}

void outmcm(unsigned char addr, unsigned char data)
{

	unsigned char temp;
	unsigned long flags;

	//addr = (addr >> 2 & 1) | ((addr >> 1 &1 ) << 1) | ((addr >> 0 & 1 ) << 2);
	//temp = addr << 5;
	temp = (addr & 0x07);

	//gpio_direction_output(RC531_READ_WRITE_DIR, 0);
	local_irq_save(flags);
	REG_RC531_WRITE( (unsigned char *)(RC500_DATA_DATA), temp);
	REG_RC531_WRITE( (unsigned char *)(RC500_DATA_DATA), data);
	local_irq_restore(flags);

}

unsigned char ReadRawIO( unsigned char addr )
{
	//return (inmcm( addr ));
	return *(unsigned char*)(((addr & 0x07)<<1)+RC500_DATA_DATA);;
}


void WriteRawIO( unsigned char addr, unsigned char data )
{

	//outmcm( addr, data );
	*(unsigned char*)(((addr & 0x07)<<1)+RC500_DATA_DATA)=data;
}

unsigned char ReadIO(unsigned char Address)
{
#if 0
	WriteRawIO(0x00, GetRegPage( Address ));
	return ReadRawIO( Address );//modify
#else
	*(unsigned char*)RC500_DATA_DATA = ((Address>>3)& 0x07)|0x80; //page select
        return *(unsigned char*)(((Address& 0x07)<<1)+RC500_DATA_DATA); //
#endif

}

void WriteIO(unsigned char Address,unsigned char value)
{
#if 0
	WriteRawIO(0x00, GetRegPage(Address));
	WriteRawIO( Address , value );//modify
#else
	*(unsigned char*)RC500_DATA_DATA = ((Address>>3)& 0x07)|0x80;
        *(unsigned char*)(((Address& 0x07)<<1)+RC500_DATA_DATA)=value;
#endif
}

  
/***********************************************************************/  
//  FUNCTION: FlushFIFO  
//        IN: -  
//       OUT: -  
//    RETURN:  
//   COMMENT: All remaining date in the FIFO of the reader module is  
//            erased by this function. Before wrinting new data or  
//            starting a new command, all remaining data from former  
//            commands should be deleted. Please note, that in  
//            normal operation, never data should be left, that means  
//            that a call to this function should not be necessary.  
//  
/************************************************************************/  
void FlushFIFO(void)  
{  
    SetBitMask(RegControl,0x01);  
}  
  
/***********************************************************************/  
//  FUNCTION: SetBitMask  
//        IN: reg      register address  
//            mask     bit mask to set  
//       OUT: -  
//    RETURN:  
//   COMMENT:  This function performs a read - modify - write sequence  
//             on the specified register. All bits with a 1 in the mask  
//             are set - all other bits keep their original value.  
/***********************************************************************/  
void SetBitMask(unsigned char reg,unsigned char mask)  
{  
   	unsigned char tmp;  
  
   	tmp = ReadIO(reg);  
   	WriteIO(reg,tmp | mask);                 // set bit mask  
   	return;  
}  
  
/***********************************************************************/  
//  FUNCTION: ClearBitMask  
//        IN: reg      register address  
//            mask     bit mask to clear  
//       OUT: -  
//    RETURN:  
//   COMMENT:  This function performs a read - modify - write sequence  
//             on the specified register. All bits with a 1 in the mask  
//             are cleared - all other bits keep their original value.  
/***********************************************************************/  
void ClearBitMask(unsigned char reg,unsigned char mask)  
{  
   	unsigned char tmp;  
  
    	tmp = ReadIO(reg);  
   	WriteIO(reg,tmp & ~mask);                // clear bit mask  
   	return;  
}  
  
/***********************************************************************/  
//  FUNCTION: PcdSetTmo  
//        IN: tmoLength   1  ... 1.0 ms timeout periode  
//                        2  ... 1.5 ms timeout periode  
//                        3  ... 6.0 ms timeout periode  
//       OUT: -  
//    RETURN:  
//   COMMENT: Set timeout length of the reader internal timer.  
/**********************************************************************/  

void PcdSetTmo(unsigned long tmoLength)  
{
    unsigned char prescale = 7;
    unsigned long reload = tmoLength;		

    while (reload > 255)
    {
       prescale++;
       reload = reload >> 1; // division by 2
    }
    // if one of the higher bits are set, the prescaler is set
    // to the largest value
    if (prescale > 0x15)
    {
       prescale = 0x15;
       reload   = 0xFF;
    }

    // fcnt = 13560000 / (2^prescale)
    // T = (reload - counter) / fcnt
    WriteIO(RegTimerClock,prescale); // TAutoRestart=0,TPrescale=prescale
    WriteIO(RegTimerReload,reload);// TReloadVal = reload
	
    return ;
}
  
/*********************************************************************/ 
//                 M I F A R E    R E S E T  
/*********************************************************************/  
void PcdRfReset(unsigned short ms)  
{  
    	ClearBitMask(RegTxControl,0x03);       // Tx2RF-En, Tx1RF-En disablen  
	udelay(ms*1000);
   	 SetBitMask(RegTxControl,0x03);       	// Tx2RF-En, Tx1RF-En enable  
   	return;  
}  

/*********************************************************************/ 
//                 M I F A R E    RESET PIN SET  
/*********************************************************************/ 
void PcdRfHwReset(void)
{  
#if 0
    s3c_gpio_setpin(S5PV210_GPG1(2), 0);
    udelay(1000);	//500ms

	s3c_gpio_setpin(S5PV210_GPG1(2), 1);
    udelay(1000); //100ms
	udelay(1000);
	udelay(1000);
	udelay(1000);
	
	s3c_gpio_setpin(S5PV210_GPG1(2), 0);
	udelay(1000); //1ms
	udelay(1000);
	udelay(1000);
	udelay(1000);
	udelay(1000);
	udelay(1000);
#endif
}		

/*********************************************************************/ 
//                 M I F A R E    RESET PIN SET  
/*********************************************************************/ 
void pcd_rf_hw_reset(void) //2004.4.27 add
{
#if 0
	s3c_gpio_setpin(S5PV210_GPG1(2), 0);
    udelay(1000);	//500ms

	s3c_gpio_setpin(S5PV210_GPG1(2), 1);
    udelay(1000); //100ms
	udelay(1000);
	udelay(1000);
	udelay(1000);
	
	s3c_gpio_setpin(S5PV210_GPG1(2), 0);
	udelay(1000); //1ms
	udelay(1000);
	udelay(1000);
	udelay(1000);
	udelay(1000);
	udelay(1000);
#endif
}	

/*********************************************************************/ 
//                 M I F A R E    GPIO PIN INIT
/*********************************************************************/ 

static void rc531_initialize(void)
{

 //   	rBWSCON &= ~((0x1 << 23)|(0x1 << 22)|(0x3 <<20)); 
//    	rBWSCON |=  ((0x0 << 23) | (0x0 << 20)  | (0x00 << 20));
	    
    	//rBANKCON5 = (0x0 << 13) | (0x3 << 11) | (0x7 << 8) | 
       //         (0x1 << 6) | (0x0 << 4) | (0x3 << 2) | 	0x0;
                	
#if 0
	rGPGCON &= ~(0x3<<26);
	rGPGCON |= (0x1<<26);
	
	rGPFCON &= ~(0x3 << 0);
	rGPFCON |= (0x2 << 0); //EINT0

	rEXTINT0 &= ~(0x7 << 0);		/* EINT0 is cleared */
	rEXTINT0 |= (0x2 <<0);			/* Falling edge mode */	
#endif
//	ClearPending(IRQ_EINT0);
  	 	
}

/***********************************************************************/  
//  FUNCTION: PcdReset  
//        IN: -  
//       OUT: -  
//    RETURN: MI_OK  
//            MI_RESETERR  
//            MI_INTERFACEERR  
//   COMMENT: This function initiates a reset of the reader IC. Note, that  
//            the reset pin of the reader has to be connected to the  
//            expected port pin of the microcontroller.  
/***********************************************************************/  
unsigned char PcdReset(void)		// not used for pwr on rst  
{  
	PcdRfHwReset();
    	
    	WaitTimer = 10000L;  
    	while(--WaitTimer){         // wait until reset command recognized  
		if((ReadIO(RegCommand) & 0x3F) == 0x00){ 
			break;  
		}
    	}  
    	if(WaitTimer == 0 ){ 
		return MI_RESETERR;
	}
    	WriteIO(RegPage,0x80);	// Dummy access in order to determine the bus  
                           	// configuration  
                           	// necessary read access  
                            // after first write access, the returned value  
                            // should be zero ==> interface recognized 
    	if (ReadIO(RegCommand) != 0x00) return MI_INTERFACEERR;  
    	return MI_OK;  
}  
  
/***********************************************************************/
// NAME     : PcdReadE2      
// FUNCTION : 
// ARGUMENT :
// RETURNS  :
/***********************************************************************/
unsigned char PcdReadE2(unsigned short addr,int len,unsigned char *E2Data)  
{  
   	unsigned char status;  
   
   	PcdSetTmo(3400);
   	
   	ResetInfo(MInfo);  
   	MSndBuffer[0] = (unsigned char)addr;    	// addr low unsigned char  
   	MSndBuffer[1] = (unsigned char)(addr>>8); // addr high unsigned char  
   	MSndBuffer[2] = len;         	// Page  
   	MInfo.nBytesToSend   = 3;  
  
   	//SetTimer = 10000L;  
   	PcdSetTmo(3);
   	status = PcdSingleResponseCmd(PCD_READE2,  
                       MSndBuffer,  
                       MRcvBuffer,  
                       &MInfo);     // read e2  
   	if(status != MI_OK){
	   	return status;
   	}	   
   	memcpy(E2Data,MRcvBuffer,len);  
   	return status;  
}  
  
/***********************************************************************/
// NAME     : PcdWriteE2      
// FUNCTION : 
// ARGUMENT :
// RETURNS  :
/***********************************************************************/
static unsigned char PcdWriteE2(unsigned short addr,int len,unsigned char *E2Data)  
{  
   	unsigned char status;  
   	
   	PcdSetTmo(6800);
   	
   	ResetInfo(MInfo);  
   	MSndBuffer[0] = (unsigned char)addr;      // addr low unsigned char  
   	MSndBuffer[1] = (unsigned char)(addr>>8); // addr high unsigned char  
   	memcpy(&MSndBuffer[2],E2Data,len);  
   	MInfo.nBytesToSend   = len+2;  
   	//SetTimer = 10000L;  
//   	PcdSetTmo(6);
   	status = PcdSingleResponseCmd(PCD_WRITEE2,  
                       MSndBuffer,  
                       MRcvBuffer,  
                       &MInfo);     // write e2 
   	return status;  
}  

  
/***********************************************************************/  
//  FUNCTION: Mf500PcdConfig  
//        IN: -  
//       OUT: -  
//    RETURN: MI_OK  
//            MI_RESETERR  
//            MI_INTERFACEERR  
//   COMMENT: Configuration of the reader IC. This function has to be called  
//            before the first data is written to the reader. A reset  
//            is initiated and several registers are set.  
/***********************************************************************/  
unsigned char Mf500PcdConfig(void)  
{  
    unsigned char status;  
  
    status = PcdReset();        	// when pwr on rst not used  
    
    if(status != MI_OK) return status;  
    WriteIO(RegClockQControl,0x0); 	 	//	Qclock auto calibration 
    WriteIO(RegClockQControl,0x40);  	// no calibration
    udelay(100);
    ClearBitMask(RegClockQControl,0x40);// clear bit ClkQCalib for  
                                     	// further calibration  
    //WriteIO(RegBitPhase,0xa8);//0xAD); 		 	// reset value	 
   // WriteIO(RegRxThreshold,0x9d);//rev1 0x44    	// initialize minlevel  
    //WriteIO(RegRxControl1,0x73);		// 8Pulses,42db	
    WriteIO(RegRxControl2,0x41);     //tjkim rev1 0x01	// disable auto power down  
    //WriteIO(RegFIFOLevel,0x04); 	 	// water level	 
   // WriteIO(RegModWidth,0x13);			// modulation width
   //WriteIO(RegTimerControl,0x06);  //tjkim rev1 	// TStopRxEnd=0,TStopRxBeg=0,  
                                     	// TStartTxEnd=1,TStartTxBeg=0  
                                     	// timer must be stopped manually  
                                     	
    /*RegRxWait °¡ 4º¸´Ù ÀÛÀ¸¸é µ¿ÀÛÇÏÁö ¾Ê´Â´Ù*/								 
    //WriteIO(RegRxWait,0x06);//6); //tjkim rev1 0x06
    WriteIO(RegIRqPinConfig,0x03);//tjkim rev1 0x03	// interrupt active low enable  
                                     	// initialize internal key memory  
    memset(&MKeys[0],0x00,sizeof(MKeys));  
    return MI_OK;  
}  
////////////////////////////////////////////////////////////////////////
// type select 
// type A : picc_type = 'A', type B : picc_type = 'B'
////////////////////////////////////////////////////////////////////////
void rf_switch_on(unsigned char picc_type)
{
   	if(picc_type == 'B'){                	// type B select
	    WriteIO(RegCoderControl,	0x20);	// iso14443 b - NRZ
	    WriteIO(RegDecoderControl,	0x19);	// RxFraming-ISO typeB , RxCoding - BPSK
	    WriteIO(RegTypeBFraming,	0x23);	//rev1 // 0x2f-EOF11,EGT1,SOF11-3
	    WriteIO(RegCRCPresetLSB ,	0xff);	// 
	    WriteIO(RegCRCPresetMSB ,	0xff);	//
	    WriteIO(RegBPSKDemControl,	0x3e); //rev1
	    WriteIO(RegModConductance,	0x06);	//rev1// modulation 10%	
	    WriteIO(RegCwConductance,	0x3f);	// 100%
	    WriteIO(RegTxControl,		0x4B);	//rev1
	    WriteIO(RegChannelRedundancy,0x2c);	//
	    WriteIO(RegBitFraming,		0x00);	//
	   // WriteIO(RegTestDigiSelect,	0x83);	//
	    WriteIO(RegTestAnaSelect,	0x2);	//
	}	
    else{                                	// type A select
    	    WriteIO(RegBPSKDemControl,	0x0e);
    	    
	    WriteIO(RegCoderControl,  	0x19);
	    WriteIO(RegModWidth,0x13);
	    
	    WriteIO(RegRxControl1,0x73);    	    	    
	    WriteIO(RegDecoderControl,	0x08);
	    WriteIO(RegCRCPresetLSB ,	0x63);
	    WriteIO(RegCRCPresetMSB ,	0x63);
	    
	    WriteIO(RegRxThreshold,0x88);
//	    WriteIO(RegModConductance,	0x3f);
//	    WriteIO(RegCwConductance,	0x3f);
//	    WriteIO(RegTxControl,	0x5B);
	    WriteIO(RegBPSKDemControl,	0x00);
	}	
   	SetBitMask(RegTxControl,0x03);    	// Tx2RF-En, Tx1RF-En enable  

}

/***********************************************************************/  
//  FUNCTION: Mf500PiccRequest  
//        IN: rq_code  can take the following values  
//                     ALL   Request Code 52hex is sent out to get also a  
//                           response from cards in halt state.  
//                     IDLE  Request Code 26hex is sent out to get a response  
//                           only from cards that are not in halt state.  
//                     Note: Future cards will work also with other request  
//                           codes.  
//       OUT: atq      16 bit ATQ (answer to request).  
//                     atq[0] .. LSunsigned char  
//                     atq[1] .. MSunsigned char  
//    RETURN:  
//   COMMENT: This function accesses the reader module and activates sending 
//            the REQ code to the MIFARE card. After sending the command to the
//            card the function waits for the card's answer.  
//  
//            Please note, that the actual work is done by the function  
//            M500PiccCommonRequest, because of the interface behaviour between 
//            Mifare and ISO 14443  
//  
//            The Card replies the ATQ.  
//  
//            ATQ:  
//  
//                    +----+----+----+----+----+----+----+----+  
//            LSunsigned char  | b8 | b7 | b6 | b5 | b4 | b3 | b2 | b1 |  
//                    +----+----+----+----+----+----+----+----+  
//                    |         |    |                        |  
//                    | UID size| RFU|   bit-frame anticoll   |  
//                    |         |    |                        |  
//                    | 00..std |    |  (if any bit set .. Y, |  
//                    | 01..dbl |    |             else .. N) |  
//                    | 10..tpl |    |                        |  
//  
//  
//  
//                    +----+----+----+----+----+----+----+----+  
//            MSunsigned char  | b8 | b7 | b6 | b5 | b4 | b3 | b2 | b1 |  
//                    +----+----+----+----+----+----+----+----+  
//                    |                                       |  
//                    |                 RFU                   |  
//                    |                                       |  
/***********************************************************************/  
unsigned char Mf500PiccRequest(unsigned char req_code,unsigned char *atq)  
{  
    unsigned char status;  

	PcdSetTmo(60);  
	// RxCRC and TxCRC disable, parity enable  
    	WriteIO(RegChannelRedundancy,0x03);	
    	ClearBitMask(RegControl,0x08); // disable crypto 1 unit  
    	WriteIO(RegBitFraming,0x07);        // set TxLastBits to 7  
  
	//if(req_code & ALL) req_code = PICC_REQALL;
	//else 			   req_code = PICC_REQIDL;
	
   	ResetInfo(MInfo);  
   	MSndBuffer[0] = req_code;  
   	MInfo.nBytesToSend   = 1;  
   	MInfo.DisableDF = 1;
   	//SetTimer = 1000L;  

    	status = PcdSingleResponseCmd(PCD_TRANSCEIVE,  
                      	MSndBuffer,  
                      	MRcvBuffer,  
                      	&MInfo);  
                      	
      if ((status == MI_OK) && (MInfo.nBitsReceived != 16)) // 2 bytes expected
      {
         status = MI_BITCOUNTERR;
      } 
      //if ((status == MI_COLLERR) && (MInfo.nBitsReceived == 16))
      if (MInfo.nBitsReceived == 16)
      { //
         status = MI_OK; // all received tag-types are combined to the 16 bit
      }  
      // in any case, copy received data to output - for debugging reasons
      if (MInfo.nBytesReceived >= 2)
      {
         memcpy(atq,MRcvBuffer,2);      
      }
      else
      {
         if (MInfo.nBytesReceived == 1)
            atq[0] = MRcvBuffer[0];
         else
            atq[0] = 0x00;
         atq[1] = 0x00;
      } 
   	return MI_OK;  
}  
  
/***********************************************************************/  
//  FUNCTION: Mf500PiccCascAnticoll  
//        IN: select_code    0x93  standard select code  
//                           0x95  cascaded level 1  
//                           0x97  cascaded level 2  
//            bcnt	Number of snr-bits that are known (default value is 0)  
//            *snr  4 unsigned chars serial number (number of bits, which  
//                  are known and indicated by "bcnt"  
//       OUT: *snr  4 unsigned chars serial number, determined by the anticollision  
//                       sequence  
//    RETURN:  
//   COMMENT: Corresponding to the specification in ISO 14443, this function  
//            is able to handle extended serial numbers. Therefore more than  
//            one select_code is possible. The function transmitts a  
//            select code and all ready tags are responding. The highest  
//            serial number within all responding tags will be returned by  
//            this function.  
/***********************************************************************/  
unsigned char Mf500PiccCascAnticoll (unsigned char select_code,unsigned char bcnt,unsigned char *snr)  
{  
   	unsigned char  status = MI_OK;  
   	unsigned char  snr_in[4];             // copy of the input parameter snr  
   	unsigned char  nbytes     = 0;  
   	unsigned char  nbits      = 0;  
   	unsigned char  complete   = 0;  
   	unsigned char  i          = 0;  
   	unsigned char  byteOffset = 0;  
   	unsigned char  snr_crc;  
   	unsigned char  snr_check;  
   	//unsigned char  select_code = 0x93;  
   	unsigned char dummyShift1;       // dummy byte for snr shift
   	unsigned char dummyShift2;       // dummy byte for snr shift    
  
   	PcdSetTmo(106); 
   	//************* Initialisierung ******************************  
   	memcpy(snr_in,snr,4);  
  
    	WriteIO(RegDecoderControl,0x28);   // ZeroAfterColl aktivieren  
    	ClearBitMask(RegControl,0x08); // disable crypto 1 unit  
  
   	//************** Anticollision Loop ***************************  
   	complete=0;  
    	//bcnt = 0;                           // no part of the snr is known  
   	while (!complete && (status == MI_OK) )
	{  
         	if (bcnt > 32)
         	{
            		status = MI_WRONG_PARAMETER_VALUE;
            		continue;
         	}
         		
		ResetInfo(MInfo);  
		MInfo.cmd = select_code;  //tjkim add // pass command flag to ISR        
         	MInfo.DisableDF = 1;      //tjkim add
         	WriteIO(RegChannelRedundancy,0x03); // RxCRC and TxCRC disable, parity enable
        	nbits = bcnt % 8;               // remaining number of bits  
        	if (nbits)
		{  
			// TxLastBits/RxAlign auf nb_bi  
			WriteIO(RegBitFraming,nbits << 4 | nbits); 
         		nbytes = bcnt / 8 + 1;	
			// number of unsigned chars known  in order to solve an inconsistancy in the
			// anticollision sequence (will be solved soon), the case of 7 bits
			// has to be treated in a separate way - please note the errata 
			// sheet  
            		if (nbits == 7)
			{  
				MInfo.RxAlignWA = 1; //tjkim add
				MInfo.nBitsReceived = 7; //tjkim add
                		WriteIO(RegBitFraming,nbits);	// reset RxAlign to zero  
        		}  
      		}  
        	else
		{
         		nbytes = bcnt / 8;  
      		}  
  
      		MSndBuffer[0] = select_code;  
        	MSndBuffer[1] = 0x20 + ((bcnt/8) << 4) + nbits;//number of unsigned chars send  
  
        	for (i = 0; i < nbytes; i++)
		{                  	
			// Sende Buffer beschreiben  
         		MSndBuffer[i + 2] = snr_in[i];  
      		}  
      		MInfo.nBytesToSend   = 2 + nbytes;  
      //	SetTimer = 10000L;  

      		status = PcdSingleResponseCmd(PCD_TRANSCEIVE,  
                         	MSndBuffer,  
                         	MRcvBuffer,  
                         	&MInfo);  
  
  		/* tjkim add ver3.0 ÂüÁ¶*/
           	if (MInfo.RxAlignWA)
         	{
            	// reorder received bits
            		dummyShift1 = 0x00;
            		for (i = 0; i < MInfo.nBytesReceived; i++)
            		{
                		dummyShift2 = MRcvBuffer[i];
                		MRcvBuffer[i] = (dummyShift1 >> (i+1)) | (MRcvBuffer[i] << (7-i));
                		dummyShift1 = dummyShift2;
            		}
            		MInfo.nBitsReceived -= MInfo.nBytesReceived; // subtract received parity bits
            		// recalculation of collision position
            		if ( MInfo.collPos ) MInfo.collPos += 7 - (MInfo.collPos + 6) / 9;
         	}
         
        	if (status == MI_OK || status == MI_COLLERR)
		{   // no other occured  
         	// Response  Processing  
			// not 5 unsigned chars answered  
            		byteOffset = 0;
            		if ( nbits != 0 )           // last byte was not complete
            		{
               			snr_in[nbytes - 1] = snr_in[nbytes - 1] | MRcvBuffer[0];
               			byteOffset = 1;
            		}
            		for ( i =0; i < (4 - nbytes); i++)     
            		{
               			snr_in[nbytes + i] = MRcvBuffer[i + byteOffset];
            		}
            		// R e s p o n s e   P r o c e s s i n g   
            		if ( MInfo.nBitsReceived != (40 - bcnt) ) // not 5 bytes answered
            		{
               			status = MI_BITCOUNTERR;
            		} 
            		else 
            		{
               			if (status != MI_COLLERR ) // no error and no collision
               			{
                  			// SerCh check
                  			snr_crc = snr_in[0] ^ snr_in[1] ^ snr_in[2] ^ snr_in[3];
                  			snr_check = MRcvBuffer[MInfo.nBytesReceived - 1];
                  			if (snr_crc != snr_check)
                  			{
                     				status = MI_SERNRERR;
                  			} 
                  			else   
                  			{
                     				complete = 1;	
                  			}
               			}
               			else                   // collision occured
               			{
                  			bcnt = bcnt + MInfo.collPos - nbits;
                  			status = MI_OK;	
               			}
            		}
        	}
      	}
//   }
   // transfer snr_in to snr - even in case of an error - for 
   // debugging reasons
   	memcpy(snr,snr_in,4);
  
   	//----------------------Einstellungen aus Initialisierung ruecksetzen  
    	ClearBitMask(RegDecoderControl,0x20); 	// ZeroAfterColl disable  
  
   	return status;  
}  
  
/***********************************************************************/  
//  FUNCTION: Mf500PiccCascSelect  
//        IN: select_code  
//            *snr          4 unsigned chars serial number  
//       OUT: *sak          1 unsigned char select acknowledge  
//    RETURN:  
//   COMMENT: Selects a UID level, depending on select code.  
//            Returns Select Acknowledge unsigned char.  
//  
//            Corresponding to the specification in ISO 14443, this function  
//            is able to handle extended serial numbers. Therefore more than  
//            one select_code is possible.  
//  
//            Select codes:  
//  
//            +----+----+----+----+----+----+----+----+  
//            | b8 | b7 | b6 | b5 | b4 | b3 | b2 | b1 |  
//            +-|--+-|--+-|--+-|--+----+----+----+-|--+  
//              |    |    |    |  |              | |  
//                                |              |  
//              1    0    0    1  | 001..std     | 1..bit frame anticoll  
//                                | 010..double  |  
//                                | 011..triple  |  
//  
//            SAK:  
//  
//            +----+----+----+----+----+----+----+----+  
//            | b8 | b7 | b6 | b5 | b4 | b3 | b2 | b1 |  
//            +-|--+-|--+-|--+-|--+-|--+-|--+-|--+-|--+  
//              |    |    |    |    |    |    |    |  
//                        |              |  
//                RFU     |      RFU     |      RFU  
//  
//                        1              0 .. UID complete, ATS available  
//                        0              0 .. UID complete, ATS not available  
//                        X              1 .. UID not complete  
/***********************************************************************/  
unsigned char Mf500PiccCascSelect(unsigned char select_code,unsigned char *snr,unsigned char *sak)  
{  
   	unsigned char status = MI_OK;  
   	//unsigned char select_code = 0x93;  
  
  	PcdSetTmo(106); 
    	WriteIO(RegChannelRedundancy,0x0F); 	// RxCRC,TxCRC, Parity enable  
    	ClearBitMask(RegControl,0x08);   	// disable crypto 1 unit  
  
   	//************* Cmd Sequence **********************************  
   	ResetInfo(MInfo);  
   	MSndBuffer[0] = select_code;  
    	MSndBuffer[1] = 0x70;               	// number of unsigned chars send  
   	memcpy(MSndBuffer + 2,snr,4);  
   	MSndBuffer[6] = MSndBuffer[2]  
                   	^ MSndBuffer[3]  
                   	^ MSndBuffer[4]  
                   	^ MSndBuffer[5];  
   	MInfo.nBytesToSend   = 7;  
 	MInfo.DisableDF = 1;
   	status = PcdSingleResponseCmd(PCD_TRANSCEIVE,  
                       	MSndBuffer,  
                       	MRcvBuffer,  
                       	&MInfo);  

//       printk("Select = %d bit=%d sak=0x%x\n",status,MInfo.nBitsReceived,MRcvBuffer[0]);
   	*sak = 0;  
      	if (status == MI_OK)    // no timeout occured
      	{
         	if (MInfo.nBitsReceived != 8)    // last byte is not complete
         	{
            		status = MI_BITCOUNTERR;
         	}
         	else
         	{
            		//memcpy(MLastSelectedSnr,snr,4);       
            		memcpy(MIF_CSNR,snr,4);     
         	}
      	}
      // copy received data in any case - for debugging reasons
      	*sak = MRcvBuffer[0];  
	
    	return MI_OK;  
}  
  
//////////////////////////////////////////////////////////////////////
//        S E T   D E F A U L T    C O M M   A T T R I B S
///////////////////////////////////////////////////////////////////////
static char Mf500PcdSetDefaultAttrib(void)
{
   char   status = MI_OK;

	WriteIO(RegRxControl1,0x73);							// disable auto power down
	WriteIO(RegDecoderControl,0x08);						// Manchester Coding
	WriteIO(RegRxThreshold,0x88);
	WriteIO(RegRxControl2,0x01);	// DECODER SOURCE : EXT
	WriteIO(RegClockQControl,0x00);
	WriteIO(RegCRCPresetLSB,0x63);							// set CRC preset to 0x6363
	WriteIO(RegCRCPresetMSB,0x63);

	WriteIO(RegTxControl,0x5b);							// Force100ASK, TX1 & TX2 enable
	WriteIO(RegCoderControl,0x19);							// Miller coding, 106kbps

 	WriteIO(RegMfOutSelect,0x02); //MFOUT ½ÅÈ£ µð¹ö±ë¿ë
 	
	WriteIO(RegFIFOLevel,0x04);							// FIFO LEVEL
	WriteIO(RegTestAnaSelect,0x0b);
	WriteIO(RegModConductance,0x3f);
	WriteIO(RegCwConductance,0x3f);
	
	PcdRfReset(1); //ÀÌ°Í ¶§¹®¿¡ ÇÑ¹øÀº Á¤»ó, ¶ÇÇÑ¹øÀº ¿¡·¯°¡ ¹ß»ýÇß´ø°ÍÀÓ
	
   return status;
}
  
///////////////////////////////////////////////////////////////////////
//       M I F A R E   P I C C   A C T I V A T I O N    S E Q E N C E
///////////////////////////////////////////////////////////////////////
static char Mf500PiccActivateIdle(unsigned char br,
                           unsigned char *atq, 
                           unsigned char *sak, 
                           unsigned char *uid, 
                           unsigned char uid_len)
{
  unsigned char cascade_level=0;
  unsigned char sel_code;
  unsigned char uid_index;
  signed char status;
  unsigned char cmdASEL=0;

  uid_len      = 0;

  //call activation with def. divs
  /*rfa_type.c ¸¦ ÂüÁ¶ÇÏ¿© ÇÑ¹ø ¸¸µé¾î º»°Í*/
  Mf500PcdSetDefaultAttrib();
  
    
     status = Mf500PiccRequest(PICC_REQIDL,atq);
     
//     printk("ATQ_a[0]=0x%x ATQ_a[1]=0x%x \n",atq[0],atq[1]);
//printk("request status = %d\n",status);
  if (status == MI_OK)
  {
     if((atq[0] & 0x1F) == 0x00) // check lower 5 bits, for tag-type
                                 // all tags within this 5 bits have to
                                 // provide a bitwise anticollision
     {
        status = MI_NOBITWISEANTICOLL;
     }
  }
  if (status == MI_OK)
  {
      //Get UID in 1 - 3 levels (standard, [double], [triple] )
      //-------
      switch(br)
      {
         case 0: cmdASEL = PICC_ANTICOLL1; break;
         case 1: cmdASEL = PICC_ANTICOLL11; break;
         case 2: cmdASEL = PICC_ANTICOLL12; break;
         case 3: cmdASEL = PICC_ANTICOLL13; break;
         default:
              status = MI_BAUDRATE_NOT_SUPPORTED; break;
      }
  }
  if (status == MI_OK)
  {
      cascade_level = 0;
      uid_index     = 0;
      do
      {
        //Select code depends on cascade level
        sel_code   = cmdASEL + (2 * cascade_level);
        cmdASEL = PICC_ANTICOLL1; // reset anticollistion level for calculation
        //ANTICOLLISION
        status = Mf500PiccCascAnticoll(sel_code, 0, &uid[uid_index]);
//        printk("Anticoll status = %d\n",status);
        //SELECT
//#ifdef MF_DEBUG        
//        printk("Anticollision OK snr[0]=0x%x snr[1]=0x%x snr[2]=0x%x snr[3]=0x%x \n",uid[0],uid[1],uid[2],uid[3] );
//#endif        
        if (status == MI_OK)
        {
           status = Mf500PiccCascSelect(sel_code, &uid[uid_index], sak);
//           printk("Select status = %d\n",status);
           if (status == MI_OK)
           {
              cascade_level++;

              //we differ cascaded and uncascaded UIDs
              if (*sak & 0x04) // if cascaded, bit 2 is set in answer to select
              {
                 //this UID is cascaded, remove the cascaded tag that is
                 //0x88 as first of the 4 byte received
                 memmove(&uid[uid_index], &uid[uid_index + 1], 3);
                 uid_index += 3;
                 uid_len += 3;
              }
              else
              {
                 //this UID is not cascaded -> the length is 4 bytes
                 uid_index += 4;
                 uid_len += 4;
              }
           }
        }
      }
      while((status == MI_OK)        // error status
            && (*sak & 0x04)         // no further cascade level
            && (cascade_level < 3)); // highest cascade level is reached
   }
   if (status == MI_OK)
   {
      //Exit function, if cascade level is triple and sak indicates another
      //cascase level.
      if ((cascade_level == 3) && (*sak & 0x04))
      {
         uid_len = 0;
         status = MI_CASCLEVEX;
      }
   }
   return (status);
}

///////////////////////////////////////////////////////////////////////
//          M I F A R E      A U T H E N T I C A T I O N
//   calling compatible version    
///////////////////////////////////////////////////////////////////////
static char Mf500PiccKeyLoad(unsigned char key_type,    // PICC_AUTHENT1A or PICC_AUTHENT1B
                   unsigned char key_sector,    // key address in reader storage
                   unsigned char *mk)       // block number which should be 
{
	unsigned char index;	
   	unsigned char offset = 0;
	
	if((key_type != PICC_AUTHENT1A) && (key_type != PICC_AUTHENT1B))
		return MI_CODEERR;
		
	index = key_type - 0x60; //index is 0 or 1
	
	if(key_type == PICC_AUTHENT1B ){
		offset = 6;  
 	} 
// 	printk("sec=%d index=%d offset=%d\n",key_sector,index,offset);
   	memcpy(&MKeys[key_sector][index][offset],mk,6);  
   	return MI_OK;  
}

  
/***********************************************************************/  
//  FUNCTION: Mf500PiccAuth 
//        IN: auth_mode PICC_AUTHENT1A or PICC_AUTHENT1B  
//            selects master key A or master key B  
//            *snr  	4 unsigned char serial number of the card, which should be  
//                       authenticated  
//            key_sector spec. the key E2PROM key number from which the data  
//                       should be taken (0..15)  
//            sector    addresses the sector number on the card, which shall be
//                      authenticated. For MIFARE standard cards, addr can take
//                      a value from 0 to 15, for other card types please refer
//                      to the according product description.  
//       OUT: -  
//    RETURN:  
//   COMMENT: This function authenticates one card's sector using the specified
//            mode. After sending the command to the card the  
//            function waits for the card's answer.  
//            The keys for authentication have to be stored in E2PROM in  
//            advance.  
/***********************************************************************/  
unsigned char Mf500PiccAuth(unsigned char auth_mode,		// KEYA, KEYB  
                        unsigned char key_sector)    // 0 <= key_sector <= 15  
{  
    unsigned char block = key_sector*4;  
	unsigned char status = MI_OK;  
  
   	// eeprom address calculation  
   	// 0x80 ... offset  
   	// key_sector ... sector  
   	// 0x18 ... 2 * 12 = 24 = 0x18  
    unsigned short e2addr = 0x80 + key_sector * 0x18;  
  
	
	if(auth_mode & KEYB)
	{
		auth_mode = PICC_AUTHENT1B;
		e2addr += 12;
	}
	else
	{
		auth_mode = PICC_AUTHENT1A;
	}
	FlushFIFO();	
   	ResetInfo(MInfo);  
    MSndBuffer[0] = (unsigned char)e2addr;  
    MSndBuffer[1] = (unsigned char)(e2addr>>8);  
   	MInfo.nBytesToSend   = 2;  
    // write load command  
    SetTimer = 10000L;  
	PcdSetTmo(3);
   	if ((status=PcdSingleResponseCmd(PCD_LOADKEYE2,
							MSndBuffer,
							MRcvBuffer,
							&MInfo)) != MI_OK)
	{  
		return status;
	}
     	// execute authentication 
	udelay(1000);
   	status = Mf500PiccAuthState(auth_mode,MIF_CSNR,block);  
   	return status;  
}  
  
/***********************************************************************/  
//  FUNCTION: MfPiccAuth  
//        IN: auth_mode PICC_AUTHENT1A or PICC_AUTHENT1B  
//            selects master key A or master key B  
//            KS0, KS1, KS2 selects one of the three master key sets.  
//            *snr       4 unsigned char serial number of the card, which should be  
//                       authenticated  
//            key_sector spec. the key RAM address from which the data should be
//                      taken (0..15)  
//            block     addresses the block address on the card, which shall be 
//                      authenticated. For MIFARE standard cards, addr can take 
//                      a value from 0 to 63, for other card types please refer 
//                      to the according product description.  
//       OUT: -  
//    RETURN:  
//   COMMENT: This function authenticates one card's sector (according to the  
//            block address addr) using the specified mode and master key,  
//            addressed with key_addr. After sending the command to the card the
//            function waits for the card's answer.  
//            This function is calling compatible (exept the serial number) with
//            older authentication functions. The keys are stored by the  
//            microcontroller, which is capable for the key management.  
/***********************************************************************/  
unsigned char Mf500PiccAuthentication(  unsigned char key_type,         // KEYA or KEYB  
                    unsigned char key_sector,
                    unsigned char *snr)      // key address in reader storage  
{  
    unsigned char	status = MI_OK;  
    unsigned char 	key[13];  
    unsigned char   	keycoded[12];  
    unsigned char    block = key_sector *  4;  
    unsigned char 	offset = 0;
	unsigned char 	index=0;
	unsigned char auth_mode;
	
	if(key_type == B_TYPE){
		offset = 6;  
		index = 0;//auth_mode-KEYB;
		auth_mode = PICC_AUTHENT1B;
	}
	else{
		index = 0;//auth_mode;
		auth_mode = PICC_AUTHENT1A;
	}
	
	
	memcpy(key,&MKeys[key_sector][index][offset],6);  
	

   	Mf500HostCodeKey(key,keycoded);  
    status = Mf500PiccAuthKey(auth_mode,  
                            snr,  
                            keycoded,  
                          	block);  
  	return status;  
}  
  
/***********************************************************************/  
//          	M A K E 12 B Y T E S  C R Y P T O 1 K E Y  
/***********************************************************************/  
unsigned char Mf500HostCodeKey(unsigned char *uncoded, 	// 6 unsigned chars key value uncoded  
                 unsigned char *coded)   	// 12 unsigned chars key value coded  
{  
   	unsigned char cnt = 0;  
    unsigned char ln  = 0;                 	// low nibble  
    unsigned char hn  = 0;                 	// high nibble  
  
    for (cnt = 0; cnt < 6; cnt++)
	{  
      	ln = uncoded[cnt] & 0x0F;  
      	hn = uncoded[cnt] >> 4;  
      	coded[cnt * 2 + 1]     =  (~ln << 4) | ln;  
      	coded[cnt * 2 ] =  (~hn << 4) | hn;  
   	}  
   	return MI_OK;  
}  
  
/***********************************************************************/  
//  FUNCTION: Mf500PiccAuthKey  
//        IN: auth_mode PICC_AUTHENT1A or PICC_AUTHENT1B  
//            selects master key A or master key B  
//            *snr       4 unsigned char serial number of the card, which should be  
//                       authenticated  
//            *keys      12 unsigned chars master key coded  
//            sector    addresses the sector number on the card, which shall be 
//                      authenticated. For MIFARE standard cards, addr can take 
//                      a value from 0 to 15, for other card types please refer 
//                      to the according product description.  
//       OUT: -  
//    RETURN:  
//   COMMENT: The keys are first loaded to the reader module and used for  
//            authentication of the secified sector. In order to get the  
//            required keys coded, the function Mf500HostCodeKey can be used.  
/***********************************************************************/  
unsigned char Mf500PiccAuthKey(unsigned char auth_mode,  
                     unsigned char *snr,  
                     unsigned char *keys,  
                     unsigned char block)  
{  
    unsigned char status     = MI_OK;  
 
	PcdSetTmo(106); 
	FlushFIFO();    // empty FIFO
   	ResetInfo(MInfo);  
    	memcpy(&MSndBuffer[0],keys,12);            	// write 12 unsigned chars of the key  
   	MInfo.nBytesToSend = 12;  

                            // write load command  
    if ((status=PcdSingleResponseCmd(PCD_LOADKEY,
							MSndBuffer,
							MRcvBuffer,&MInfo)) != MI_OK)
	{  
		return status;
	}
                                        	// execute authentication  
   	status = Mf500PiccAuthState(auth_mode,snr,block);  
   	return status;  
}  
  	
/////////////////////////////////////////////////////////////////////////  
//  FUNCTION: Mf500PiccAuthState  
//        IN: auth_mode  
//            snr  
//            sector  
//       OUT: -  
//    RETURN:  
//   COMMENT:  
/***********************************************************************/  
unsigned char Mf500PiccAuthState( unsigned char auth_mode,  
                        unsigned char *snr,  
                        unsigned char block)  
{  
   	unsigned char status = MI_OK;  
  
  	/** ¾ÆÁÖ Áß¿ä ÀÌ°ÍÀÌ ºüÁö¸é read½Ã bitcount °¡ 0 µÈ´Ù.*/
  	WriteIO(RegChannelRedundancy,0x07); // RxCRC disable,TxCRC, Parity enable
  	
  	PcdSetTmo(150); 
    //status = ReadIO(RegErrorFlag);// read error flags of the previous key load  
   	//if (status != MI_OK)
	//{  
      //  if (status & 0x40) return MI_KEYERR;// key error flag set  
       // return MI_AUTHERR;              	// generic authentication error  
   	//}  
    MSndBuffer[0] = auth_mode;      // write authentication command  
    MSndBuffer[1] = block;      	// write block number for authentication  
    memcpy(MSndBuffer + 2,snr,4);   // write 4 unsigned chars card serial number  
    ResetInfo(MInfo);  
    MInfo.nBytesToSend = 6;  	
 	
    	if ((status = PcdSingleResponseCmd(PCD_AUTHENT1,  
                               MSndBuffer,  
                               MRcvBuffer,  
                               &MInfo)) != MI_OK){  
        	return status;  
    	}  
  
    	if (ReadIO(RegSecondaryStatus) & 0x07)
	{										// RxLastBits muß leer sein  
        	return MI_BITCOUNTERR;  
    	}  
	else {
		WriteIO(RegChannelRedundancy,0x03); // RxCRC,TxCRC disable, Parity enable	
    		ResetInfo(MInfo);  
    		MInfo.nBytesToSend = 0;  
    		
    		if ((status = PcdSingleResponseCmd(PCD_AUTHENT2,  
                               MSndBuffer,  
                               MRcvBuffer,  
                               &MInfo)) == MI_OK){  
//#ifdef MF_DEBUG   	         	
//         		printk("Auth status 2= %d\n",status);
//#endif         	
            		if ( ReadIO(RegControl) & 0x08 ) // Crypto1 activated
            		{
//#ifdef MF_DEBUG   	            	
//            			printk("Authentication OK!!\n");
//#endif            	
                		status = MI_OK;
            		}
            		else
            		{
//#ifdef MF_DEBUG   	            	
//            			printk("Authentication ERROR!!\n");
//#endif            	
                		status = MI_AUTHERR;
            		}
         	}
//#ifdef MF_DEBUG   	         
//         printk("Auth status 3= %d\n",status);
//#endif         
	}

//#ifdef MF_DEBUG   	   
//   printk("Auth status 4= %d\n",status);
//#endif   
    return MI_OK;  
}  
  
/***********************************************************************/  

//        IN: addr    addresses -  the block on the card from which data shall  
//                    be read. For MIFARE standard cards, addr can take a value 
//                    from 0 to 63 (255 for Mifare Pro), for other card types  
//                    please refer to the according product description.  
//       OUT: data    is a pointer to the 16 unsigned char data block read from the card.
//       OUT:  
//    RETURN:  
//   COMMENT: This function reads a 16 unsigned char block from the specified card's 
//            block address addr. After sending the command to the card the 
//            function waits for the card's answer.  
/***********************************************************************/  
unsigned char Mf500PiccRead(unsigned char addr,unsigned char datalen,unsigned char *data)  
{  
    	unsigned char status = MI_OK;  
	char bitsExpected;
	int i;
	
	FlushFIFO();    // empty FIFO
	
	PcdSetTmo(640);             	// long timeout  
	
	WriteIO(RegChannelRedundancy,0x0F);//org 0x0f // RxCRC, TxCRC, Parity enable
	
   	ResetInfo(MInfo);  
    	MSndBuffer[0] = PICC_READ;         	// read command code  
   	MSndBuffer[1] = addr;  
   	MInfo.nBytesToSend   = 2;  	
  
        
   	status = PcdSingleResponseCmd(PCD_TRANSCEIVE,  
                       	MSndBuffer,  
                       	MRcvBuffer,  
                       	&MInfo);  
  
//printk("picc read status = %d bitcount=%d\n",status,MInfo.nBitsReceived);
  
	if (status != MI_OK)
   	{
      		if (status != MI_NOTAGERR ) // no timeout occured
      		{
         		if (MInfo.nBitsReceived == 8 && (ReadIO(RegDecoderControl) & 0x01))
         		{
            			// upper nibble should be equal to lower nibble, otherwise 
            			// there is a coding error on card side.
            			if ((MRcvBuffer[0] & 0x0f) == ((MRcvBuffer[0] >> 4) & 0x0f))
               				bitsExpected = 8;
            			else
            			{	
               				status = MI_CODINGERR;
               				bitsExpected = 0; // data is copied to output
            			}
         		}
         		else
            			bitsExpected = 4;  
            			
         		if (MInfo.nBitsReceived == bitsExpected)  // NACK
         		{
             			MRcvBuffer[0] &= 0x0f;  // mask out upper nibble
             			if ((MRcvBuffer[0] & 0x0a) == 0)
             			{
                			status = MI_NOTAUTHERR;
             			}
             			else
             			{
                			status = MI_CODEERR;
             			}
          		}
          		else
          		{
             			// return data - even if an error occured - for debugging reasons
             			if (MInfo.nBytesReceived >= datalen)
                			memcpy(data,MRcvBuffer,datalen);
             			else
             			{
                			memcpy(data,MRcvBuffer,MInfo.nBytesReceived);
                			for (i = MInfo.nBytesReceived; i < datalen; i++)
                			{
                   				data[i] = 0x00;
                			}
             			}
          		}
      		}
      		else
        		memcpy(data,"0000000000000000",datalen); // in case of an error initialise 
                                             // data
   	}
   	else   // Response Processing
   	{
  		
//   		printk("picc read bytecount = %d\n",MInfo.nBytesReceived);
   		
      		if (MInfo.nBytesReceived != datalen)
      		{
         		status = MI_BYTECOUNTERR;
         		// return data, even if an error occured
         		if (MInfo.nBytesReceived >= datalen)
            			memcpy(data,MRcvBuffer,datalen);
         		else
         		{
            			memcpy(data,MRcvBuffer,MInfo.nBytesReceived);             
            			for (i = MInfo.nBytesReceived; i < datalen; i++)
            			{
               				data[i] = 0x00;
            			}
         		}   
      		}
      		else
      		{
        		memcpy(data,MRcvBuffer,datalen);
      		}
   	}
   	return status;  
}  
  
/***********************************************************************/  
//  FUNCTION: Mf500PiccWrite  
//        IN: addr address -  the block on the card from which data shall  
//                 be read. For MIFARE standard cards, addr can take a  
//                 value from 0 to 63 (255 for Mifare Pro), for other card  
//                 types please refer to the according product description.  
//            data is a pointer to the 16 unsigned char data block that shall be  
//                 written to the card.  
//       OUT: -  
//    RETURN:  
//   COMMENT: This function writes a 16 unsigned char block to the specified card's 
//            block address addr. After sending the command to the card the 
//            function waits for the card's answer.  
/***********************************************************************/  
unsigned char Mf500PiccWrite( unsigned char addr,unsigned char *Wdata)  
{  
   	unsigned char status = MI_OK;
	
    ResetInfo(MInfo);  
    MSndBuffer[0] = PICC_WRITE;             	// Write command code  
    MSndBuffer[1] = addr;  
    MInfo.nBytesToSend   = 2;  
    SetTimer = 10000L;  
  
	PcdSetTmo(6);
    status = PcdSingleResponseCmd(PCD_TRANSCEIVE,  
                         MSndBuffer,  
                         MRcvBuffer,  
                         &MInfo);  
  
    if (status != MI_NOTAGERR)
	{
    	if (MInfo.nBitsReceived != 4)
		{          	// 4 bits are necessary  
			status =  MI_BITCOUNTERR;
    	}  
		else
		{
    		MRcvBuffer[0] &= 0x0f;                  	// mask out upper nibble  
    		if ((MRcvBuffer[0] & 0x0a) == 0)
			{  
        		status = MI_NOTAUTHERR;  
    		}  
			else
			{
    			if (MRcvBuffer[0] != 0x0a)
				{  
    				status = MI_CODEERR;  
    			}  
				else
				{
					status = MI_OK;
				}
			}
		}
	}
	if(status == MI_OK)
	{	
		FlushFIFO();
    	ResetInfo(MInfo);  
    	memcpy(MSndBuffer,Wdata,16);  
    	MInfo.nBytesToSend   = 16;  
    	SetTimer = 10000L;  
    	PcdSetTmo(10);                           // long timeout  
    	status = PcdSingleResponseCmd(PCD_TRANSCEIVE,  
                            MSndBuffer,  
                            MRcvBuffer,  
                            &MInfo);  
    	if (status & 0x80)
		{                 // timeout occured  
        	status = MI_NOTAGERR;  
		}
		else
		{
    		if (MInfo.nBitsReceived != 4)
			{  									// 4 bits are necessary  
				status = MI_BITCOUNTERR;  
				status = MI_OK;
    		}  
			else
			{
    			MRcvBuffer[0] &= 0x0f;      	// mask out upper nibble  
    			if ((MRcvBuffer[0] & 0x0a) == 0)
				{  
        			status = MI_WRITEERR;  
    			}  
    			else{
					if (MRcvBuffer[0] != 0x0a)
					{  
        				status = MI_CODEERR;  
    				} 
					else
					{
						status = MI_OK;
					}
				}
			}
		}
	}
    return status;  
}  
  
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

unsigned char Mf500PiccInc( unsigned char addr,unsigned long value)
{
   	unsigned char status = MI_OK;
	int i;

    ResetInfo(MInfo);
    MSndBuffer[0] = PICC_INC;             	// increment command code
    MSndBuffer[1] = addr;
    MInfo.nBytesToSend   = 2;
    SetTimer = 10000L;
	PcdSetTmo(3);
    status = PcdSingleResponseCmd(PCD_TRANSCEIVE,
                         MSndBuffer,
                         MRcvBuffer,
                         &MInfo);

    if (status == MI_NOTAGERR) return status;	// no timeout error
    if (MInfo.nBitsReceived != 4)
	{          									// 4 bits are necessary
        return MI_BITCOUNTERR;
    }
    MRcvBuffer[0] &= 0x0b;                  	// mask out upper nibble
    if ((MRcvBuffer[0] & 0x0a) == 0)
	{
        return MI_NOTAUTHERR;
    }
    if (MRcvBuffer[0] != 0x0a)
	{
    	return MI_CODEERR;
    }

    ResetInfo(MInfo);
    for(i=0;i<4;i++){
        MSndBuffer[3-i] =  *(((char *) &value) + i);
    }
    MInfo.nBytesToSend   = 4;
    SetTimer = 10000L;
	PcdSetTmo(10);
    status = PcdSingleResponseCmd(PCD_TRANSCEIVE,
                            MSndBuffer,
                            MRcvBuffer,
                            &MInfo);
    if (MInfo.nBitsReceived == 4)
	{  									// 4 bits are necessary
    	MRcvBuffer[0] &= 0x0f;      	// mask out upper nibble
    	if (MRcvBuffer[0] == 0x04)
		{
        	return  MI_EMPTY;
    	}
       	return MI_CODEERR;
    }
    return MI_OK;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
unsigned char Mf500PiccDec( unsigned char addr,unsigned long value)
{
   	unsigned char status = MI_OK;
	int i;

    ResetInfo(MInfo);
    MSndBuffer[0] = PICC_DEC;             	// decrement command code
    MSndBuffer[1] = addr;
    MInfo.nBytesToSend   = 2;
    SetTimer = 10000L;
	PcdSetTmo(3);
    status = PcdSingleResponseCmd(PCD_TRANSCEIVE,
                         MSndBuffer,
                         MRcvBuffer,
                         &MInfo);

    if (status == MI_NOTAGERR) return status;	// no timeout error
    if (MInfo.nBitsReceived != 4)
	{          									// 4 bits are necessary
        return MI_BITCOUNTERR;
    }
    MRcvBuffer[0] &= 0x0b;                  	// mask out upper nibble
    if ((MRcvBuffer[0] & 0x0a) == 0)
	{
        return MI_NOTAUTHERR;
    }
    if (MRcvBuffer[0] != 0x0a)
	{
    	return MI_CODEERR;
    }

    ResetInfo(MInfo);
    for(i=0;i<4;i++){
        MSndBuffer[3-i] =  *(((char *) &value) + i);
    }
    MInfo.nBytesToSend   = 4;
	
    SetTimer = 10000L;
    PcdSetTmo(10);                   	// short timeout
    status = PcdSingleResponseCmd(PCD_TRANSCEIVE,
                            MSndBuffer,
                            MRcvBuffer,
                            &MInfo);
    if (MInfo.nBitsReceived == 4)
	{  									// 4 bits are necessary
    	MRcvBuffer[0] &= 0x0f;      	// mask out upper nibble
    	if (MRcvBuffer[0] == 0x04)
		{
        	return  MI_EMPTY;
    	}
        return MI_CODEERR;
    }
    return MI_OK;
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

unsigned char Mf500PiccTrans(unsigned char addr)
{
    unsigned char status = MI_OK;

   	ResetInfo(MInfo);
    	MSndBuffer[0] =PICC_TRANS;          	// command code
   	MSndBuffer[1] = addr;
   	MInfo.nBytesToSend   = 2;
   	SetTimer = 10000L;
	PcdSetTmo(10);
   	status = PcdSingleResponseCmd(PCD_TRANSCEIVE,
                       	MSndBuffer,
                       	MRcvBuffer,
                       	&MInfo);

    if (status != MI_OK)
	{
        if (status == MI_NOTAGERR ) return status;
        if (MInfo.nBitsReceived != 4) return status;  	// NACK
    }
    MRcvBuffer[0] &= 0x0f;                      	// mask out upper nibble
    if ((MRcvBuffer[0] & 0x0a) != 0x0a)
	{
    	return MI_CODEERR;
    }
    return  MI_OK;

}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
unsigned char Mf500PiccRest( unsigned char addr)
{
   	unsigned char status = MI_OK;
	int i;
	
    ResetInfo(MInfo);
    MSndBuffer[0] = PICC_RESTORE;             	// decrement command code
    MSndBuffer[1] = addr;
    MInfo.nBytesToSend   = 2;
    SetTimer = 10000L;
	PcdSetTmo(3);
    status = PcdSingleResponseCmd(PCD_TRANSCEIVE,
                         MSndBuffer,
                         MRcvBuffer,
                         &MInfo);

    if (status == MI_NOTAGERR) return status;	// no timeout error
    if (MInfo.nBitsReceived != 4)
	{          									// 4 bits are necessary
        return MI_BITCOUNTERR;
    }
    MRcvBuffer[0] &= 0x0f;                  	// mask out upper nibble
    if ((MRcvBuffer[0] & 0x0a) == 0)
	{
        return MI_NOTAUTHERR;
    }
    if (MRcvBuffer[0] != 0x0a)
	{
    	return MI_CODEERR;
    }
	
    ResetInfo(MInfo);
    for(i=3;i>=0;i--){
        MSndBuffer[i] =  0;
    }

    MInfo.nBytesToSend   = 4;
    SetTimer = 10000L;
	PcdSetTmo(10);
    status = PcdSingleResponseCmd(PCD_TRANSCEIVE,
                            MSndBuffer,
                            MRcvBuffer,
                            &MInfo);
	if (MInfo.nBitsReceived == 4)
	{  									// 4 bits are necessary
    	MRcvBuffer[0] &= 0x0f;      	// mask out upper nibble
    	if (MRcvBuffer[0] == 0x04)
		{
        	return  MI_EMPTY;
    	}
       	return MI_CODEERR;
    }
    return MI_OK;
}

/***********************************************************************/  
//  FUNCTION: MfPcdLoadMk  
//        IN: kl_mode  PICC_AUTHENT1A or PICC_AUTHENT1B  
//                     selects master key A or master key B  
//                     KS0, KS1, KS2 selects one of the three master key sets.  
//            key_addr spec. the key RAM address to which the data shall be  
//                     written (0..15)  
//            mk       pointer to a 6 unsigned char authentication key  
//       OUT: -  
//    RETURN: MI_OK  
//   COMMENT: In order to be "calling compatibel" to former libraries, this  
//            function enables to store all keys in the microcontroller.  
//            When calling the authentication function "Mf500PiccAuthentication",  
//            these keys can be used.  
//            Note: This concept supports backward compatibility for the  
//            application software, but does not fulfill the same security  
//            level because keys are in this case stored autside the secure 
//            key memory of the MF RC500.  
/***********************************************************************/  
unsigned char Mf500PcdLoadKey2(unsigned char  auth_mode,	// KEYA or KEYB  
                  unsigned char  key_sector,   // 0 <= key_addr <= 15  
                  unsigned char  *mk)        // 6 unsigned chars uncoded master key  
{  
	unsigned char index;	
   	unsigned char offset = 0;
	
	index = auth_mode;
	if(auth_mode == KEYB ){
		index = auth_mode-KEYB;	
		offset = 6;  
 	} 
   	memcpy(&MKeys[key_sector][index][offset],mk,6);  
   	return MI_OK;  
}  
  
/***********************************************************************/  
//  FUNCTION: MfPcdLoadMk  
//        IN: kl_mode PICC_AUTHENT1A or PICC_AUTHENT1B  
//                    selects master key A or master key B  
//                    KS0, KS1, KS2 selects one of the three master key sets.  
//            key_addr spec. the key RAM address to which the data shall be  
//                     written (0..15)  
//            mk       pointer to a 6 unsigned char authentication key  
//       OUT: -  
//    RETURN: MI_OK  
//   COMMENT: In order to be "calling compatibel" to former libraries, this  
//            function enables to store all keys in the microcontroller.  
//            When calling the authentication function "Mf500PiccAuth",  
//            these keys can be used.  
//            Note: This concept supports backward compatibility for the  
//            application software, but does not fulfill the same security  
//            level because keys are in this case stored autside the secure 
//            key memory of the MF RC500.  
/***********************************************************************/  
unsigned char Mf500PcdLoadKeyE2(unsigned char  auth_mode,       // KEYA or KEYB  
                 unsigned char  key_sector,      // 0 <= key_addr <= 15  
                 unsigned char  *mk)       		// 6 unsigned chars uncoded master key  
{  
	unsigned char status,keycoded[12];  
                                        // eeprom address calculation  
                                        // 0x80 ... offset  
                                        // key_sector ... sector  
                                        // 0x18 ... 2 * 12 = 24 = 0x18  
    unsigned short e2addr = 0x80 + key_sector * 0x18;  
  
   	if (auth_mode & KEYB )
	{
        e2addr += 12;                   // key B offset  
    }  
    Mf500HostCodeKey(mk,keycoded);  
    status = PcdWriteE2(e2addr,12,keycoded);  
    return status;  
}  
  
/***********************************************************************/  
//  FUNCTION: Mf500PiccHalt  
//        IN: -  
//       OUT: -  
//    RETURN: -  
//   COMMENT: 
//   This function sets the MIFARE card into the halt state. After  
//   sending the command to the card the function waits for a cards  
//   response. As an exception, in this protocol step no ACK is sent by  
//   the card, but only a NACK in case of an error. Thus, the function  
//   is successful, if a time out occurs.  
/***********************************************************************/  
unsigned char Mf500PiccHalt(void)  
{  
    unsigned char status = MI_CODEERR;  
    
   	// Cmd Sequence 
   	ResetInfo(MInfo);  
    	MSndBuffer[0] = PICC_HALT; 	// Halt command code  
    	MSndBuffer[1] = 0x00;       // dummy address  
   	MInfo.nBytesToSend   = 2;  
   	SetTimer = 10000L;  
  
	PcdSetTmo(1);
   	status = PcdSingleResponseCmd(PCD_TRANSCEIVE,  
                       MSndBuffer,  
                       MRcvBuffer,  
                       &MInfo);  
    if (status)
	{  
     	// timeout error ==> no NAK received ==> OK  
        if (status == MI_NOTAGERR || status == MI_ACCESSTIMEOUT)
		{  
        	status = MI_OK;  
        }  
   	}  
    //reset command register - no response from tag  
   	WriteIO(RegCommand,PCD_IDLE);  
   	return status;  
}  
  
/***********************************************************************/  
//  FUNCTION: PcdSingleResponseCmd  
//        IN: cmd  PCD_IDLE...  
//            for a detailed description of the parameter values, please  
//            have a look on the header file of the reader register definitions.
//            send unsigned char stream of variable length, which should be send to the 
//            PICC, the length of stream has to be specified in the info 
//            - structure  
//       OUT: rcv  unsigned char stream of variable length, which was received  
//                 from the PICC or PCD  
//            info  communication and status structure  
//    RETURN:  
//   COMMENT: This function provides the central interface to the reader module.
//            Depending on the "cmd"-value, all necessary interrupts are enabled
//            and the communication is started. While the processing is done by //            the reader module, this function waits for its completion.  
//            It's notable, that the data in the "send unsigned char stream" is written  
//            to the FIFO of the reader module by the ISR. Immediate after  
//            enabling the interrupts, the LoAlert interrupt is activated.  
//            The ISR writes the data to the FIFO. This function is not involved
//            in writing or fetching data from FIFO, all work is done by the  
//            ISR.After command completion, the error status is evaluated and  
//            returned to the calling function.  
/***********************************************************************/  
unsigned char PcdSingleResponseCmd(unsigned char cmd,unsigned char *send,unsigned char *rcv, struct MfCmdInfo *info)  
{  
	//int i;
   	char          tmpStatus ;  	
 	char          status    = MI_OK; 
	unsigned char lastBits;
	unsigned char validErrorFlags = 0x1F; 
	unsigned char rxMultiple = 0x00;

	unsigned char irqEn     = 0x00;
	unsigned char waitFor   = 0x00;
	unsigned char timerCtl  = 0x02;
	//unsigned long cycleCnt  = 0x00;

	unsigned char waterLevelBackup;
	long            cycleCnt;
  

  	WriteIO(RegInterruptEn,0x7F);	// disable all interrupts  
    	WriteIO(RegInterruptRq,0x7F);   // reset interrupt requests  

    	FlushFIFO();               // flush FIFO buffer  

//printk("drv debug 1\n");  		
	//for(i=0;i<10000;i++){
	//while(1) {		
	//	if((ReadIO(RegSecondaryStatus) & 0x40))break;	
	//	udelay(1000);
	//}
	    cycleCnt = 0;
		while (((ReadIO(RegSecondaryStatus) & 0x40) == 0)){
			udelay(10);
			if (cycleCnt++ >= MAX_CYCLE_CNT) break;
		}
		if (cycleCnt >= MAX_CYCLE_CNT)   // reader has not terminated
		{  
			status = MI_ACCESSTIMEOUT;
			info->collPos = 0x00;
    		return status;         
		}
		
//printk("drv debug 2\n");  			
    	WriteIO(RegCommand,PCD_IDLE);   // terminate probably running command  

  
  	waterLevelBackup = ReadIO(RegFIFOLevel);
	WriteIO(RegFIFOLevel,0x20);
//printk("drv debug 3\n"); 	
                                    // save info structures to module pointers  
   	MpIsrInfo = info;  
   	MpIsrOut  = send;  
   	MpIsrIn   = rcv;  

//printk("drv debug 4 Info =0x%x\n",MpIsrInfo->nBytesToSend);      	
//printk("drv debug 4 send[0]=0x%x\n",MpIsrOut[0]);   
    	info->irqSource = 0x0;		// reset interrupt flags  
                                // depending on the command code,  
                                // appropriate interrupts are enabled(irqEn)  
                                // and the commit interrupt is choosen(waitFor)  
	switch(cmd)
	{
		case PCD_IDLE:                   // nothing else required
			irqEn = 0x00;
			waitFor = 0x00;
			break;
		case PCD_WRITEE2:                // LoAlert and TxIRq
			timerCtl = 0x00;              // start and stop timer manually            
			irqEn = 0x11;
			waitFor = 0x10;
			validErrorFlags &= ~0x08;     // hb 20.06.2001 don't check CRC errors         
			break;
		case PCD_READE2:                 // HiAlert, LoAlert and IdleIRq
			timerCtl = 0x00;              // start and stop timer manually            
			irqEn = 0x07;
			waitFor = 0x04;
			validErrorFlags &= ~0x08;     // wu 15.05.2001 don't check CRC errors
			break;
		case PCD_LOADKEYE2:              // IdleIRq and LoAlert
			timerCtl = 0x00;              // start and stop timer manually            
			validErrorFlags &= ~0x08;     // hb 20.06.2001 don't check CRC errors               
			irqEn = 0x05;
			waitFor = 0x04;
			break;
		case PCD_LOADCONFIG:             // IdleIRq and LoAlert
			timerCtl = 0x00;              // start and stop timer manually            
			irqEn = 0x05;
			waitFor = 0x04;
			break;
		case PCD_AUTHENT1:               // IdleIRq and LoAlert
			timerCtl = 0x05;
			irqEn = 0x05;
			waitFor = 0x04;
			break;
		case PCD_CALCCRC:                // LoAlert and TxIRq
			timerCtl = 0x00;              // start and stop timer manually            
			irqEn = 0x11;
			waitFor = 0x10;
			break;
		case PCD_AUTHENT2:               // IdleIRq
			irqEn = 0x04;
			waitFor = 0x04;
			break;
		case PCD_RECEIVE:                // HiAlert and IdleIRq
			info->nBitsReceived = -(ReadIO(RegBitFraming) >> 4);      
			timerCtl = 0x04;              // start timer manually and stop
                                       // with first bit received
			irqEn = 0x06;
			waitFor = 0x04;
			break;
		case PCD_LOADKEY:                // IdleIRq
			//printk("command loadkey\n");
			timerCtl = 0x00;              // start and stop timer manually            
			irqEn = 0x05;
			waitFor = 0x04;
			break;
		case PCD_TRANSMIT:               // LoAlert and IdleIRq
			timerCtl = 0x00;              // start and stop timer manually            
			irqEn = 0x05;
			waitFor = 0x04;
			break;
		case PCD_TRANSCEIVE:             // TxIrq, RxIrq, IdleIRq and LoAlert
			//printk("PCD CMD TRANSCEIVE\n");
			info->nBitsReceived = -(ReadIO(RegBitFraming) >> 4);
			irqEn = 0x3D;
			waitFor = 0x04;
			break;
		default:
			status = MI_UNKNOWN_COMMAND;
	}    

//printk("drv debug 5\n"); 
	//MpIsrInfo->waitFor = waitFor;
	//MpIsrInfo->status = MI_OK;    
		
//printk("debug sngl cmd 3 status = %d\n",status);	    
	if (status == MI_OK)
	{
		rxMultiple = ReadIO(RegDecoderControl) & 0x40;
		if (!rxMultiple)
			SetBitMask(RegDecoderControl,0x40);
			
			/*ÀÎÅÍ·´Æ® ¼­ºñ½º ·çÆ¾Àº ¼Õ´ëÁö ¸»°í
				¾Æ·¡ ºÎºÐÀ» Ãß°¡ Çß´õ´Ï 
				PCD_LOCKKEY command ½Ã Á¤»óÀûÀ¸·Î MI_OK °¡ ¸®ÅÏ µÈ´Ù.*/
    			irqEn |= 0x20;                          // always enable timout irq  
    			waitFor |= 0x20;
    				
//printk("debug sngl cmd 4\n");	         
			/*ÀÎÅÍ·´Æ® ¹ß»ý*/
			WriteIO(RegInterruptEn,irqEn | 0x80);  //necessary interrupts are enabled // count up from 1

			WriteIO(RegTimerControl,timerCtl);
			if (~timerCtl & 0x02) {// if no start condition could be detected, then
                              // start timer manually
				SetBitMask(RegControl,0x02);
			}
//printk("debug sngl cmd 5\n");	
			WriteIO(RegCommand,cmd);               //start command   

      			// wait for commmand completion
      			// a command is completed, if the corresponding interrupt occurs
      			// or a timeout is signaled  

        	cycleCnt = 0;
			while (!(MpIsrInfo->irqSource & waitFor))
			{
				udelay(10);
    			if (cycleCnt++ >= MAX_CYCLE_CNT) break;
			}
			if (cycleCnt >= MAX_CYCLE_CNT)   // reader has not terminated
			{  
				status = MI_ACCESSTIMEOUT;
				info->collPos = 0x00;
	    		return status;         
			}


                        // wait for cmd completion or timeout
//printk("debug sngl cmd 6\n");				
			SetBitMask(RegControl,0x04);         // stop timer now
			WriteIO(RegInterruptEn,0x7F);          // disable all interrupts
			WriteIO(RegInterruptRq,0x7F);          // clear all interrupt requests

			WriteIO(RegCommand,PCD_IDLE);          // reset command register
//printk("debug sngl cmd 7 count = %d\n",cycleCnt);
			//cycleCnt=0;
			//if (cycleCnt >= MAX_CYCLE_CNT)   // reader has not terminated
			//{  
			//	status = MI_ACCESSTIMEOUT;
			//	info->collPos = 0x00;         
			//}
			//else
			//{
//printk("debug sngl cmd 8\n");						
		/* original code ´Â ¾Æ·¡¿Í °°ÀÌ µÇ¾îÀÖ´Ù.
		ÀÌ ºÎºÐ¿¡¼­ ÀÎÅÍ·´Æ® ¹ß»ý½Ã »óÅÂ¿Í ¿¡·¯ Ã¼Å©ÀÇ ÇÃ·¡±×
		°ª »óÅÂÃ¼Å©¿¡¼­ ¹ß»ýÇÏ´Â ¹®Á¦ ¶§¹®¿¡ Authentication Ã³¸®¿¡¼­ 
		Á¤»óÀûÀÎ µ¿ÀÛÀ» ÇÏÁö ¸øÇÑ °á°ú°¡ »ý°å´Ù.*/
//				tmpStatus = ReadIO(RegErrorFlag) & validErrorFlags;
			
		/* À§ÀÇ ¿¡·¯ ·¹Áö½ºÅÍ¿¡¼­ ¹Ù·Î ÀÐÀº »óÅÂ°¡ À¯È¿ÇÑÁö¸¦ °Ë»çÇÏÁö ¾Ê°í
		ISR¿¡¼­ ¼³Á¤µÇ´Â ¿¡·¯ »óÅÂ¿Í Á¶ÇÕÇÏ¿© °á°ú¸¦ ³ªÅ¸³»µµ·Ï ¾Æ·¡¿Í °°ÀÌ ¼öÁ¤ÇÑÈÄ 
		Authentication ¹®Á¦´Â ¹ß»ýÇÏÁö ¾Ê¾Ò´Ù.2004.10.12*/

			if(cmd == PCD_AUTHENT2)
	      		status = MpIsrInfo->status;          // set status
      		
      		if (MpIsrInfo->irqSource & 0x20)     // if timeout expired - look at old error state
         		MpIsrInfo->errFlags |= MpIsrInfo->saveErrorState;
      		MpIsrInfo->errFlags &=  validErrorFlags;
      		tmpStatus = MpIsrInfo->errFlags;
				
				//status = MpIsrInfo->status;          // set status
				//MpIsrInfo->errFlags |= MpIsrInfo->saveErrorState;
				//MpIsrInfo->errFlags &=  validErrorFlags;
				//tmpStatus = MpIsrInfo->errFlags;
//printk("debug sngl cmd tmpStatus = 0x%x\n",tmpStatus);				
				if (tmpStatus) // error occured
				{
					if (tmpStatus & 0x01)   // collision detected
					{
						info->collPos = ReadIO(RegCollPos); // read collision position
						status = MI_COLLERR;
					}
					else
					{
						info->collPos = 0; 
						if (tmpStatus & 0x02)   // parity error
						{
                					//info->nBitsReceived = 0;          	// reject any received data  
                					//info->nBytesReceived = 0; 							
							status = MI_PARITYERR;
						}
					}
					if (tmpStatus & 0x04)   // framing error
					{
              					//info->nBitsReceived = 0;          	// reject any received data  
              					//info->nBytesReceived = 0; 						
						status = MI_FRAMINGERR;
					}
					else
					if (tmpStatus & 0x10)   // FIFO overflow
					{
						FlushFIFO();
                				//info->nBitsReceived = 0;          	// reject any received data  
                				//info->nBytesReceived = 0; 							
						status = MI_OVFLERR;
					}
					else
					if (tmpStatus & 0x08) // CRC error
					{
                				//info->nBitsReceived = 0;          	// reject any received data  
                				//info->nBytesReceived = 0; 						
						status = MI_CRCERR;
					}	
					else
					if (status == MI_OK)
						status = MI_NYIMPLEMENTED;
                		// key error occures always, because of 
                		// missing crypto 1 keys loaded
//printk("debug sngl cmd 8\n");	                		
				}
	             		// if the last command was TRANSCEIVE, the number of 
             			// received bits must be calculated - even if an error occured
				if (cmd == PCD_TRANSCEIVE || cmd == PCD_RECEIVE)
				{
                		// number of bits in the last byte
//printk("debug sngl cmd 9\n");	                		
					lastBits = ReadIO(RegSecondaryStatus) & 0x07;
//printk("debug sngl cmd 9 lastBits=%d\n",lastBits);						
					if (lastBits)
						info->nBitsReceived += (info->nBytesReceived-1) * 8 + lastBits;
					else
						info->nBitsReceived += info->nBytesReceived * 8;
				}
			//}
      
			if (!rxMultiple)
				ClearBitMask(RegDecoderControl,0x40);
		}		
		
		//printk("debug status = %d \n",status);
		
		//rINTMSK |= BIT_EINT6;
		//disable_irq(IRQ_EINT0); //tjkim
				
		MpIsrInfo = 0;         // reset interface variables for ISR
		MpIsrOut  = 0;
		MpIsrIn   = 0; 

   		// restore the previous value for the FIFO water level
		WriteIO(RegFIFOLevel, waterLevelBackup);

	return status;
}  
  
/***********************************************************************/  
//              	I N T E R R U P T   R O U T I N E  
/***********************************************************************/  

static void SingleResponseIsr(void)
{
	static unsigned char  irqBits;
	static unsigned char  irqMask;            
	static unsigned char  oldPageSelect;
	static unsigned char  nbytes;
	static unsigned char  cnt;
	static unsigned char errFlags;
   
	//printk("EINT6 RC531 interrupt\n");

	u32 rc531_inter_data = gpio_get_value( RC531_INTER_GPIO );

	if(!rc531_inter_data)
	{
		//printk("RC531 Interrupt Low\r\n");
	}
	else
	{
		//printk("RC531 Interrupt High\r\n");
	}

	if (MpIsrInfo && MpIsrOut && MpIsrIn)  // transfer pointers have to be set correctly
	{
		oldPageSelect = ReadRawIO(RegPage); // save old page select 
                                          // Attention: ReadIO cannnot be
                                          // used because of the internal
                                          // write sequence to the page 
                                          // reg
		errFlags = ReadIO(RegErrorFlag) & 0x0F; // save error state
		WriteRawIO(RegPage,0x80);           // select page 0 for ISR
      
		udelay(200);
      
		while( (ReadRawIO(RegPrimaryStatus) & 0x08)) // loop while IRQ pending
		{

			irqMask = ReadRawIO(RegInterruptEn); // read enabled interrupts

			// read pending interrupts
			irqBits = ReadRawIO(RegInterruptRq) & irqMask;

			MpIsrInfo->irqSource |= irqBits; // save pending interrupts
			
			//printk("IRQ Bit = 0x%x\n",irqBits);
			
			//************ LoAlertIRQ ******************
			if (irqBits & 0x01)    // LoAlert
			{
//				printk("LoAlertIRQ interrupt\n");
				nbytes = MFIFOLength - ReadRawIO(RegFIFOLength);
				// less bytes to send, than space in FIFO
				if ((MpIsrInfo->nBytesToSend - MpIsrInfo->nBytesSent) <= nbytes)
				{
					nbytes = MpIsrInfo->nBytesToSend - MpIsrInfo->nBytesSent;
					WriteRawIO(RegInterruptEn,0x01); // disable LoAlert IRQ
				}
                // write remaining data to the FIFO
				for ( cnt = 0;cnt < nbytes;cnt++)
                		{
					WriteRawIO(RegFIFOData,MpIsrOut[MpIsrInfo->nBytesSent]); 
					MpIsrInfo->nBytesSent++;
                		}
            
				WriteRawIO(RegInterruptRq,0x01);  // reset IRQ bit
			}
      
			//************* TxIRQ Handling **************
			if (irqBits & 0x10)       // TxIRQ
			{
//				printk("TxIRQ Handling interrupt\n");
         	
				WriteRawIO(RegInterruptRq,0x10);    // reset IRQ bit 
				WriteRawIO(RegInterruptEn,0x82);    // enable HiAlert Irq for
                                                // response
                                /* ±âÁ¸ ¹öÀüÀÇ µå¶óÀÌ¹ö¿¡¼­..*/
				//if (((MpIsrInfo->cmd & 0xF0) == 0x90) // if cmd is anticollision
						//&& (MpIsrInfo->nBitsReceived == 7)) // and 7 bits are known
				/*ver3.0µå¶óÀÌ¹ö¸¦ ÂüÁ¶*/
				if (MpIsrInfo->RxAlignWA) // if cmd is anticollision and 7 bits are known						
				{                                    // switch off parity generation
					WriteIO(RegChannelRedundancy,0x02); // RxCRC and TxCRC disable, parity disable               
					WriteRawIO(RegPage,0x00);  // reset page address
				}
			}
				//**************** RxIRQ Handling *******************************
			if (irqBits & 0x08) // RxIRQ - possible End of response processing
			{
				//printk("RxIRQ Handling interrupt\n");
         	
             // no error or collision during
				if (errFlags == 0x00)
				{
               				WriteRawIO(RegCommand,0x00); // cancel current command
					irqBits |= 0x04; // set idle flag in order to signal the end of
                                // processing. For single reponse processing, this
                                // flag is already set.
				}
				else // error occured - flush data and continue receiving
				{
					MpIsrInfo->saveErrorState = errFlags; // save error state
					WriteIO(RegControl,0x01); //FIFO fulsh
					WriteRawIO(RegPage,0x00);
					MpIsrInfo->nBytesReceived = 0x00;
					irqBits &= ~0x08; // clear interrupt request
					WriteRawIO(RegInterruptRq,0x08);
				}
			}

         		//************* HiAlertIRQ or RxIRQ Handling ******************
			if (irqBits & 0x0E) // HiAlert, Idle or valid RxIRQ
			{
            			// read some bytes ( length of FIFO queue)              
            			// into the receive buffer
				nbytes = ReadRawIO(RegFIFOLength);
			//	printk("FIFO length = %d\n",nbytes);
				//printk("HiAlertIRQ or RxIRQ Handling interrupt bytes %d\n",nbytes);
            			// read date from the FIFO and store them in the receive buffer
				do
				{
                			for ( cnt = 0; cnt < nbytes; cnt++)               
					{
                   			// accept no more data, than reserved memory space
						if (MpIsrInfo->nBytesReceived < MAX_RF_BUF_SIZE) 
						{
							MpIsrIn[MpIsrInfo->nBytesReceived] = ReadRawIO(RegFIFOData);
 							//printk("%2x ",MpIsrIn[MpIsrInfo->nBytesReceived]);
							MpIsrInfo->nBytesReceived++;
						}
						else
						{
							 MpIsrInfo->status = MI_RECBUF_OVERFLOW;
							break;
						}
					}
					// check for remaining bytes
					nbytes = ReadRawIO(RegFIFOLength);
				}
				while (MpIsrInfo->status == MI_OK && nbytes > 0);

				WriteRawIO(RegInterruptRq,0x0A & irqBits);  
                                       // reset IRQ bit - idle irq will
                                       // be deleted in a seperate section
			}   

         		//************* additional HiAlertIRQ Handling ***
			if (irqBits & 0x02)
			{
 			//printk("additional HiAlertIRQ Handling interrupt\n");
            		// if highAlertIRQ is pending and the receiver is still
            		// running, then the timeout counter should be stopped,
            		// otherwise a timeout could occure while receiving 
            		// correct data
				if ((ReadRawIO(RegPrimaryStatus) & 0x70) == 0x70)
				{      
					WriteRawIO(RegPage,0x81); // switch Page register
					cnt = ReadRawIO(RegControl); // read control register
					WriteRawIO(RegControl,cnt|0x04); // stop reader IC timer
                                                // write modified register
					WriteRawIO(RegPage,0x00); // reset Page Register
				}
			}

         		//************** additional IdleIRQ Handling ******
			if (irqBits & 0x04)     // Idle IRQ
			{
				//printk("additional IdleIRQ Handling interrupt = 0x%x\n",irqBits);
				WriteRawIO(RegInterruptEn,0x20); // disable Timer IRQ
				WriteRawIO(RegInterruptRq,0x24); // disable Timer IRQ request
				//WriteRawIO(RegInterruptRq,0x20); // disable Timer IRQ request
				irqBits &= ~0x20;   // clear Timer IRQ in local var
				MpIsrInfo->irqSource &= ~0x20; // clear Timer IRQ in info var
				//WriteRawIO(RegInterruptRq,0x04);
                                        // when idle received, then cancel
                                        // timeout
				MpIsrInfo->irqSource |= 0x04; // set idle-flag in case of valid rx-irq
								
            		// status should still be MI_OK
            		// no error - only used for wake up
			}
       
         		//************* TimerIRQ Handling ***********
			if (irqBits & 0x20)       // timer IRQ
			{
 				//printk("TimerIRQ Handling interrupt\n");
				WriteRawIO(RegInterruptRq,0x20); // reset IRQ bit 
				// only if no other error occured
				if (MpIsrInfo->status == MI_OK)
					MpIsrInfo->status = MI_NOTAGERR; // timeout error
                                        // otherwise ignore the interrupt
			}
		}
		WriteRawIO(RegPage,oldPageSelect | 0x80);
		}    
}


///////////////////////////////////////////////////////////////////////
//          R E A D   S N R   O F   R E A D E R   I C
///////////////////////////////////////////////////////////////////////
#if 0
static signed char mif_get_snr(unsigned char *snr)
{
	char status;
   	//unsigned long flags;
   	
	//save_flags_cli(flags);
	status = PcdReadE2(0x08,0x04,snr);
	//restore_flags(flags);	
	return status;
}
#endif
static signed char mif_get_pti(unsigned char *pti)
{
	char status;
   
	status = PcdReadE2(0x00,0x05,pti);
	return status;
}
  
/***********************************************************************/  
//     M F R C 5 0 0 I N I T I A L I Z E & DEFAULT KEYLOAD  
/***********************************************************************/  
unsigned char Mf500PcdInit(unsigned char KeyType)  
{  
    unsigned char i,status = 0;  

    unsigned char KeyA[7] = {0xa0,0xa1,0xa2,0xa3,0xa4,0xa5};  
    //unsigned char KeyB[7] = {0xb0,0xb1,0xb2,0xb3,0xb4,0xb5};  
    unsigned char KeyF[7] = {0xff,0xff,0xff,0xff,0xff,0xff};  
	
   	status = Mf500PcdConfig(); 
   	
    	if(status != MI_OK)return status; 
    	
    	rf_switch_on('A');
	//rf_switch_on('B');

	PcdRfReset(1);//1ms
	
    	if(KeyType == ATMEL_KEY){
    		for(i=15;i<16;i++){
        		status = Mf500PcdLoadKey2(KEYA,i,KeyA);  
        		//status = Mf500PcdLoadKey2(KEYB,i,KeyB);  
        		if(status != MI_OK)break;  
		}	
       		if(status != MI_OK)return status;
		
    		for(i=15;i<16;i++){
        		status = Mf500PcdLoadKey2(KEYA+1,i,KeyF);  
        		//status = Mf500PcdLoadKey2(KEYB+1,i,KeyF);  
        		if(status != MI_OK)break;  
		}	
       		if(status != MI_OK)return status;
		
    	}  
    	else if(KeyType == PHILLIPS_KEY){
    		for(i=15;i<16;i++){
        		//status = Mf500PcdLoadKey2(KEYA,i,KeyF);  
        		status = Mf500PcdLoadKey2(KEYA,i,KeyF);  
        		if(status != MI_OK)break;  
		}	
       		if(status != MI_OK)return status;
		
    		for(i=15;i<16;i++){
        		//status = Mf500PcdLoadKey2(KEYA+1,i,KeyA);  
        		status = Mf500PcdLoadKey2(KEYA+1,i,KeyF);  
        		if(status != MI_OK)break;  
		}	
       		if(status != MI_OK)return status;
    	} 
    	return status;  
}  


static unsigned char InitReader(void)
{
	char status = MI_OK;
	
	if((status = Mf500PcdInit(PHILLIPS_KEY)) == MI_OK)
		status = MI_OK;
	return (status);
}

/*#############################################################################*/
/*####################      standard I/O function        ######################*/
/*#############################################################################*/
//static void WriteIO(unsigned char Address, unsigned char value)
static ssize_t reader_write(struct file *file, const char *buffer, size_t Address, loff_t *posp)
{
	unsigned char value,addr;
	unsigned long flags;
	
	addr = (unsigned char)Address;
	//value = (unsigned char)buffer[0];
	copy_from_user(&value,buffer,1);
	//get_user(value, (int *)buffer);
	
	//printk("addr = 0x%x write val = 0x%x\n",addr,value);
	
	local_irq_save(flags);
	WriteRawIO(0x00,GetRegPage(addr));   // select appropriate page
	WriteRawIO(addr,value);              // write value at the specified 
        local_irq_restore(flags);                               // address	
	
	return 1;
}

//static unsigned char ReadIO(unsigned char Address)
static ssize_t reader_read(struct file *file,char *buffer,size_t Address,loff_t *posp)
{
	unsigned char value,addr;
	unsigned long flags;
	
	local_irq_save(flags);
		
	addr = (unsigned char)Address;
	
	/*ReadIO() ÇÔ¼ö¿Í °°´Ù.*/
	WriteRawIO(0x00,GetRegPage(addr));   // select appropriate page
	value = ReadRawIO(addr);              // read value at the specified 
	
	local_irq_restore(flags);
	
	//memcpy(&buffer[0],&value,1);
	copy_to_user(buffer,&value,1);
	
	//printk("debug read val = 0x%x\n",value);	
	
	return 1;	
}


static int reader_ioctl(struct inode *inode, struct file *file, unsigned int command, unsigned long arg)
{
	unsigned char atqA[2];//,atqB[12];
	unsigned char sak;
	
	unsigned char snr[4];
	unsigned char pti[5];	
	unsigned char pcd_buf[0x200];	
			
	signed char status = MI_OK;
	//unsigned char ret;
	struct reader_st temp;
	struct pcd_st	temp2;	
	
	static struct card_key key_ptr;	
	static struct card_data card_ptr;
	
//  printk(READER_NAME ": reader_ioctl: cmd=%d\n", command);

  	switch (command) 
	{
		case READER_HW_RESET: //2004.4.27
			pcd_rf_hw_reset();
			break;

  		case READER_CARD_DETECT:
  			copy_from_user(&key_ptr,(struct card_key *)arg,sizeof key_ptr);
  					
  			if(key_ptr.card_type == A_TYPE) {
  				
  				if((status = Mf500PiccActivateIdle(0,atqA,&sak,snr,4)) == MI_OK) {
//#ifdef MF_DEBUG  				
// 					printk("A Card Activate SAK = %d\n",sak);
//#endif  				//memcpy(MIF_CSNR,key_ptr.snrVal,4); //Å×½ºÆ®ÀÇ ¸ñÀûÀÌ´Ù.
					//printk("key_ptr.key_type = 0x%x key_ptr.sector = %d\n",key_ptr.key_type,key_ptr.sector_num);
  					if((status=Mf500PiccKeyLoad(key_ptr.key_type , key_ptr.sector_num , key_ptr.keyVal)) == MI_OK) {
  						if((status=Mf500PiccAuthentication(key_ptr.card_type, key_ptr.sector_num ,snr)) == MI_OK) {
  							memcpy(key_ptr.snrVal , snr , 4);
  							copy_to_user ((struct card_key *)arg, &key_ptr, sizeof key_ptr);
  						}
  					}
  				}
  			}
  			else if(key_ptr.card_type == B_TYPE){
  				//if((status = Mf500Picc_B_ActivateIdle(atqB,0,0)) == MI_OK) {
//#ifdef MF_DEBUG  				
// 					printk("B Card Activate SAK = %d\n",sak);
//#endif  		
					//printk("key_ptr.key_type = 0x%x key_ptr.sector = %d\n",key_ptr.key_type,key_ptr.sector_num);
  					//if((status=Mf500PiccKeyConfig(key_ptr.key_type , key_ptr.sector_num , key_ptr.keyVal)) == MI_OK) {
  					//	if((status=Mf500PiccAuthentication(key_ptr.key_type, key_ptr.sector_num ,uid)) == MI_OK) {
  					//		memcpy(key_ptr.snrVal , uid , 4);
  					//		copy_to_user ((struct card_key *)arg, &key_ptr, sizeof key_ptr);
  					//	}
  					//}
  					//memcpy(key_ptr.snrVal , atqB , 12);
  					//copy_to_user ((struct card_key *)arg, &key_ptr, sizeof(key_ptr);
  				//}
  			}
  			else
  				status = MI_NOTCARD;				
  			break;

    			//printk("kernel SNR : 0x%2x,0x%2x,0x%2x,0x%2x \n", snr[0],snr[1],snr[2], snr[3] );

    			//printk(READER_NAME ": reader_snr_ioctl: cmd=%d\n", command);
		  		
  		case READER_CARD_READ:
  			copy_from_user(&card_ptr,(struct card_data *)arg,sizeof card_ptr);
//#ifdef MF_DEBUG  			
//  			printk("card_ptr.block_num = %d\n",card_ptr.block_num);
//#endif  			
  			
  			if((status = Mf500PiccRead(card_ptr.block_num,16,card_ptr.data_ptr)) == MI_OK)
  			{
  				copy_to_user ((void *)arg, &card_ptr, sizeof card_ptr);
  			}
  			  			
  			//if((status = Mf500PiccRead(60,rdata_buf)) == MI_OK)
  			//{
  			//	copy_to_user ((void *)arg, rdata_buf, sizeof(rdata_buf));
  			//}
    			break;
    			
  		case READER_PTI_GET:
    			if((status = mif_get_pti(pti)) == MI_OK) {
    				copy_to_user ((void *)arg, pti, 5);
    			//printk(READER_NAME ": reader_pti_ioctl: cmd=%d\n", command);
    				break;
    			}  	
    			else status = -EFAULT;
    			break;
		case READER_SNG_RSP_SET:
			copy_from_user(&temp,(struct reader_st *)arg,sizeof temp);
			//printk("reader_st size =%d\n",sizeof temp);
			//status = PcdSingleResponseCmd(PCD_TRANSCEIVE, MSndBuffer,	MRcvBuffer, &MInfo);
			memcpy(MSndBuffer,temp.byTxBuffer, sizeof(MSndBuffer));
			
			memcpy((void *)&MInfo,(const void *)&temp.MInfo, sizeof(MInfo));
			//printk("pcd cmd = 0x%x tx_buf = 0x%x\n",temp.pcd_cmd , MSndBuffer[0]);
			//printk("MInfo 1= %d MInfo 2 = %d\n",MInfo.nBytesToSend,MInfo.DisableDF);
			//status = PcdSingleResponseCmd(temp.pcd_cmd, temp.byTxBuffer, temp.byRxBuffer, &temp.MInfo);
			status = PcdSingleResponseCmd(temp.pcd_cmd, MSndBuffer,MRcvBuffer,&MInfo);
			
			memcpy(temp.byRxBuffer, MRcvBuffer , sizeof(MRcvBuffer));
			//temp.byRxBuffer[0] = MRcvBuffer[0];
			//printk("rx buf read = 0x%x\n",temp.byRxBuffer[0]);
			memcpy((void *)&temp.MInfo, (const void *)&MInfo , sizeof(MInfo));			
			//temp.MInfo.nBytesReceived = MInfo.nBytesReceived;
			//printk("MInfo read = %d\n",temp.MInfo.nBytesReceived);
			copy_to_user((struct reader_st *)arg , &temp , sizeof temp);
			//if(status != MI_OK) return -EFAULT;
			break;
			
		case READER_PCD_RD_E2:
			//status = PcdReadE2(0x08,0x04,snr);
			copy_from_user(&temp2 ,(struct pcd_st *)arg, sizeof temp2);
			//printk("start = 0x%x length = 0x%x\n",temp2.startaddr,temp2.length);
			
			status = PcdReadE2(temp2.startaddr , temp2.length , pcd_buf);
			if(status != MI_OK) return -EFAULT;
			
			//printk("SNR : 0x%02x,0x%02x,0x%02x,0x%02x \n", pcd_buf[0],pcd_buf[1],pcd_buf[2], pcd_buf[3] );
			//memcpy(temp2.data , pcd_buf , sizeof(pcd_buf));
			memcpy(temp2.data , pcd_buf , temp2.length);
			copy_to_user((struct pcd_st *)arg , &temp2, sizeof temp2);			
			break;
			
		case READER_PCD_WR_E2:
			copy_from_user(&temp2 ,(struct pcd_st *)arg, sizeof temp2);	
			status = PcdWriteE2(temp2.startaddr,temp2.length, temp2.data);					
			if(status != MI_OK) return -EFAULT;
			break;
		
		case READER_PCD_RF_RST:
			PcdRfReset(arg);
			break;
			
		case READER_PCD_HALT:
			Mf500PiccHalt();
			break;
			
		case READER_RST_INFO:
			ResetInfo(MInfo);
			break;	
		case READER_MAP_KEY_GET:
			copy_to_user((unsigned char *)arg , MKeys, sizeof(MKeys));
			break;				
		default :
			return -EFAULT;    		
  	} 
  	/* invalid command type */
  	return (char)status;
}

static int reader_open(struct inode *inode, struct file *file)
{
	char status = MI_OK;

   	if (rf_DeviceOpen) {
       	//printk(READER_NAME " : Device is already opened\n");
       	return -EBUSY;
   	}

	++rf_DeviceOpen;

	//MOD_INC_USE_COUNT;

//  	printk(READER_NAME ": reader_open\n");

	//if((status = Mf500PcdConfig()) == MI_OK)
	//	status = MI_OK;
	//return (status); //¼º°øÇÏ¸é 0 ½ÇÆÐÇÏ¸é -1À» ¸®ÅÏ
	//if((status = InitReader())== MI_OK)
	//	printk("M I F A R E   M O D U L E Configuration done\n");
	//else return -1;	 
	
  	/* everything OK */
  	return status;
}

static int reader_release(struct inode *inode, struct file *file)
{
	char status = 0;

   	if (!rf_DeviceOpen) {
       	//printk(READER_NAME " : Device has not opened\n");
       	return -EINVAL;
   	}

   	--rf_DeviceOpen;
	
  	//MOD_DEC_USE_COUNT;

	if((status=Mf500PiccHalt())==MI_OK)
		return 0;
	else
		return -1;
}


/*
 * initialization
 *
 */

static struct file_operations reader_fops =
{
	read:		reader_read,		
	write:		reader_write,
	ioctl:		reader_ioctl,
	open:		reader_open,
	release:	reader_release,
};

static void reader_version(void)
{

	printk("MF RC531 reader driver version 1.0\n");
}

int __init rc531_reader_init(void)
{
	int err , ret;
	
  	//printk(READER_NAME ": rc531_reader_init\n");

  	hw_rf_rc531_cs_base = ioremap(DIV_Y3_RFMALE , 0x0040);

  	//printk("[ RC531 VA ADDR : 0x%x ]\r\n", hw_rf_rc531_cs_base );
	
   	reader_version();
   
	rc531_initialize();   
	
	ResetInfo(MInfo); 

	if(InitReader()== MI_OK)
		printk("M I F A R E   M O D U L E Configuration done\n");
	else {
		printk("M I F A R E   M O D U L E not configured!!\n");
	 	return -1;	 
	 }	

    gpio_request(S5PV210_GPH1(3), "GPH13");
    s3c_gpio_cfgpin(S5PV210_GPH1(3), S3C_GPIO_SFN(0xf));
    s3c_gpio_setpull(S5PV210_GPH1(3), S3C_GPIO_PULL_NONE);

  	/* register device */
  	if ((err = register_chrdev(READER_MAJOR, READER_NAME, &reader_fops)) != 0)
    	{
      	/* log error and fail */
      		printk(READER_NAME ": unable to register major device %d\n", READER_MAJOR);
      		return(err);
    	}

        if ((ret = request_irq( IRQ_EINT11, (void *)SingleResponseIsr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, READER_NAME, NULL))) {
            	printk(READER_NAME": couldn't allocate IRQ (errno %d)\n", ret);
        	return -EBUSY;
	}

	//enable_irq(IRQ_GPIO(90));
	
  	/* log device registration */
  	//printk("MF RC531 driver is registered with Major Number =%d \n", READER_MAJOR);
	
  	/* everything OK */
  	return(0);
}

void __exit rc531_reader_exit(void)
{

  	/* log device unload */
  	//printk(READER_NAME " unloading module\n");
  	
	free_irq(IRQ_EINT11, NULL);
	
  	/* unregister driver */
  	unregister_chrdev(READER_MAJOR, READER_NAME);

}

module_init(rc531_reader_init);
module_exit(rc531_reader_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TJKIM <prodalma@astonlinux.com>");
MODULE_DESCRIPTION("MFRC531 Contactless Card Reader driver for S3C2410");
