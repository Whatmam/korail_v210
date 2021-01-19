#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <plat/gpio-cfg.h>

#define	DEV_NAME	"LM_KEYS"
#define DEV_MAJOR	243

#define KEY1_GPIO	S5PV210_GPH2(0)
#define KEY2_GPIO	S5PV210_GPH2(1)

#if defined(CONFIG_KORAIL_MAIN_BOARD)
#define KEY3_GPIO	S5PV210_GPH2(2)
#define KEY4_GPIO	S5PV210_GPH2(3)
#endif

struct input_dev *input_dev;

int key_code_data[] = {
	KEY_ESC,
	KEY_DOWN,
#if defined(CONFIG_KORAIL_MAIN_BOARD)
	KEY_UP,
	KEY_ENTER,
#endif
};

irqreturn_t key1_irq(int irq, void *dev_id)
{
	u32 keydat = gpio_get_value( KEY1_GPIO );

	if(!keydat)
	{
		input_report_key(input_dev, key_code_data[0], 1);
		//printk("ESC1 pushed\r\n");
	}
	else
	{
		input_report_key(input_dev, key_code_data[0], 0);
		//printk("ESC1 released\r\n");
	}
	input_sync(input_dev);

	return IRQ_HANDLED;
}

irqreturn_t key2_irq(int irq, void *dev_id)
{
	u32 keydat = gpio_get_value( KEY2_GPIO );

    if(!keydat)
    {
        input_report_key(input_dev, key_code_data[1], 1);
		//printk("DOWN1 pushed\r\n");
    }
    else
    {
        input_report_key(input_dev, key_code_data[1], 0);
		//printk("DOWN1 released\r\n");
    }
	input_sync(input_dev);
	return IRQ_HANDLED;
}

#if defined(CONFIG_KORAIL_MAIN_BOARD)
irqreturn_t key3_irq(int irq, void *dev_id)
{
	u32 keydat = gpio_get_value( KEY3_GPIO );

    if(!keydat)
    {
        input_report_key(input_dev, key_code_data[2], 1);
		//printk("UP1 pushed\r\n");
    }
    else
    {
        input_report_key(input_dev, key_code_data[2], 0);
		//printk("UP1 released\r\n");
    }
	input_sync(input_dev);
	return IRQ_HANDLED;
}



irqreturn_t key4_irq(int irq, void *dev_id)
{
	u32 keydat = gpio_get_value( KEY4_GPIO );

    if(!keydat)
    {
        input_report_key(input_dev, key_code_data[3], 1);
		//printk("ENTER1 pushed\r\n");
    }
    else
    {
        input_report_key(input_dev, key_code_data[3], 0);
		//printk("ENTER1 released\r\n");
    }
	input_sync(input_dev);
	return IRQ_HANDLED;
}
#endif

static int __init korail_key_probe(struct platform_device *pdev)
{
	//printk("KEY DRIVER PROBE\r\n");

	//printk("KEY PROBE END\r\n");
	return 0;
}

static struct platform_driver korail_key_driver = {
	.probe		= korail_key_probe,
	.driver		= {
		.name	= DEV_NAME,
		.owner	= THIS_MODULE,
	}
};

static int __init korail_key_init(void)
{
	int key=0, code=0, res=0;

	//request GPIO
	//gpio_request( KEY1_GPIO, "GPH20");
	//gpio_request( KEY2_GPIO, "GPH21");
#if defined(CONFIG_KORAIL_MAIN_BOARD)
	//gpio_request( KEY3_GPIO, "GPH23");
	//gpio_request( KEY4_GPIO, "GPH24");
#endif

	//set pin to GPIO input
	//s3c_gpio_cfgpin( KEY1_GPIO, S3C_GPIO_INPUT );
	//s3c_gpio_cfgpin( KEY2_GPIO, S3C_GPIO_INPUT );
#if defined(CONFIG_KORAIL_MAIN_BOARD)
	//s3c_gpio_cfgpin( KEY3_GPIO, S3C_GPIO_INPUT );
	//s3c_gpio_cfgpin( KEY4_GPIO, S3C_GPIO_INPUT );
#endif

	//input device
	input_dev = input_allocate_device();
	if(!input_dev)
	{
		//printk("KEY input_allocate_device fail\r\n");
	}

	input_dev->name = "lm_key";
	input_dev->evbit[0] = BIT(EV_KEY);
	input_dev->phys = "lm_key/input0";
	input_dev->id.bustype = BUS_HOST;
	input_dev->id.vendor = 0x0001;
	input_dev->id.product = 0x0001;
	input_dev->id.version = 0x0001;

#if defined(CONFIG_KORAIL_MAIN_BOARD)
	for (key = 0; key < 4; key++) {
#else
	for (key = 0; key < 2; key++) {
#endif
		code = key_code_data[key];
		set_bit(code & KEY_MAX, input_dev->keybit);
	}

	res = input_register_device(input_dev);
	
	if(res)
	{
		//printk("KEY input device register error\r\n");
	}

	//irq setting.
	res = request_irq(IRQ_EINT(16), key1_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, DEV_NAME, &input_dev);
	
	if(res)
	{
		//printk("KEY ESC1 init fail\r\n");
	}

	res = request_irq(IRQ_EINT(17), key2_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, DEV_NAME, &input_dev);
	
	if(res)
	{
		//printk("KEY DOWN1 init fail\r\n");
	}
	else
	{
		//printk("KEY DOWN1 init success\r\n");
	}
	
#if defined(CONFIG_KORAIL_MAIN_BOARD)
	res = request_irq(IRQ_EINT(18), key3_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, DEV_NAME, &input_dev);
	
	if(res)
	{
		//printk("KEY UP1 init fail\r\n");
	}

	res = request_irq(IRQ_EINT(19), key4_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, DEV_NAME, &input_dev);
	
	if(res)
	{
		//printk("KEY ENTER1 init fail\r\n");
	}
#endif

	if(!platform_driver_register(&korail_key_driver))
	{
		//printk("KEY Driver registered\r\n");
	}

	return 0;
}

static void __exit korail_key_exit(void)
{
	int i=0;

	//printk("KEY EXIT\r\n");
	input_free_device(input_dev);
	platform_driver_unregister(&korail_key_driver);

	free_irq(IRQ_EINT(16), NULL);
	free_irq(IRQ_EINT(17), NULL);

#if defined(CONFIG_KORAIL_MAIN_BOARD)
	free_irq(IRQ_EINT(18), NULL);
	free_irq(IRQ_EINT(19), NULL);
#endif
}

module_init(korail_key_init); //mknod /dev/LM_KEYS c 243 0 
module_exit(korail_key_exit);
