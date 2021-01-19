//	@(#)korail_rc531_bypass.c	1.0f	2014/08/21	(SOLIDTEK)

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

#include "rfio.h"

#define dispmsg(T,args...)	if(T) printk(args)

#define RC531_INTER_GPIO	S5PV210_GPH1(3)
#define GetRegPage(adr)     	(0x80 | (adr>>3))

volatile byte *rc531_base_cs;

#define RC531_ADDR_PTR		1
#define RC531_BASE		0xE0842800

static rfinf_t 	rfinf;
static rfinf_t 	*isrInf;
static byte	*isrSnd;
static byte	*isrRcv;
static int	rfT = 0;   // kernel trace
static int 	irqTest = 0;
static int	irqCount = 0;
static int	oldPage = 0;
static int 	isrFlag = 0;

static byte 	sendBuf[512];  
static byte 	recvBuf[512];  

extern void RC531_ISR(void);

void rfint_enable(void)
{
	irqCount++;
	enable_irq(IRQ_EINT11);
}

void rfint_disable(void)
{
	for ( ; irqCount > 0 ; irqCount--)
		disable_irq(IRQ_EINT11);
}

byte rfraw_read(byte addr)
{
#if RC531_ADDR_PTR
	return(__raw_readb(rc531_base_cs + (addr << 1)));
#else
	return(*(volatile byte *)(RC531_BASE + (addr << 1)));
#endif
}

void rfraw_write(byte addr, byte data)
{
#if RC531_ADDR_PTR
	__raw_writeb(data, rc531_base_cs+(addr << 1));
#else
	*(volatile byte *)(RC531_BASE + (addr << 1)) = data;
#endif
}

#if RC531_ADDR_PTR
byte rfreg_read(byte addr)
{
	int newpage = addr / 8;

	if (oldPage == newpage)
		return(__raw_readb(rc531_base_cs+(addr << 1)));

	oldPage = newpage;
	__raw_writeb(GetRegPage(addr), rc531_base_cs);	
	return(__raw_readb(rc531_base_cs+(addr << 1)));
}

void rfreg_write(byte addr, byte data)
{
	int newpage = addr / 8;

	if (oldPage != newpage) {
		oldPage = newpage;
		__raw_writeb(GetRegPage(addr), rc531_base_cs);
	}
	__raw_writeb(data, rc531_base_cs+(addr << 1));
}

#else

byte rfreg_read(byte addr)
{
	int newpage = addr / 8;

	if (oldPage == newpage)
		return(*(volatile byte *)(RC531_BASE + (addr << 1)));

	oldPage = newpage;
	*(volatile byte *)(RC531_BASE) = GetRegPage(addr);
	return(*(volatile byte *)(RC531_BASE + (addr << 1)));
}

void rfreg_write(byte addr, byte data)
{
	int newpage = addr / 8;

	if (oldPage != newpage) {
		oldPage = newpage;
		*(volatile byte *)(RC531_BASE) = GetRegPage(addr);
	}
	*(volatile byte *)(RC531_BASE + (addr << 1)) = data;
}
#endif

void rfreg_setbit(byte addr, byte mask)
{
	byte data;

	data = rfreg_read(addr);
	data |= mask;
	rfreg_write(addr, data);
}

void rfreg_clrbit(byte addr, byte mask)
{
	byte data;
	
	data = rfreg_read(addr);
	data &= ~mask;
	rfreg_write(addr, data);
}

void rfpin_setbit(byte data)
{
	if (data)
		gpio_set_value( S5PV210_GPG1(2), 1);
	else
		gpio_set_value( S5PV210_GPG1(2), 0);
}

int chk_expire(struct timeval *otm)
{
	struct timeval ntm;

	do_gettimeofday(&ntm);
	if (otm->tv_sec > ntm.tv_sec) 
	{
		ntm.tv_usec += 1000000;
		ntm.tv_sec--;
	}
	return(((ntm.tv_sec-otm->tv_sec)*1000)+(ntm.tv_usec-otm->tv_usec)/1000);
}

