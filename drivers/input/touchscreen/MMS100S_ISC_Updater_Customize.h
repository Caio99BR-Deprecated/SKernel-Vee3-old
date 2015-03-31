/*
 * MMS100S ISC Updater�� customize�ϱ� ���� ����Դϴ�.
 * ������ ���� �����ϼž� �ϴ� ����Դϴ�.
 */
#ifndef __MMS100S_ISC_Updater_CUSTOMIZE_H__
#define __MMS100S_ISC_Updater_CUSTOMIZE_H__

#include "MMS100S_ISC_Updater.h"

/*
 * TODO: �ʿ��� header ������ include�� �ּ���.
 * �ʿ��� �������̽��� �Ʒ��� �����ϴ�.
 * memset, malloc, free, strcmp, strstr, fopen, fclose, delay �Լ�, �� �� ����� �޼����� ���� �Լ� ��.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/wakelock.h>
#include <linux/i2c-gpio.h>
#include CONFIG_LGE_BOARD_HEADER_FILE

#include <asm/uaccess.h>
#include <asm/gpio.h>
#include <asm/io.h>

extern const unsigned char mfs_i2c_slave_addr;

/* TODO: File name�� �� ��� �����ؼ� �����ϴ� ��� �� ���� Ű�� �ּ���. */
extern char* mfs_bin_filename;
extern char* mfs_bin_path;
extern mfs_bool_t MFS_I2C_set_slave_addr(unsigned char _slave_addr);
extern mfs_bool_t MFS_I2C_read_with_addr(unsigned char* _read_buf, unsigned char _addr, int _length);
extern mfs_bool_t MFS_I2C_write(const unsigned char* _write_buf, int _length);
extern mfs_bool_t MFS_I2C_read(unsigned char* _read_buf, int _length);
extern void MFS_debug_msg(const char* fmt, int a, int b, int c);
extern void MFS_ms_delay(int msec);
extern void MFS_reboot(void);

#endif
