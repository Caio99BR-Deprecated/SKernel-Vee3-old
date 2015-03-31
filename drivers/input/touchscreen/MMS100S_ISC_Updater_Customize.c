/*
 * MMS100S ISC Updater�� customize�ϱ� ���� �ҽ��Դϴ�.
 * ������ ���� �����ϼž� �ϴ� �ҽ��Դϴ�.
 */
#include "MMS100S_ISC_Updater_Customize.h"

#define ON	1
#define OFF	0

/*
 *	MMS100S_ISC_Updater_V03.c�� MMS100S_FIRMWARE.c�� �߿��� ���ϸ����� ������ �ּ���.
 * */

/*
 *  TODO: ��ſ� ����ϴ� slave address�� ����� �ּ���.
 * !! ���Ǹ� ��Ź�帳�ϴ� !!
 * Config ���� �� ���Ǵ� default slave address�� 0x48 �Դϴ�.
 * �ش� slave address�� ����ϴ� I2C slave device�� ������ Ȯ���� �ֽð�,
 * ���� �ִٸ� MMM100S�� �ٿ�ε� �� ���� �ش� slave device�� disable ���� �ֽʽÿ�.
 */
const unsigned char mfs_i2c_slave_addr = 0x48;

/*
 * TODO: .bin file�� ��θ� ������ �ּ���.
 * ������ ��� ǥ�ù���(e.g. slash)�� �Բ� �Է��� �ּ���.
 */
char* mfs_bin_path;// = "./";


/*
 * TODO: .bin�� ������ ��section�� filename�� ������ �ּ���.
 */

char* mfs_bin_filename;

/*���� setting�� slave address�� �˰� ���� �� �����ϼ���.*/
unsigned char mfs_slave_addr;

#if defined (CONFIG_MACH_LGE)
#define SUPPORT_TOUCH_KEY 1
#else
#define SUPPORT_TOUCH_KEY 0
#endif

#if SUPPORT_TOUCH_KEY
#define LG_FW_HARDKEY_BLOCK
#endif

struct mms100s_ts_device {
	struct i2c_client *client;
	struct input_dev *input_dev;
	/* struct delayed_work work; */
	struct work_struct  work;
#ifdef LG_FW_HARDKEY_BLOCK
	struct hrtimer touch_timer;
	bool hardkey_block;
#endif
	int num_irq;
	int intr_gpio;
	int scl_gpio;
	int sda_gpio;
	bool pendown;
	int (*power)(unsigned char onoff);
	struct workqueue_struct *ts_wq;

	/*20110607 seven.kim@lge.com for touch frimware download [START] */
	struct wake_lock wakelock;
	int irq_sync;
	int fw_version;
	int hw_version;
	int status;
	int tsp_type;
	/*20110607 seven.kim@lge.com for touch frimware download [END] */
};
extern struct mms100s_ts_device mcs8000_ts_dev;

mfs_bool_t MFS_I2C_set_slave_addr(unsigned char _slave_addr)
{
	mfs_slave_addr = _slave_addr << 1; /*�������� ���ʽÿ�.*/

	/* TODO: I2C slave address�� ������ �ּ���. */
	return MFS_TRUE;
}

mfs_bool_t MFS_I2C_read_with_addr(unsigned char* _read_buf,
		unsigned char _addr, int _length)
{
	/* TODO: I2C�� 1 byte address�� �� �� _length ������ŭ �о� _read_buf�� ä�� �ּ���. */

	unsigned char ucTXBuf[1] = {0};
	int iRet = 0;
	struct mms100s_ts_device *dev = NULL;

	dev = &mcs8000_ts_dev;
	ucTXBuf[0] = _addr;

	iRet = i2c_master_send(dev->client, ucTXBuf, 1);
	if(iRet < 0)
	{
		printk(KERN_ERR "MFS_I2C_read_with_addr: i2c failed\n");
		return MFS_FALSE;	
	}

	iRet = i2c_master_recv(dev->client, _read_buf, _length);
	if(iRet < 0)
	{
		printk(KERN_ERR "MFS_I2C_read_with_addr: i2c failed\n");
		return MFS_FALSE;	
	}
	return MFS_TRUE;
}

mfs_bool_t MFS_I2C_write(const unsigned char* _write_buf, int _length)
{
	/*
	 * TODO: I2C�� _write_buf�� ������ _length ������ŭ �� �ּ���.
	 * address�� ����ؾ� �ϴ� �������̽��� ���, _write_buf[0]�� address�� �ǰ�
	 * _write_buf+1���� _length-1���� �� �ֽø� �˴ϴ�.
	 */
	int iRet = 0;
	struct mms100s_ts_device *dev = NULL;

	dev = &mcs8000_ts_dev;

	iRet = i2c_master_send(dev->client, _write_buf, _length);
	if(iRet < 0)
	{
		printk(KERN_ERR "MFS_I2C_write: i2c failed\n");
		return MFS_FALSE;	
	}

	return MFS_TRUE;
}

mfs_bool_t MFS_I2C_read(unsigned char* _read_buf, int _length)
{
	/* TODO: I2C�� _length ������ŭ �о� _read_buf�� ä�� �ּ���. */
	int iRet = 0;
	struct mms100s_ts_device *dev = NULL;

	dev = &mcs8000_ts_dev;

	iRet = i2c_master_recv(dev->client, _read_buf, _length);
	if(iRet < 0)
	{
		printk(KERN_ERR "MFS_I2C_read: i2c failed\n");
		return MFS_FALSE;	
	}

	return MFS_TRUE;
}

void MFS_debug_msg(const char* fmt, int a, int b, int c)
{
#if 0
	log_printf(0, fmt, a, b, c);
#endif
}
void MFS_ms_delay(int msec)
{
	msleep(msec);
}

void MFS_reboot(void)
{

	struct mms100s_ts_device *dev = NULL;
	dev = &mcs8000_ts_dev;

	printk("<MELFAS> TOUCH IC REBOOT!!!\n");
	MFS_debug_msg("<MELFAS> TOUCH IC REBOOT!!!\n", 0, 0, 0);

	dev->power(OFF);
	msleep(100);

	dev->power(ON);
	msleep(100);
}