void time_delay(dword data)
{
	struct timeval tval;

	// 1ms 단위 지연 함수이지만 기준값의 두배의 지연이 발생함(확인 요망) 
	do_gettimeofday(&tval);
	while (chk_expire(&tval) < data)
		;
}

void tick_delay(dword data)
{
#if 1
	// a tick time = 8ms (HZ값이 250이면 4ms임=>확인이 필요한 사항)
	u64 endtime = get_jiffies_64() + data;	
	while (jiffies < endtime)
		;
#else
	unsigned long timeout;
	
	timeout = jiffies + data;
	dispmsg(rfT, "HZ(%u), jiffies(%u), timeout(%u)\n", HZ, jiffies, timeout);
	while (time_after(jiffies, timeout))
		; // delay
#endif
}

void rfirq_test(dword cnt)
{
	int i, irqreq, isrcnt;

	irqTest = 1;
	//printk("RC531 IRQ Test.\n");
	rfint_disable();
	rfpin_setbit(1);
	irqreq = 0;
	isrFlag = 0;
	for (i = 0 ; (i < cnt || cnt == 0) ; i++) 
	{
		isrcnt = 0;
		mdelay(100);
		rfint_enable();
		do 
		{
			if (irqreq == 0) 
			{
				irqreq = 1;
				dispmsg(rfT,"1");
				rfpin_setbit(0);	// irq request...
			}

			if (irqreq && isrFlag)
			{
				irqreq = 0;
				isrFlag = 0;
				dispmsg(rfT,"3");
				isrcnt++;
			}
		} while (isrcnt < 3);
		rfint_disable();
	}

	rfpin_setbit(0);
	irqTest = 0;
}
	
static int RF_RC531_interface_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int RF_RC531_interface_release(struct inode *inode, struct file *filp)
{
	return 0;
}                         

ssize_t RF_RC531_interface_write(struct file *filp, const char *buf, size_t length, loff_t *f_pos)
{
	byte wbuf[16];	
	byte cmd;
	byte addr;
	byte data;

	copy_from_user(wbuf, buf, length);	
	
	cmd = wbuf[0];
	addr = wbuf[1];
	data = wbuf[2];

	switch (cmd) {
		case 'N': 
			rfraw_write(addr, data); 
			break;
		case 'P':
			rfreg_write(addr, data);
			break;
		case 'C': // bit clear
			rfreg_clrbit(addr, data);
			break;
		case 'S': // bit set
			rfreg_setbit(addr, data);
			break;
		case 'I':
			isrFlag = data;
			break;
		case 'D':
			tick_delay(data);
			break;
		case 'T':
			rfT = data;
			break;
		default : break;
	}
	return length;
}

ssize_t RF_RC531_interface_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	byte rbuf[16];
	byte cmd;
	byte addr;
	byte data = 0;

	copy_from_user(rbuf, buf, count);
	
	cmd = rbuf[0];
	addr = rbuf[1];

	if (cmd == 'N') 
		data = rfraw_read(addr);
	else if (cmd == 'P')  
		data = rfreg_read(addr);
	else if (cmd == 'I')
		data = isrFlag;

	rbuf[2] = data;
	
	copy_to_user(buf , rbuf, 4);
	return count;
}

void rfinf_reset(void)
{
	memset(&rfinf, 0x00, sizeof(rfinf_t));
}

void rfisr_reset(void)
{
	isrInf = 0;
	isrSnd = 0;
	isrRcv = 0;
	isrFlag = 0;
}

void rfifo_flush(void) 
{
	rfreg_setbit(RegControl,0x01);
}

// rf-module command transceive
int rfmod_trans(byte cmd, byte *send, byte *rcv, rfinf_t *info)
{
	int	err = 0;
	int	tmperr = 0;
	int	lastbit;
	int 	irqen;
	int 	waitfor;
	int	timeout;
	struct  timeval tval;

	rfreg_write(RegInterruptEn,0x7F);
	rfreg_write(RegInterruptRq,0x7F);
	rfreg_write(RegCommand,PCD_IDLE);
	
	rfifo_flush(); 
	isrInf = info;
	isrSnd = send;
	isrRcv = rcv;
	
	info->irqsrc = 0x0; 
	switch(cmd)
	{
		case PCD_IDLE:      
			irqen = 0x00;
			waitfor = 0x00;
			break;
		case PCD_WRITEE2:    
			irqen = 0x11;
			waitfor = 0x10;
			break;
		case PCD_READE2:      
			irqen = 0x07;
			waitfor = 0x04;
			break;
		case PCD_LOADCONFIG:  
		case PCD_LOADKEYE2: 
		case PCD_AUTHENT1:     
			irqen = 0x05;
			waitfor = 0x04;
			break;
		case PCD_CALCCRC:         
			irqen = 0x11;
			waitfor = 0x10;
			break;
		case PCD_AUTHENT2:         
			irqen = 0x04;
			waitfor = 0x04;
			break;
		case PCD_RECEIVE:           
			info->bitsrecv = -(rfreg_read(RegBitFraming) >> 4);
			irqen = 0x06;
			waitfor = 0x04;
			break;
		case PCD_LOADKEY:            
			irqen = 0x05;
			waitfor = 0x04;
			break;
		case PCD_TRANSMIT:            
			irqen = 0x05;	// 0000-0101
			waitfor = 0x04; // 0000-0100
			break;
		case PCD_TRANSCEIVE:           
			info->bitsrecv = -(rfreg_read(RegBitFraming) >> 4);
			irqen = 0x3D; 	// 0011-1101
			waitfor = 0x04; // 0000-0100
			break;
		default:
			err = MI_UNKNOWN_COMMAND;
	}

	if (err) 
	{
		isrInf = 0;         
		isrSnd = 0;
		isrRcv = 0;

		info->retpos = 0x1000;
		info->err = err;
		return(err);
	}

	irqen |= 0x20;     
	waitfor |= 0x20; 
	isrFlag = 0;
	info->isrcnt = 0;
	timeout = info->timeout/2; // 시스템 오류로 시간값이 2배로 지연됨
	if (!timeout)
		timeout = 2; 

	do_gettimeofday(&tval);
	//rfint_enable();
	rfreg_write(RegInterruptEn,irqen | 0x80);
	rfreg_write(RegCommand,cmd);
#if 0
	while (chk_expire(&tval) < timeout)
	{	// interrupt processing...
		if (info->irqsrc & waitfor)
			break;
	}
#else
	while(!(info->irqsrc & waitfor)) 
	{
		if (isrFlag > info->isrcnt) 
			RC531_ISR();

		if ((chk_expire(&tval) >= timeout))
			break;
	}
#endif
	//rfint_disable();
	rfreg_write(RegInterruptEn,0x7F);       
	rfreg_write(RegInterruptRq,0x7F);
	rfreg_setbit(RegControl,0x04);        
	rfreg_write(RegCommand,PCD_IDLE);

	if (!(info->irqsrc & waitfor)) {
		info->retpos = 0x2000;
		err = MI_ACCESSTIMEOUT;
	}
	else {
		info->retpos = 0x4000;
		err = isrInf->err;
	}

	if (err) {
		isrInf = 0;         
		isrSnd = 0;
		isrRcv = 0;
		info->collpos = 0x00;
		info->err = err;
		return(err);
	}

	tmperr = rfreg_read(RegErrorFlag) & 0x17;
	if (tmperr) 
	{
		info->retpos |= 0x100;	// 0x4100
		if (tmperr & 0x01)   			   
		{
			info->retpos |= 0x01;
			info->collpos = rfreg_read(RegCollpos); 
			err = MI_COLLERR;
		}
		else
		{
			info->retpos |= 0x02;
			info->collpos = 0;
			if (tmperr & 0x02) 
			{   	
				info->retpos |= 0x04;
				err = MI_PARITYERR;
			}
		}

		if (tmperr & 0x04) 
		{   	
			info->retpos |= 0x08;
			err = MI_FRAMINGERR;
		}

		if (tmperr & 0x10)   
		{
			info->retpos |= 0x10;
			rfifo_flush();
			err = MI_OVFLERR;
		}

		if (tmperr & 0x08) 
		{ 	
			info->retpos |= 0x20;
			err = MI_CRCERR;
		}

		if (err == 0) 
		{
			info->retpos |= 0x40;
			err = MI_NY_IMPLEMENTED;
		}
	}
	else 
		info->retpos |= 0x200;
	
	if (cmd == PCD_TRANSCEIVE)
	{
		lastbit = rfreg_read(RegSecondaryStatus) & 0x07;
		if (lastbit) { 
			info->bitsrecv += (info->byterecv-1) * 8 + lastbit;
			info->retpos |= 0x400;
		}
		else {
			info->bitsrecv += info->byterecv * 8;
			info->retpos |= 0x800;
		}
	}

	isrInf = 0;         
	isrSnd = 0;
	isrRcv = 0;
	info->err = err;
	return(err);
}

int RF_RC531_interface_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int	err = 0, data;

	switch (cmd) {
		case __IOCTL_RC531_INT_ENA__:
			rfint_enable();	
			break;
		case __IOCTL_RC531_INT_DIS__:
			rfint_disable();
			break;
		case __IOCTL_RC531_ISR_RST__:
			rfisr_reset();
			break;
		case __IOCTL_RC531_INF_RST__:
			rfinf_reset();
			break;
		case __IOCTL_RC531_DLY_TEST__:
			copy_from_user(&data, (long *)arg, sizeof(long));
			//tick_delay(data);
			time_delay(data);
			break;
		case __IOCTL_RC531_IRQ_TEST__:
			copy_from_user(&data, (int *)arg, sizeof(int));
			rfirq_test(data);
			break;
		case __IOCTL_RC531_RST_HIGH__:
			rfpin_setbit(1);
			break;
		case __IOCTL_RC531_RST_LOW__:
			rfpin_setbit(0);
			break;
		case __IOCTL_RC531_CMD_TRX__:
			copy_from_user(&rfinf, (rfinf_t *)arg, sizeof(rfinf_t));
			copy_from_user(sendBuf, rfinf.sendbuf, rfinf.bytesend);
			
			err = rfmod_trans(rfinf.cmd, sendBuf, recvBuf, &rfinf);
			
			copy_to_user(rfinf.recvbuf, recvBuf, rfinf.byterecv);
			copy_to_user((rfinf_t *)arg, &rfinf, sizeof(rfinf_t));
			break;
		default:
			err = -EFAULT;
		    	break; 
	} 

	return err;  
}

irqreturn_t RC531_ISR_FUNC(int irq, void *dev_id)
{
	isrFlag++;
	if (irqTest) 
	{
		rfpin_setbit(1);	
		dispmsg(rfT, "2");
	}

	return IRQ_HANDLED;
}

void RC531_ISR(void)
{
	static byte irqBits;
	static byte irqMask;
	static byte nbytes;
	static byte cnt;
	int    count = 0;

	if (!(isrInf && isrSnd && isrRcv)) 
		return;

	isrInf->isrcnt++;
	isrInf->isrpos |= 0x01;
	while(rfreg_read(RegPrimaryStatus) & 0x08)
	{
		isrInf->isrpos |= 0x02;
		irqMask = rfreg_read(RegInterruptEn); 
		irqBits = rfreg_read(RegInterruptRq) & irqMask;
		isrInf->irqsrc |= irqBits; 
		if (irqBits & 0x01)        /* LoAlert  */
		{
			isrInf->isrpos |= 0x04;
			nbytes = 64 - rfreg_read(RegFIFOLength); 
			if ((byte)(isrInf->bytesend - isrInf->bytesent) <= nbytes)
			{
				nbytes = isrInf->bytesend - isrInf->bytesent;
				rfreg_write(RegInterruptEn,0x01); 
			}
			for (cnt = 0; cnt < nbytes; cnt++)
			{
				rfreg_write(RegFIFOData,isrSnd[isrInf->bytesent]);
				isrInf->bytesent++;
			}
			rfreg_write(RegInterruptRq,0x01);  
		}

		if (irqBits & 0x10)       /* TxIRQ */
		{
			isrInf->isrpos |= 0x08;
			rfreg_write(RegInterruptRq,0x10);    
			rfreg_write(RegInterruptEn,0x82);
			if (isrInf->piccmd == PICC_ANTICOLL1)
				rfreg_write(RegChannelRedundancy,0x02);
		}

		if (irqBits & 0x0E)        /* HiAlert, Idle or RxIRQ */
		{ 
			isrInf->isrpos |= 0x10;
			nbytes = rfreg_read(RegFIFOLength);
			for (cnt = 0; cnt < nbytes; cnt++)
			{
				isrRcv[isrInf->byterecv] = rfreg_read(RegFIFOData);
				isrInf->byterecv++;
			}
			rfreg_write(RegInterruptRq,0x0A & irqBits);
		}
	
		if (irqBits & 0x04)        /* Idle IRQ    */
		{
			isrInf->isrpos |= 0x20;
			rfreg_write(RegInterruptEn,0x20); 
			rfreg_write(RegInterruptRq,0x20); 
			irqBits &= ~0x20;   		
			isrInf->irqsrc &= ~0x20;
			rfreg_write(RegInterruptRq,0x04); 
		}

		if (irqBits & 0x20)        /* timer IRQ    */
		{
			isrInf->isrpos |= 0x40;
			rfreg_write(RegInterruptRq,0x20);
			isrInf->err = MI_NOTAGERR;
		}

		if (++count >= 20)
		{
			isrInf->isrpos |= 0x80;
			break;
		}
	}
}

static struct file_operations RF_RC531_interface_fops = {
	.owner      =   THIS_MODULE,
	.read       =   RF_RC531_interface_read,
	.write      =   RF_RC531_interface_write,
	.open       =   RF_RC531_interface_open,
	.release    =   RF_RC531_interface_release,
	.ioctl      =   RF_RC531_interface_ioctl,
};


int __init rc531_reader_init(void)
{
	int err , ret;

	irqCount = 0;
	rfisr_reset();
	rc531_base_cs = ioremap(DIV_Y3_RFMALE , 0x0040);

	/* register device */
	if ((err = register_chrdev(READER_MAJOR, READER_NAME, &RF_RC531_interface_fops)) != 0)
	{
		/* log error and fail */
		printk(READER_NAME ": unable to register major device %d\n", READER_MAJOR);
		return(err);
	}
#if 1
	ret = request_irq(IRQ_EINT11, RC531_ISR_FUNC, IRQF_TRIGGER_FALLING, READER_NAME, NULL); 
#else
	ret = request_irq(IRQ_EINT11, RC531_ISR_FUNC, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, 
	                  READER_NAME, NULL); 
#endif
	if (ret) 
	{
		printk(READER_NAME": couldn't allocate IRQ (errno %d)\n", ret);
		return -EBUSY;
	}

	irqCount++;

	/* log device registration */
	printk("MF RC531 driver is registered with Major Number =%d \n", READER_MAJOR);

	/* everything OK */
	return 0;
}

void __exit rc531_reader_exit(void)
{
	/* log device unload */
	//printk(READER_NAME " unloading module\n");
	free_irq(IRQ_EINT11, NULL);
	//gpio_free(S5PV210_GPG1(2)); // rf reset pin free	
	//gpio_free(S5PV210_GPH1(3));

	/* unregister driver */
	unregister_chrdev(READER_MAJOR, READER_NAME);
}

module_init(rc531_reader_init);
module_exit(rc531_reader_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HSKIM <hyungsik.kim@gmail.com>");
MODULE_DESCRIPTION("MFRC531 Contactless Card Reader driver for S5PV210");
