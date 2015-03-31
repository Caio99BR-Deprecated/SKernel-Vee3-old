/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/gpio_event.h>
#include <linux/leds.h>
#include <linux/i2c/atmel_mxt_ts.h>
#include <linux/i2c.h>
#include <linux/input/rmi_platformdata.h>
#include <linux/input/rmi_i2c.h>
#include <linux/delay.h>
#include <linux/atmel_maxtouch.h>
#include <linux/input/ft5x06_ts.h>
#include <asm/mach-types.h>
#include <mach/rpc_server_handset.h>
#include <mach/pmic.h>
#include <asm/gpio.h>

/*[LGE_BSP_S][yunmo.yang@lge.com] LP5521 RGB Driver*/
#include <linux/leds-lp5521.h>
/*[LGE_BSP_E][yunmo.yang@lge.com] LP5521 RGB Driver*/
/*LGE_CHANGE_S : byungyong.hwang@lge.com touch - Synaptics s3203 panel  for V7*/
#ifdef CONFIG_LGE_TOUCHSCREEN_SYNAPTICS_I2C_RMI4
#include <linux/input/lge_touch_core.h>
#endif
/*LGE_CHANGE_E : byungyong.hwang@lge.com touch - Synaptics s3203 panel  for V7*/

/*LGE_CHANGE_S : seven.kim@lge.com kernel3.4 for v3/v5*/
#if defined (CONFIG_MACH_LGE)
#include "../../devices.h"
#include "../../board-msm7627a.h"
#include "../../devices-msm7x2xa.h"
#include CONFIG_LGE_BOARD_HEADER_FILE
#else /*qct original*/
#include "devices.h"
#include "board-msm7627a.h"
#include "devices-msm7x2xa.h"
#endif /*CONFIG_MACH_LGE*/
/*LGE_CHANGE_E : seven.kim@lge.com kernel3.4 for v3/v5*/

#ifdef CONFIG_LGE_NFC
#include <linux/nfc/pn544_lge.h>
#endif

#define ATMEL_TS_I2C_NAME "maXTouch"
#define ATMEL_X_OFFSET 13
#define ATMEL_Y_OFFSET 0

/*LGE_CHANGE_S : byungyong.hwang@lge.com  key-remapping for V7*/
#define KEY_SIM_SWITCH 228
#undef  KEY_HOME
#define KEY_HOME 172
#undef  KEY_MENU
#define KEY_MENU 139
/*LGE_CHANGE_E : byungyong.hwang@lge.com  key-remapping for V7*/


#if defined(CONFIG_TOUCHSCREEN_SYNAPTICS_RMI4_I2C) || \
defined(CONFIG_TOUCHSCREEN_SYNAPTICS_RMI4_I2C_MODULE)

#ifndef CLEARPAD3000_ATTEN_GPIO
#define CLEARPAD3000_ATTEN_GPIO (48)
#endif

#ifndef CLEARPAD3000_RESET_GPIO
#define CLEARPAD3000_RESET_GPIO (26)
#endif

#define KP_INDEX(row, col) ((row)*ARRAY_SIZE(kp_col_gpios) + (col))

static unsigned int kp_row_gpios[] = {31, 32, 33, 34, 35};
static unsigned int kp_col_gpios[] = {36, 37, 38, 39, 40};

static const unsigned short keymap[ARRAY_SIZE(kp_col_gpios) *
					  ARRAY_SIZE(kp_row_gpios)] = {
	[KP_INDEX(0, 0)] = KEY_7,
	[KP_INDEX(0, 1)] = KEY_DOWN,
	[KP_INDEX(0, 2)] = KEY_UP,
	[KP_INDEX(0, 3)] = KEY_RIGHT,
	[KP_INDEX(0, 4)] = KEY_ENTER,

	[KP_INDEX(1, 0)] = KEY_LEFT,
	[KP_INDEX(1, 1)] = KEY_SEND,
	[KP_INDEX(1, 2)] = KEY_1,
	[KP_INDEX(1, 3)] = KEY_4,
	[KP_INDEX(1, 4)] = KEY_CLEAR,

	[KP_INDEX(2, 0)] = KEY_6,
	[KP_INDEX(2, 1)] = KEY_5,
	[KP_INDEX(2, 2)] = KEY_8,
	[KP_INDEX(2, 3)] = KEY_3,
	[KP_INDEX(2, 4)] = KEY_NUMERIC_STAR,

	[KP_INDEX(3, 0)] = KEY_9,
	[KP_INDEX(3, 1)] = KEY_NUMERIC_POUND,
	[KP_INDEX(3, 2)] = KEY_0,
	[KP_INDEX(3, 3)] = KEY_2,
	[KP_INDEX(3, 4)] = KEY_SLEEP,

	[KP_INDEX(4, 0)] = KEY_BACK,
	[KP_INDEX(4, 1)] = KEY_HOME,
	[KP_INDEX(4, 2)] = KEY_MENU,
	[KP_INDEX(4, 3)] = KEY_VOLUMEUP,
	[KP_INDEX(4, 4)] = KEY_VOLUMEDOWN,
};

/* SURF keypad platform device information */
static struct gpio_event_matrix_info kp_matrix_info = {
	.info.func	= gpio_event_matrix_func,
	.keymap		= keymap,
	.output_gpios	= kp_row_gpios,
	.input_gpios	= kp_col_gpios,
	.noutputs	= ARRAY_SIZE(kp_row_gpios),
	.ninputs	= ARRAY_SIZE(kp_col_gpios),
	.settle_time.tv64 = 40 * NSEC_PER_USEC,
	.poll_time.tv64 = 20 * NSEC_PER_MSEC,
	.flags		= GPIOKPF_LEVEL_TRIGGERED_IRQ | GPIOKPF_DRIVE_INACTIVE |
			  GPIOKPF_PRINT_UNMAPPED_KEYS,
};

static struct gpio_event_info *kp_info[] = {
	&kp_matrix_info.info
};

static struct gpio_event_platform_data kp_pdata = {
	.name		= "7x27a_kp",
	.info		= kp_info,
	.info_count	= ARRAY_SIZE(kp_info)
};

static struct platform_device kp_pdev = {
	.name	= GPIO_EVENT_DEV_NAME,
	.id	= -1,
	.dev	= {
		.platform_data	= &kp_pdata,
	},
};

/* 8625 keypad device information */
static unsigned int kp_row_gpios_8625[] = {31};
static unsigned int kp_col_gpios_8625[] = {36, 37};

static const unsigned short keymap_8625[] = {
	KEY_VOLUMEUP,
	KEY_VOLUMEDOWN,
};

static const unsigned short keymap_8625_evt[] = {
	KEY_VOLUMEDOWN,
	KEY_VOLUMEUP,
};

static struct gpio_event_matrix_info kp_matrix_info_8625 = {
	.info.func      = gpio_event_matrix_func,
	.keymap         = keymap_8625,
	.output_gpios   = kp_row_gpios_8625,
	.input_gpios    = kp_col_gpios_8625,
	.noutputs       = ARRAY_SIZE(kp_row_gpios_8625),
	.ninputs        = ARRAY_SIZE(kp_col_gpios_8625),
	.settle_time.tv64 = 40 * NSEC_PER_USEC,
	.poll_time.tv64 = 20 * NSEC_PER_MSEC,
	.flags          = GPIOKPF_LEVEL_TRIGGERED_IRQ | GPIOKPF_DRIVE_INACTIVE |
			  GPIOKPF_PRINT_UNMAPPED_KEYS,
};

static struct gpio_event_info *kp_info_8625[] = {
	&kp_matrix_info_8625.info,
};

static struct gpio_event_platform_data kp_pdata_8625 = {
	.name           = "7x27a_kp",
	.info           = kp_info_8625,
	.info_count     = ARRAY_SIZE(kp_info_8625)
};

static struct platform_device kp_pdev_8625 = {
	.name   = GPIO_EVENT_DEV_NAME,
	.id     = -1,
	.dev    = {
		.platform_data  = &kp_pdata_8625,
	},
};

#define LED_GPIO_PDM 96
#define LED_RED_GPIO_8625 49
#define LED_GREEN_GPIO_8625 34

static struct gpio_led gpio_leds_config_8625[] = {
	{
		.name = "green",
		.gpio = LED_GREEN_GPIO_8625,
	},
	{
		.name = "red",
		.gpio = LED_RED_GPIO_8625,
	},
};

static struct gpio_led_platform_data gpio_leds_pdata_8625 = {
	.num_leds = ARRAY_SIZE(gpio_leds_config_8625),
	.leds = gpio_leds_config_8625,
};

static struct platform_device gpio_leds_8625 = {
	.name          = "leds-gpio",
	.id            = -1,
	.dev           = {
		.platform_data = &gpio_leds_pdata_8625,
	},
};

#define MXT_TS_IRQ_GPIO         48
#define MXT_TS_RESET_GPIO       26
#define MAX_VKEY_LEN		100

static ssize_t mxt_virtual_keys_register(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	char *virtual_keys = __stringify(EV_KEY) ":" __stringify(KEY_MENU) \
		":60:840:120:80" ":" __stringify(EV_KEY) \
		":" __stringify(KEY_HOME)   ":180:840:120:80" \
		":" __stringify(EV_KEY) ":" \
		__stringify(KEY_BACK) ":300:840:120:80" \
		":" __stringify(EV_KEY) ":" \
		__stringify(KEY_SEARCH)   ":420:840:120:80" "\n";

	return snprintf(buf, strnlen(virtual_keys, MAX_VKEY_LEN) + 1 , "%s",
			virtual_keys);
}

static struct kobj_attribute mxt_virtual_keys_attr = {
	.attr = {
		.name = "virtualkeys.atmel_mxt_ts",
		.mode = S_IRUGO,
	},
	.show = &mxt_virtual_keys_register,
};

static struct attribute *mxt_virtual_key_properties_attrs[] = {
	&mxt_virtual_keys_attr.attr,
	NULL,
};

static struct attribute_group mxt_virtual_key_properties_attr_group = {
	.attrs = mxt_virtual_key_properties_attrs,
};

struct kobject *mxt_virtual_key_properties_kobj;

static int mxt_vkey_setup(void)
{
	int retval = 0;

	mxt_virtual_key_properties_kobj =
		kobject_create_and_add("board_properties", NULL);
	if (mxt_virtual_key_properties_kobj)
		retval = sysfs_create_group(mxt_virtual_key_properties_kobj,
				&mxt_virtual_key_properties_attr_group);
	if (!mxt_virtual_key_properties_kobj || retval)
		pr_err("failed to create mxt board_properties\n");

	return retval;
}

static const u8 mxt_config_data[] = {
	/* T6 Object */
	0, 0, 0, 0, 0, 0,
	/* T38 Object */
	16, 1, 0, 0, 0, 0, 0, 0,
	/* T7 Object */
	32, 16, 50,
	/* T8 Object */
	30, 0, 20, 20, 0, 0, 20, 0, 50, 0,
	/* T9 Object */
	3, 0, 0, 18, 11, 0, 32, 75, 3, 3,
	0, 1, 1, 0, 10, 10, 10, 10, 31, 3,
	223, 1, 11, 11, 15, 15, 151, 43, 145, 80,
	100, 15, 0, 0, 0,
	/* T15 Object */
	131, 0, 11, 11, 1, 1, 0, 45, 3, 0,
	0,
	/* T18 Object */
	0, 0,
	/* T19 Object */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
	/* T23 Object */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	/* T25 Object */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0,
	/* T40 Object */
	0, 0, 0, 0, 0,
	/* T42 Object */
	0, 0, 0, 0, 0, 0, 0, 0,
	/* T46 Object */
	0, 2, 32, 48, 0, 0, 0, 0, 0,
	/* T47 Object */
	1, 20, 60, 5, 2, 50, 40, 0, 0, 40,
	/* T48 Object */
	1, 12, 80, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 6, 6, 0, 0, 100, 4, 64,
	10, 0, 20, 5, 0, 38, 0, 20, 0, 0,
	0, 0, 0, 0, 16, 65, 3, 1, 1, 0,
	10, 10, 10, 0, 0, 15, 15, 154, 58, 145,
	80, 100, 15, 3,
};

static const u8 mxt_config_data_evt[] = {
	/* T6 Object */
	0, 0, 0, 0, 0, 0,
	/* T38 Object */
	20, 0, 0, 0, 0, 0, 0, 0,
	/* T7 Object */
	24, 12, 10,
	/* T8 Object */
	30, 0, 20, 20, 0, 0, 9, 45, 10, 192,
	/* T9 Object */
	3, 0, 0, 18, 11, 0, 16, 60, 3, 1,
	0, 1, 1, 0, 10, 10, 10, 10, 107, 3,
	223, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	20, 15, 0, 0, 2,
	/* T15 Object */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0,
	/* T18 Object */
	0, 0,
	/* T19 Object */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
	/* T23 Object */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	/* T25 Object */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0,
	/* T40 Object */
	17, 0, 0, 30, 30,
	/* T42 Object */
	3, 20, 45, 40, 128, 0, 0, 0,
	/* T46 Object */
	0, 2, 16, 16, 0, 0, 0, 0, 0,
	/* T47 Object */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* T48 Object */
	1, 128, 96, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 6, 6, 0, 0, 63, 4, 64,
	10, 0, 32, 5, 0, 38, 0, 8, 0, 0,
	0, 0, 0, 0, 16, 65, 3, 1, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0,
};

static struct mxt_config_info mxt_config_array[] = {
	{
		.config		= mxt_config_data,
		.config_length	= ARRAY_SIZE(mxt_config_data),
		.family_id	= 0x81,
		.variant_id	= 0x01,
		.version	= 0x10,
		.build		= 0xAA,
	},
};

static int mxt_key_codes[MXT_KEYARRAY_MAX_KEYS] = {
	[0] = KEY_HOME,
	[1] = KEY_MENU,
	[9] = KEY_BACK,
	[10] = KEY_SEARCH,
};

static struct mxt_platform_data mxt_platform_data = {
	.config_array		= mxt_config_array,
	.config_array_size	= ARRAY_SIZE(mxt_config_array),
	.panel_minx		= 0,
	.panel_maxx		= 479,
	.panel_miny		= 0,
	.panel_maxy		= 799,
	.disp_minx		= 0,
	.disp_maxx		= 479,
	.disp_miny		= 0,
	.disp_maxy		= 799,
	.irqflags		= IRQF_TRIGGER_FALLING,
	.i2c_pull_up		= true,
	.reset_gpio		= MXT_TS_RESET_GPIO,
	.irq_gpio		= MXT_TS_IRQ_GPIO,
	.key_codes		= mxt_key_codes,
};

static struct i2c_board_info mxt_device_info[] __initdata = {
	{
		I2C_BOARD_INFO("atmel_mxt_ts", 0x4a),
		.platform_data = &mxt_platform_data,
		.irq = MSM_GPIO_TO_INT(MXT_TS_IRQ_GPIO),
	},
};

static int synaptics_touchpad_setup(void);

static struct msm_gpio clearpad3000_cfg_data[] = {
	{GPIO_CFG(CLEARPAD3000_ATTEN_GPIO, 0, GPIO_CFG_INPUT,
			GPIO_CFG_NO_PULL, GPIO_CFG_6MA), "rmi4_attn"},
	{GPIO_CFG(CLEARPAD3000_RESET_GPIO, 0, GPIO_CFG_OUTPUT,
			GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA), "rmi4_reset"},
};

static struct rmi_XY_pair rmi_offset = {.x = 0, .y = 0};
static struct rmi_range rmi_clipx = {.min = 48, .max = 980};
static struct rmi_range rmi_clipy = {.min = 7, .max = 1647};
static struct rmi_f11_functiondata synaptics_f11_data = {
	.swap_axes = false,
	.flipX = false,
	.flipY = false,
	.offset = &rmi_offset,
	.button_height = 113,
	.clipX = &rmi_clipx,
	.clipY = &rmi_clipy,
};

#define MAX_LEN		100

static ssize_t clearpad3000_virtual_keys_register(struct kobject *kobj,
		     struct kobj_attribute *attr, char *buf)
{
	char *virtual_keys = __stringify(EV_KEY) ":" __stringify(KEY_MENU) \
			     ":60:830:120:60" ":" __stringify(EV_KEY) \
			     ":" __stringify(KEY_HOME)   ":180:830:120:60" \
				":" __stringify(EV_KEY) ":" \
				__stringify(KEY_SEARCH) ":300:830:120:60" \
				":" __stringify(EV_KEY) ":" \
			__stringify(KEY_BACK)   ":420:830:120:60" "\n";

	return snprintf(buf, strnlen(virtual_keys, MAX_LEN) + 1 , "%s",
			virtual_keys);
}

static struct kobj_attribute clearpad3000_virtual_keys_attr = {
	.attr = {
		.name = "virtualkeys.sensor00fn11",
		.mode = S_IRUGO,
	},
	.show = &clearpad3000_virtual_keys_register,
};

static struct attribute *virtual_key_properties_attrs[] = {
	&clearpad3000_virtual_keys_attr.attr,
	NULL
};

static struct attribute_group virtual_key_properties_attr_group = {
	.attrs = virtual_key_properties_attrs,
};

struct kobject *virtual_key_properties_kobj;

static struct rmi_functiondata synaptics_functiondata[] = {
	{
		.function_index = RMI_F11_INDEX,
		.data = &synaptics_f11_data,
	},
};

static struct rmi_functiondata_list synaptics_perfunctiondata = {
	.count = ARRAY_SIZE(synaptics_functiondata),
	.functiondata = synaptics_functiondata,
};

static struct rmi_sensordata synaptics_sensordata = {
	.perfunctiondata = &synaptics_perfunctiondata,
	.rmi_sensor_setup	= synaptics_touchpad_setup,
};

static struct rmi_i2c_platformdata synaptics_platformdata = {
	.i2c_address = 0x2c,
	.irq_type = IORESOURCE_IRQ_LOWLEVEL,
	.sensordata = &synaptics_sensordata,
};

static struct i2c_board_info synaptic_i2c_clearpad3k[] = {
	{
	I2C_BOARD_INFO("rmi4_ts", 0x2c),
	.platform_data = &synaptics_platformdata,
	},
};

static int synaptics_touchpad_setup(void)
{
	int retval = 0;

	virtual_key_properties_kobj =
		kobject_create_and_add("board_properties", NULL);
	if (virtual_key_properties_kobj)
		retval = sysfs_create_group(virtual_key_properties_kobj,
				&virtual_key_properties_attr_group);
	if (!virtual_key_properties_kobj || retval)
		pr_err("failed to create ft5202 board_properties\n");

	retval = msm_gpios_request_enable(clearpad3000_cfg_data,
		    sizeof(clearpad3000_cfg_data)/sizeof(struct msm_gpio));
	if (retval) {
		pr_err("%s:Failed to obtain touchpad GPIO %d. Code: %d.",
				__func__, CLEARPAD3000_ATTEN_GPIO, retval);
		retval = 0; /* ignore the err */
	}
	synaptics_platformdata.irq = gpio_to_irq(CLEARPAD3000_ATTEN_GPIO);

	gpio_set_value(CLEARPAD3000_RESET_GPIO, 0);
	usleep(10000);
	gpio_set_value(CLEARPAD3000_RESET_GPIO, 1);
	usleep(50000);

	return retval;
}
#endif
/* handset device */
static struct msm_handset_platform_data hs_platform_data = {
	.hs_name = "7k_handset",
	.pwr_key_delay_ms = 500, /* 0 will disable end key */
};

static struct platform_device hs_pdev = {
	.name   = "msm-handset",
	.id     = -1,
	.dev    = {
		.platform_data = &hs_platform_data,
	},
};
 
/* LGE_CHANGE  [yoonsoo.kim@lge.com]  20111006  : VEE7 Key Configuration */
static unsigned int keypad_row_gpios[] = {36, 37,38};
static unsigned int keypad_col_gpios[] = {32,33};
#define KEY_QUICK_MEMO 225
#define KEYMAP_INDEX(col, row) ((col)*ARRAY_SIZE(keypad_row_gpios) + (row))

static const unsigned short keypad_keymap_vee7[] = {
	[KEYMAP_INDEX(0, 0)] = KEY_VOLUMEUP,
	[KEYMAP_INDEX(0, 1)] = KEY_VOLUMEDOWN,
	[KEYMAP_INDEX(0, 2)] = KEY_QUICK_MEMO,
	[KEYMAP_INDEX(1, 2)] = KEY_HOME,
};
/* LGE_CHANGE_E  [yoonsoo.kim@lge.com]  20111006  : U0 RevB Key Configuration */

int vee7_matrix_info_wrapper(struct gpio_event_input_devs *input_dev,
			   struct gpio_event_info *info, void **data, int func)
{
	int ret;
	if (func == GPIO_EVENT_FUNC_RESUME) {
		gpio_tlmm_config(GPIO_CFG(keypad_row_gpios[0], 0, GPIO_CFG_INPUT,
				 GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		gpio_tlmm_config(GPIO_CFG(keypad_row_gpios[1], 0, GPIO_CFG_INPUT,
				 GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	}
	/* LGE_CHANGE [yoonsoo.kim@lge.com] 20120303  : Revert Patch Code */
	
	ret = gpio_event_matrix_func(input_dev, info, data, func);
	return ret ;
}

static int vee7_gpio_matrix_power(const struct gpio_event_platform_data *pdata, bool on)
{
	/* this is dummy function
	 * to make gpio_event driver register suspend function
	 * 2010-01-29, cleaneye.kim@lge.com
	 * copy from ALOHA code
	 * 2010-04-22 younchan.kim@lge.com
	 */
	return 0;
}

static struct gpio_event_matrix_info vee7_keypad_matrix_info = {
	.info.func	= vee7_matrix_info_wrapper,
	.keymap		= keypad_keymap_vee7,
	.output_gpios	= keypad_col_gpios,
	.input_gpios	= keypad_row_gpios,
	.noutputs	= ARRAY_SIZE(keypad_col_gpios),
	.ninputs	= ARRAY_SIZE(keypad_row_gpios),	
/*LGE_CHANGE_S : seven.kim@lge.com kernel3.0 proting
 * gpio_event_matrix_info structure member was changed, ktime_t -> struct timespec */	
	.settle_time.tv64 = 40 * NSEC_PER_USEC,
	.poll_time.tv64 = 20 * NSEC_PER_MSEC,
/*LGE_CHANGE_E : seven.kim@lge.com kernel3.0 proting*/
	.flags		= GPIOKPF_LEVEL_TRIGGERED_IRQ | GPIOKPF_PRINT_UNMAPPED_KEYS | GPIOKPF_DRIVE_INACTIVE
};

static struct gpio_event_info *vee7_keypad_info[] = {
	&vee7_keypad_matrix_info.info
};

static struct gpio_event_platform_data vee7_keypad_data = {
	.name		= "vee7_keypad",
	.info		= vee7_keypad_info,
	.info_count	= ARRAY_SIZE(vee7_keypad_info),
	.power          = vee7_gpio_matrix_power,
};

struct platform_device keypad_device_vee7 = {
	.name	= GPIO_EVENT_DEV_NAME,
	.id	= -1,
	.dev	= {
		.platform_data	= &vee7_keypad_data,
	},
};

/* input platform device */
static struct platform_device *vee7_input_devices[] __initdata = {
	&hs_pdev,
};

static struct platform_device *vee7_gpio_input_devices[] __initdata = {
	&keypad_device_vee7,
};


/* LGE_CHANGE_S [seven.kim@lge.com] 20110922 New Bosch compass+accel Sensor Porting*/ 
#if defined (CONFIG_SENSORS_BMM050) || defined (CONFIG_SENSORS_BMA250)
static struct gpio_i2c_pin accel_i2c_pin[] = {
	[0] = {
		.sda_pin	= SENSOR_GPIO_I2C_SDA,
		.scl_pin	= SENSOR_GPIO_I2C_SCL,
		.reset_pin	= 0,
		.irq_pin	= ACCEL_GPIO_INT,
	},
};

static struct gpio_i2c_pin ecom_i2c_pin[] = {
	[0] = {
		.sda_pin	= SENSOR_GPIO_I2C_SDA,
		.scl_pin	= SENSOR_GPIO_I2C_SCL,
		.reset_pin	= 0,
		.irq_pin	= ECOM_GPIO_INT,
	},
};

static struct i2c_gpio_platform_data sensor_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 2,
};

static struct platform_device sensor_i2c_device = {
	.name = "i2c-gpio",
	.dev.platform_data = &sensor_i2c_pdata,
};

static struct i2c_board_info sensor_i2c_bdinfo[] = {
	[0] = {
    #if defined (CONFIG_SENSORS_BMA2X2)
		I2C_BOARD_INFO("bma250", ACCEL_I2C_ADDRESS),
		.type = "bma2x2",
    #else
		I2C_BOARD_INFO("bma250", ACCEL_I2C_ADDRESS),
		.type = "bma250",
    #endif
	},
	[1] = {
		I2C_BOARD_INFO("bmm050", ECOM_I2C_ADDRESS),
		.type = "bmm050",
	},
};


static void __init vee7_init_i2c_sensor(int bus_num)
{
	sensor_i2c_device.id = bus_num;

	lge_init_gpio_i2c_pin_pullup(&sensor_i2c_pdata, accel_i2c_pin[0], &sensor_i2c_bdinfo[0]);
	lge_init_gpio_i2c_pin_pullup(&sensor_i2c_pdata, ecom_i2c_pin[0], &sensor_i2c_bdinfo[1]);

	i2c_register_board_info(bus_num, sensor_i2c_bdinfo, ARRAY_SIZE(sensor_i2c_bdinfo));

	platform_device_register(&sensor_i2c_device);
}
#endif /* LGE_CHANGE_E [seven.kim@lge.com 20110922 New Bosch compass+accel Sensor Porting*/ 

/* proximity */
static int prox_power_set(unsigned char onoff)
{
	//Need to implement Power Control : PMIC L12 Block use
	return 0;
}

static struct proximity_platform_data proxi_pdata = {
	.irq_num = PROXI_GPIO_DOUT,
	.power = prox_power_set,
	.methods = 0,
	.operation_mode = 0,
	.debounce = 0,
	.cycle = 2,
};

static struct i2c_board_info prox_i2c_bdinfo[] = {
	[0] = {
//[LGSI_SP4_BSP_BEGIN][kirankumar.vm@lge.com] Proximity Changes
#ifdef CONFIG_SENSOR_APDS9190
		I2C_BOARD_INFO("proximity_apds9190", PROXI_I2C_ADDRESS),
#else /*CONFIG_SENSOR_APDS9130 */
		I2C_BOARD_INFO("apds9130", PROXI_I2C_ADDRESS),
#endif
	        .irq = MSM_GPIO_TO_INT(PROXI_GPIO_DOUT),
//[LGSI_SP4_BSP_END][kirankumar.vm@lge.com] Proximity Changes
		.platform_data = &proxi_pdata,
	},
};

static struct gpio_i2c_pin proxi_i2c_pin[] = {
	[0] = {
		.sda_pin	= PROXI_GPIO_I2C_SDA,
		.scl_pin	= PROXI_GPIO_I2C_SCL,
		.reset_pin	= 0,
		.irq_pin	= PROXI_GPIO_DOUT,
	},
};

static struct i2c_gpio_platform_data proxi_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 2,
};

static struct platform_device proxi_i2c_device = {
	.name = "i2c-gpio",
	.dev.platform_data = &proxi_i2c_pdata,
};

static void __init vee7_init_i2c_prox(int bus_num)
{
	proxi_i2c_device.id = bus_num;

	lge_init_gpio_i2c_pin_pullup(
		&proxi_i2c_pdata, proxi_i2c_pin[0], &prox_i2c_bdinfo[0]);

	i2c_register_board_info(bus_num, &prox_i2c_bdinfo[0], 1);
	platform_device_register(&proxi_i2c_device);
}
/*[LGE_BSP_S][yunmo.yang@lge.com] LP5521 RGB Driver*/
#ifdef CONFIG_LEDS_LP5521

static struct lp5521_led_config lp5521_led_config[] = {
	{
		.name = "R",
		.chan_nr	= 0,
		//.led_current	= 170,
		//.max_current	= 170,
		.led_current	= 70,
		.max_current	= 255,
	},
	{
		.name = "G",
		.chan_nr	= 1,
		//.led_current	= 200,
		//.max_current	= 200,
		.led_current	= 40,
		.max_current	= 255,
	},
	{
		.name = "B",
		.chan_nr	= 2,
		//.led_current	= 130,
		//.max_current	= 130,
		.led_current	= 80,
		.max_current	= 255,
	},
};


//[pattern_id : 1, PowerOn_Animation]
//static u8 mode1_red[] = {0x40, 0x00, 0x08, 0x7e, 0x08, 0x7f, 0x08, 0xff, 0x08, 0xfe};
//static u8 mode1_green[] = {0x40, 0x00, 0x08, 0x7e, 0x08, 0x7f, 0x08, 0xff, 0x08, 0xfe};
//static u8 mode1_blue[] = {0x40, 0x00, 0x08, 0x7e, 0x08, 0x7f, 0x08, 0xff, 0x08, 0xfe};
//[Tuning 20121119]
static u8 mode1_red[] = {0xE0, 0x0C, 0x40, 0x00, 0x0C, 0x2F, 0x06, 0x28, 0x05, 0x2D, 0x06, 0x2A, 0x06, 0x25, 0x03, 0xDC, 0x02, 0xFA, 0x48, 0x00, 0x03, 0x54, 0x44, 0x01, 0x23, 0x06, 0x31, 0x84, 0x06, 0xA8, 0x0C, 0xAF};
static u8 mode1_green[] = {0xE0, 0x80, 0x40, 0x00, 0x52, 0x00, 0x0B, 0x15, 0x05, 0x2D, 0x03, 0x48, 0x03, 0x4B, 0x09, 0x1B, 0x02, 0x63, 0x19, 0x89, 0x03, 0xCA, 0x04, 0xC1, 0x05, 0xB2, 0x06, 0xA6, 0x12, 0x8D, 0x52, 0x00};
static u8 mode1_blue[] = {0xE0, 0x80, 0x40, 0x00, 0x12, 0xFE, 0x40, 0xC0, 0x0A, 0x18, 0x06, 0xA6, 0x06, 0xAA, 0x03, 0xCF, 0x04, 0xB6, 0x52, 0x00};

//[pattern_id : 2, Not used, LCDOn]
static u8 mode2_red[]={0x40, 0xff, 0x4d, 0x00, 0x0a, 0xff, 0x0a, 0xfe, 0xc0, 0x00};
static u8 mode2_green[]={0x40, 0xff, 0x4d, 0x00, 0x0a, 0xff, 0x0a, 0xfe, 0xc0, 0x00};
static u8 mode2_blue[]={0x40, 0xff, 0x4d, 0x00, 0x0a, 0xff, 0x0a, 0xfe, 0xc0, 0x00};

//[pattern_id : 3, Charging0_99]
//static u8 mode3_red[] = {0x40, 0x19, 0x27, 0x19, 0x0c, 0x65, 0x0c, 0x65, 0x0c, 0xe5, 0x0c, 0xe5, 0x29, 0x98, 0x5a, 0x00};
//[Tuning 20121119]
//static u8 mode3_red[] = {0x40, 0x0D, 0x44, 0x0C, 0x24, 0x32, 0x24, 0x32, 0x66, 0x00, 0x24, 0xB2, 0x24, 0xB2, 0x44, 0x8C};
static u8 mode3_red[] = {0x40, 0x1a, 0x42, 0x18, 0x12, 0x65, 0x12, 0x65, 0x66, 0x00, 0x12, 0xe5, 0x12, 0xe5, 0x42, 0x98};

//[pattern_id : 4, Charging100]
//static u8 mode4_green[]={0x40, 0xff};
//[Tuning 20121119]
static u8 mode4_green[]={0x40, 0xff};
//[pattern_id : 5, Not used, Charging16_99]
static u8 mode5_red[]={0x40, 0x19, 0x27, 0x19, 0xe0, 0x04, 0x0c, 0x65, 0xe0, 0x04, 0x0c, 0x65, 0xe0, 0x04, 0x0c, 0xe5, 0xe0, 0x04, 0x0c, 0xe5, 0xe0, 0x04, 0x29, 0x98, 0xe0, 0x04, 0x5a, 0x00};
static u8 mode5_green[]={0x40, 0x0c, 0x43, 0x0b, 0xe0, 0x80, 0x19, 0x30, 0xe0, 0x80, 0x19, 0x30, 0xe0, 0x80, 0x19, 0xb0, 0xe0, 0x80, 0x19, 0xb0, 0xe0, 0x80, 0x43, 0x8b, 0xe0, 0x80, 0x5a, 0x00};


//[pattern_id : 6, PowerOff]
//static u8 mode6_red[] = {0x08, 0x7e, 0x08, 0x7f, 0x10, 0xff, 0x10, 0xFe};
//static u8 mode6_green[] = {0x08, 0x7e, 0x08, 0x7f, 0x10, 0xff, 0x10, 0xFe};
//static u8 mode6_blue[] = {0x08, 0x7e, 0x08, 0x7f, 0x10, 0xff, 0x10, 0xFe};
//[Tuning 20121119]
static u8 mode6_red[] = {0xE0, 0x0C, 0x40, 0x00, 0x0C, 0x2F, 0x06, 0x28, 0x05, 0x2D, 0x06, 0x2A, 0x06, 0x25, 0x03, 0xDC, 0x02, 0xFA, 0x48, 0x00, 0x03, 0x54, 0x44, 0x01, 0x23, 0x06, 0x31, 0x84, 0x06, 0xA8, 0x0C, 0xAF};
static u8 mode6_green[] = {0xE0, 0x80, 0x40, 0x00, 0x52, 0x00, 0x0B, 0x15, 0x05, 0x2D, 0x03, 0x48, 0x03, 0x4B, 0x09, 0x1B, 0x02, 0x63, 0x19, 0x89, 0x03, 0xCA, 0x04, 0xC1, 0x05, 0xB2, 0x06, 0xA6, 0x12, 0x8D, 0x52, 0x00};
static u8 mode6_blue[] = {0xE0, 0x80, 0x40, 0x00, 0x12, 0xFE, 0x40, 0xC0, 0x0A, 0x18, 0x06, 0xA6, 0x06, 0xAA, 0x03, 0xCF, 0x04, 0xB6, 0x52, 0x00,};


//[pattern_id : 7, MissedNoti]
//static u8 mode7_green[]={0x40, 0xff, 0x02, 0xff, 0x02, 0xfe, 0x48, 0x00, 0x40, 0xff, 0x02, 0xff, 0x02, 0xfe, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x52, 0x00};
//static u8 mode7_blue[]={0x40, 0xff, 0x02, 0xff, 0x02, 0xfe, 0x48, 0x00, 0x40, 0xff, 0x02, 0xff, 0x02, 0xfe, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x52, 0x00};
//[Tuning 20121119]
//static u8 mode7_red[]={0x40, 0x00, 0x10, 0xFE, 0x40, 0x5D, 0xE2, 0x00, 0x07, 0xAD, 0xE2, 0x00, 0x07, 0xAE, 0xE2, 0x00, 0x48, 0x00, 0x40, 0x5D, 0xE2, 0x00, 0x07, 0xAD, 0xE2, 0x00, 0x07, 0xAE, 0xE2, 0x00, 0x25, 0xFE,};
//static u8 mode7_green[]={0x40, 0x00, 0x10, 0xFE, 0x40, 0xCD, 0xE2, 0x00, 0x03, 0xE6, 0xE2, 0x00, 0x03, 0xE5, 0xE2, 0x00, 0x48, 0x00, 0x40, 0xCD, 0xE2, 0x00, 0x03, 0xE6, 0xE2, 0x00, 0x03, 0xE5, 0xE2, 0x00, 0x25, 0xFE,};
//static u8 mode7_blue[]={0x40, 0x00, 0x10, 0xFE, 0x40, 0xE6, 0xE0, 0x06, 0x03, 0xF2, 0xE0, 0x06, 0x03, 0xF2, 0xE0, 0x06, 0x48, 0x00, 0x40, 0xE6, 0xE0, 0x06, 0x03, 0xF2, 0xE0, 0x06, 0x03, 0xF2, 0xE0, 0x06, 0x25, 0xFE,};
/*K-pjt Scenario & Pattern change[2013.02.04] */
static u8 mode7_red[]={};
static u8 mode7_green[]={0x40, 0x00, 0x10, 0xfe, 0x40, 0xff, 0x02, 0xd4, 0x02, 0xd4, 0x02, 0xd4, 0x48, 0x00, 0x40, 0xff, 0x02, 0xd4, 0x02, 0xd4, 0x02, 0xd4, 0x25, 0xfe};
static u8 mode7_blue[]={};


//[pattern_id : 14, MissedNoti(favorite)]
static u8 mode8_red[]={0x40, 0x00, 0x10, 0xFE, 0x40, 0xE6, 0xE2, 0x00, 0x03, 0xF2, 0xE2, 0x00, 0x03, 0xF2, 0xE2, 0x00, 0x48, 0x00, 0x40, 0xE6, 0xE2, 0x00, 0x03, 0xF2, 0xE2, 0x00, 0x03, 0xF2, 0xE2, 0x00, 0x25, 0xFE,};
static u8 mode8_green[]={0x40, 0x00, 0x10, 0xFE, 0x40, 0x66, 0x4F, 0x00, 0x0B, 0xA8, 0xE0, 0x80, 0x0B, 0xA8, 0xE0, 0x80, 0x40, 0x66, 0x4F, 0x00, 0x09, 0xB2, 0xE0, 0x80, 0x09, 0xB2, 0xE0, 0x80, 0x1A, 0xFE,};
static u8 mode8_blue[]={0x40, 0x00, 0x10, 0xFE, 0x40, 0x73, 0x4F, 0x00, 0x08, 0xBC, 0xE0, 0x80, 0x0F, 0x9E, 0xE0, 0x80, 0x40, 0x73, 0x4F, 0x00, 0x05, 0xD5, 0xE0, 0x80, 0x10, 0x9C, 0xE0, 0x80, 0x1A, 0xFE,};

static struct lp5521_led_pattern board_led_patterns[] = {
	{
		.r = mode1_red,
		.g = mode1_green,
		.b = mode1_blue,
		.size_r = ARRAY_SIZE(mode1_red),
		.size_g = ARRAY_SIZE(mode1_green),
		.size_b = ARRAY_SIZE(mode1_blue),
	},
	{
		.r = mode2_red,
		.g = mode2_green,
		.b = mode2_blue,
		.size_r = ARRAY_SIZE(mode2_red),
		.size_g = ARRAY_SIZE(mode2_green),
		.size_b = ARRAY_SIZE(mode2_blue),
		},
	{
		.r = mode3_red,
		/*.g = mode3_green,*/
		/*.b = mode3_blue,*/
		.size_r = ARRAY_SIZE(mode3_red),
		/*.size_g = ARRAY_SIZE(mode3_green),*/
		/*.size_b = ARRAY_SIZE(mode3_blue),*/
	},
	{
//		.r = mode4_red,
		.g = mode4_green,
//		.b = mode4_blue,
//		.size_r = ARRAY_SIZE(mode4_red),
		.size_g = ARRAY_SIZE(mode4_green),
//		.size_b = ARRAY_SIZE(mode4_blue),
	},
	{
		.r = mode5_red,
		.g = mode5_green,
//		.b = mode5_blue,
		.size_r = ARRAY_SIZE(mode5_red),
		.size_g = ARRAY_SIZE(mode5_green),
//		.size_b = ARRAY_SIZE(mode5_blue),
	},
	{
		.r = mode6_red,
		.g = mode6_green,
		.b = mode6_blue,
		.size_r = ARRAY_SIZE(mode6_red),
		.size_g = ARRAY_SIZE(mode6_green),
		.size_b = ARRAY_SIZE(mode6_blue),
	},
	{
		.r = mode7_red,
		.g = mode7_green,
		.b = mode7_blue,
		.size_r = ARRAY_SIZE(mode7_red),
		.size_g = ARRAY_SIZE(mode7_green),
		.size_b = ARRAY_SIZE(mode7_blue),
	},
	{
		.r = mode8_red,
		.g = mode8_green,
		.b = mode8_blue,
		.size_r = ARRAY_SIZE(mode8_red),
		.size_g = ARRAY_SIZE(mode8_green),
		.size_b = ARRAY_SIZE(mode8_blue),
	},
};


#define LP5521_ENABLE PM8921_GPIO_PM_TO_SYS(21)

static struct gpio_i2c_pin rgb_i2c_pin[] = {
	[0] = {
		.sda_pin	= RGB_GPIO_I2C_SDA,
		.scl_pin	= RGB_GPIO_I2C_SCL,
		.reset_pin	= 0,
	},
};

static int lp5521_setup(void)       
{
       
       int rc = 0;

       printk("lp5521_enable\n\n");
       rc = gpio_request(RGB_GPIO_RGB_EN, "lp5521_led");

       if(rc){
              printk("lp5521_request failed\n");
              return rc;
       }

	rc = gpio_tlmm_config(GPIO_CFG(RGB_GPIO_RGB_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	
       if(rc){
              printk("lp5521_config failed\n");
              return rc;
       }
       return rc;
}

static void lp5521_enable(bool state)
{
       if(state){
              gpio_set_value(RGB_GPIO_RGB_EN, 1);
              printk(KERN_INFO"lp5521_enable_set\n");
       }
       else{
              gpio_set_value(RGB_GPIO_RGB_EN, 0);
              printk(KERN_INFO"lp5521_disable_set\n");
       }
       
       return;
}
      
#define LP5521_CONFIGS	(LP5521_PWM_HF | LP5521_PWRSAVE_EN | \
			LP5521_CP_MODE_AUTO | \
			LP5521_CLK_SRC_EXT)

static struct lp5521_platform_data lp5521_pdata = {
	.led_config = lp5521_led_config,
	.num_channels = ARRAY_SIZE(lp5521_led_config),
	.clock_mode = LP5521_CLOCK_EXT,
	.update_config = LP5521_CONFIGS,
	.patterns = board_led_patterns,
	.num_patterns = ARRAY_SIZE(board_led_patterns),
       .setup_resources = lp5521_setup,
       .enable = lp5521_enable
};

static struct i2c_board_info lp5521_board_info[] __initdata = {
	{
		I2C_BOARD_INFO("lp5521", 0x32),
		.platform_data = &lp5521_pdata,
	},
};
static struct i2c_gpio_platform_data rgb_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 1,
};

static struct platform_device rgb_i2c_device = {
	.name = "i2c-gpio",
	.dev.platform_data = &rgb_i2c_pdata,
};

static void __init lp5521_init_i2c_rgb(int bus_num)
{
	int rc=0;
	
	rgb_i2c_device.id = bus_num;

	lge_init_gpio_i2c_pin_pullup(&rgb_i2c_pdata, rgb_i2c_pin[0], &lp5521_board_info[0]);

	i2c_register_board_info(bus_num, lp5521_board_info, ARRAY_SIZE(lp5521_board_info));

	platform_device_register(&rgb_i2c_device);

	rc = gpio_tlmm_config(GPIO_CFG(RGB_GPIO_RGB_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);

	if (rc) {
			pr_err("[LP5521] %s: Error requesting GPIO gpio_tlmm_config, ret %d\n", __func__, rc);
		} else {
		    pr_err ("[LP5521] %s: success gpio_tlmm_config, ret %d\n", __func__, rc);
		}
}


#endif /*LP5521*/

/*[LGE_BSP_E][yunmo.yang@lge.com] LP5521 RGB Driver*/

/*LGE_CHANGE_S : byungyong.hwang@lge.com touch - Synaptics s3203 panel	for V7*/
#if defined(CONFIG_LGE_TOUCHSCREEN_SYNAPTICS_I2C_RMI4)
#ifdef LGE_TOUCHENABLE_USING_PMIC_LD0
static struct regulator *regulator_ts;
#endif
#include <linux/i2c-gpio.h>

static struct gpio_i2c_pin synaptics_ts_i2c_pin[] = {
   [0] = {
       .sda_pin    = SYNAPTICS_TS_I2C_SDA,
       .scl_pin    = SYNAPTICS_TS_I2C_SCL,
       .reset_pin  = 0,
       .irq_pin    = SYNAPTICS_TS_I2C_INT_GPIO,
   },
};

static struct i2c_gpio_platform_data synaptics_ts_i2c_pdata = {
   .sda_is_open_drain  = 0,
   .scl_is_open_drain  = 0,
   .udelay         = 1,
};

static struct platform_device synaptics_ts_i2c_device = {
   .name   = "i2c-gpio",
   .dev.platform_data = &synaptics_ts_i2c_pdata,
};

static int synaptics_ts_config_gpio(int config)
{
   if(config) {
       printk("[Touch D]  synaptics_ts_config_gpio on\n");
       /* for wake state */
       gpio_tlmm_config(GPIO_CFG(SYNAPTICS_TS_I2C_INT_GPIO, 0, GPIO_CFG_INPUT,
                GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
       gpio_tlmm_config(GPIO_CFG(SYNAPTICS_TS_I2C_SDA, 0, GPIO_CFG_OUTPUT,
                GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
       gpio_tlmm_config(GPIO_CFG(SYNAPTICS_TS_I2C_SCL, 0, GPIO_CFG_OUTPUT,
                GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
       gpio_tlmm_config(GPIO_CFG(TS_TOUCH_ID, 0, GPIO_CFG_INPUT,
                GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);

   }
   else {
       /* for sleep state */
       printk("[Touch D] synaptics_ts_config_gpio off\n");
       gpio_tlmm_config(GPIO_CFG(SYNAPTICS_TS_I2C_INT_GPIO, 0, GPIO_CFG_INPUT,
                GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
       gpio_tlmm_config(GPIO_CFG(SYNAPTICS_TS_I2C_SDA, 0, GPIO_CFG_OUTPUT,
                GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
       gpio_tlmm_config(GPIO_CFG(SYNAPTICS_TS_I2C_SCL, 0, GPIO_CFG_OUTPUT,
                GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
       gpio_tlmm_config(GPIO_CFG(TS_TOUCH_ID, 0, GPIO_CFG_INPUT,
                GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
   }
   return 0;
}

int synaptics_t1320_power_on(int onoff)
{
   int rc = 0;

       printk(KERN_INFO "[Touch D] %s: power %s\n",__func__, onoff ? "On" : "Off");

// LGE_CHANGE_S,narasimha.chikka@lge.com,Touch Enable by PMIC LD0
#ifdef LGE_TOUCHENABLE_USING_PMIC_LD0
	regulator_ts = regulator_get(NULL, "rfrx1");
	if (regulator_ts == NULL)
			pr_err("%s: regulator_get(regulator_ts) failed\n",__func__);
		
	rc = regulator_set_voltage(regulator_ts, 3000000, 3000000);
	if (rc < 0)
			pr_err("%s: regulator_set_voltage(regulator_ts) failed\n", __func__);
	if(onoff){
		synaptics_ts_config_gpio(1);
		rc = regulator_enable(regulator_ts);
		if (rc < 0) {
			pr_err("%s: regulator_enable(regulator_ts) failed\n", __func__);
		}
	}
	else{
		synaptics_ts_config_gpio(0);
		rc = regulator_disable(regulator_ts);
		if (rc < 0) {
			pr_err("%s: regulator_disble(regulator_ts) failed\n", __func__);
		}
	}

#else
	if (onoff) {
		synaptics_ts_config_gpio(1);
		gpio_tlmm_config(GPIO_CFG(TS_TOUCH_LDO_EN, 0, GPIO_CFG_OUTPUT,
				 GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		gpio_set_value(TS_TOUCH_LDO_EN, 1);
	}
	else{
		synaptics_ts_config_gpio(0);
		gpio_tlmm_config(GPIO_CFG(TS_TOUCH_LDO_EN, 0, GPIO_CFG_OUTPUT,
				 GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		gpio_set_value(TS_TOUCH_LDO_EN, 0);
	}
	
#endif
// LGE_CHANGE_E,narasimha.chikka@lge.com,Touch Enable by PMIC LD0
	return rc;
}


static struct touch_power_module touch_pwr = {
   .use_regulator  = 0,
   .power          = synaptics_t1320_power_on,
};

static struct touch_device_caps touch_caps = {
   .button_support             = 1,
   .y_button_boundary          = 0,
#if defined(CONFIG_LGE_TOUCH_FOUR_BUTTON_SUPPORT)
   .number_of_button           = 4,
   .button_name                = {KEY_BACK,KEY_HOME, KEY_MENU,KEY_SIM_SWITCH},
#else
   .number_of_button           = 2,
   .button_name                = {KEY_BACK,KEY_MENU},
#endif
   .button_margin              = 0,
   .is_width_supported         = 1,
   .is_pressure_supported      = 1,
   .is_id_supported            = 1,
   .max_width                  = 15,
   .max_pressure               = 0xFF,
   .max_id                     = 10,
   .lcd_x                      = TS_X_MAX,
   .lcd_y                      = TS_Y_MAX,
   .x_max                      = 1100,
   .y_max                      = 1901,
};

static struct touch_operation_role touch_role = {
   .operation_mode         = 1,
   .key_type               = TOUCH_HARD_KEY,
	.report_mode			= REDUCED_REPORT_MODE,
	.delta_pos_threshold 	= 1,
	.orientation			= 0,
	.report_period			= 10000000,
	.booting_delay			= 400,
	.reset_delay			= 20,
	.suspend_pwr			= POWER_OFF,
	.resume_pwr 			= POWER_ON,
	.jitter_filter_enable	= 0,
	.jitter_curr_ratio		= 30,
	.accuracy_filter_enable = 1,
	.irqflags				= IRQF_TRIGGER_FALLING,
#ifdef CUST_G_TOUCH
	.show_touches			= 0,
	.pointer_location		= 0,
	.ta_debouncing_count    = 2,
	.ta_debouncing_finger_num  = 2,
	.ghost_detection_enable = 1,
	.pen_enable		= 0,
#endif
};

static struct syna_touch_platform_data lge_ts_data = {
	.int_pin	= SYNAPTICS_TS_I2C_INT_GPIO,
	.reset_pin	= 0,
	.maker		= "Synaptics",
#if defined(CONFIG_LGE_TOUCH_FOUR_BUTTON_SUPPORT)
	.fw_version = "E049",
#else
	.fw_version = "E008",
#endif
	.caps		= &touch_caps,
	.role		= &touch_role,
	.pwr		= &touch_pwr,
};

static struct i2c_board_info synaptics_ts_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO(LGE_TOUCH_NAME, SYNAPTICS_TS_I2C_SLAVE_ADDR),
		.platform_data = &lge_ts_data,
		.irq = MSM_GPIO_TO_INT(SYNAPTICS_TS_I2C_INT_GPIO),
	},
};


static void __init synaptics_init_i2c_touch(int bus_num)
{
	synaptics_ts_i2c_device.id = bus_num;

	lge_init_gpio_i2c_pin(&synaptics_ts_i2c_pdata, synaptics_ts_i2c_pin[0], &synaptics_ts_i2c_bdinfo[0]);
	i2c_register_board_info(bus_num, &synaptics_ts_i2c_bdinfo[0], 1);
	platform_device_register(&synaptics_ts_i2c_device);
}
#else
/*LGE_CHANGE_E : byungyong.hwang@lge.com touch - Synaptics s3203 panel  for V7*/
static struct regulator_bulk_data regs_atmel[] = {
	{ .supply = "ldo12", .min_uV = 2700000, .max_uV = 3300000 },
	{ .supply = "smps3", .min_uV = 1800000, .max_uV = 1800000 },
};

#define ATMEL_TS_GPIO_IRQ 82

static int atmel_ts_power_on(bool on)
{
	int rc = on ?
		regulator_bulk_enable(ARRAY_SIZE(regs_atmel), regs_atmel) :
		regulator_bulk_disable(ARRAY_SIZE(regs_atmel), regs_atmel);

	if (rc)
		pr_err("%s: could not %sable regulators: %d\n",
				__func__, on ? "en" : "dis", rc);
	else
		msleep(50);

	return rc;
}

static int atmel_ts_platform_init(struct i2c_client *client)
{
	int rc;
	struct device *dev = &client->dev;

	rc = regulator_bulk_get(dev, ARRAY_SIZE(regs_atmel), regs_atmel);
	if (rc) {
		dev_err(dev, "%s: could not get regulators: %d\n",
				__func__, rc);
		goto out;
	}

	rc = regulator_bulk_set_voltage(ARRAY_SIZE(regs_atmel), regs_atmel);
	if (rc) {
		dev_err(dev, "%s: could not set voltages: %d\n",
				__func__, rc);
		goto reg_free;
	}

	rc = gpio_tlmm_config(GPIO_CFG(ATMEL_TS_GPIO_IRQ, 0,
				GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
				GPIO_CFG_8MA), GPIO_CFG_ENABLE);
	if (rc) {
		dev_err(dev, "%s: gpio_tlmm_config for %d failed\n",
			__func__, ATMEL_TS_GPIO_IRQ);
		goto reg_free;
	}

	/* configure touchscreen interrupt gpio */
	rc = gpio_request(ATMEL_TS_GPIO_IRQ, "atmel_maxtouch_gpio");
	if (rc) {
		dev_err(dev, "%s: unable to request gpio %d\n",
			__func__, ATMEL_TS_GPIO_IRQ);
		goto ts_gpio_tlmm_unconfig;
	}

	rc = gpio_direction_input(ATMEL_TS_GPIO_IRQ);
	if (rc < 0) {
		dev_err(dev, "%s: unable to set the direction of gpio %d\n",
			__func__, ATMEL_TS_GPIO_IRQ);
		goto free_ts_gpio;
	}
	return 0;

free_ts_gpio:
	gpio_free(ATMEL_TS_GPIO_IRQ);
ts_gpio_tlmm_unconfig:
	gpio_tlmm_config(GPIO_CFG(ATMEL_TS_GPIO_IRQ, 0,
				GPIO_CFG_INPUT, GPIO_CFG_NO_PULL,
				GPIO_CFG_2MA), GPIO_CFG_DISABLE);
reg_free:
	regulator_bulk_free(ARRAY_SIZE(regs_atmel), regs_atmel);
out:
	return rc;
}

static int atmel_ts_platform_exit(struct i2c_client *client)
{
	gpio_free(ATMEL_TS_GPIO_IRQ);
	gpio_tlmm_config(GPIO_CFG(ATMEL_TS_GPIO_IRQ, 0,
				GPIO_CFG_INPUT, GPIO_CFG_NO_PULL,
				GPIO_CFG_2MA), GPIO_CFG_DISABLE);
	regulator_bulk_free(ARRAY_SIZE(regs_atmel), regs_atmel);
	return 0;
}

static u8 atmel_ts_read_chg(void)
{
	return gpio_get_value(ATMEL_TS_GPIO_IRQ);
}

static u8 atmel_ts_valid_interrupt(void)
{
	return !atmel_ts_read_chg();
}


static struct maxtouch_platform_data atmel_ts_pdata = {
	.numtouch = 4,
	.init_platform_hw = atmel_ts_platform_init,
	.exit_platform_hw = atmel_ts_platform_exit,
	.power_on = atmel_ts_power_on,
	.display_res_x = 480,
	.display_res_y = 864,
	.min_x = ATMEL_X_OFFSET,
	.max_x = (505 - ATMEL_X_OFFSET),
	.min_y = ATMEL_Y_OFFSET,
	.max_y = (863 - ATMEL_Y_OFFSET),
	.valid_interrupt = atmel_ts_valid_interrupt,
	.read_chg = atmel_ts_read_chg,
};

static struct i2c_board_info atmel_ts_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO(ATMEL_TS_I2C_NAME, 0x4a),
		.platform_data = &atmel_ts_pdata,
		.irq = MSM_GPIO_TO_INT(ATMEL_TS_GPIO_IRQ),
	},
};
#endif 

#ifdef CONFIG_LGE_NFC
// 2012.09.26 garam.kim@lge.com NFC registration
static struct gpio_i2c_pin nfc_i2c_pin[] = {
	[0] = {
		.sda_pin	= NFC_GPIO_I2C_SDA,
		.scl_pin	= NFC_GPIO_I2C_SCL,
		.reset_pin	= NFC_GPIO_VEN,
		.irq_pin	= NFC_GPIO_IRQ,
	},
};

static struct i2c_gpio_platform_data nfc_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 2,
};

static struct platform_device nfc_i2c_device = {
	.name = "i2c-gpio",
	.dev.platform_data = &nfc_i2c_pdata,
};

static struct pn544_i2c_platform_data nfc_pdata = {
	.ven_gpio 		= NFC_GPIO_VEN,
	.irq_gpio 	 	= NFC_GPIO_IRQ,
	.scl_gpio		= NFC_GPIO_I2C_SCL,
	.sda_gpio		= NFC_GPIO_I2C_SDA,
	.firm_gpio		= NFC_GPIO_FIRM,
};

static struct i2c_board_info nfc_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("pn544", NFC_I2C_SLAVE_ADDR),
		.platform_data = &nfc_pdata,
		.irq = MSM_GPIO_TO_INT(NFC_GPIO_IRQ),
	},
};

static void __init v7_init_i2c_nfc(int bus_num)
{
	int ret;
/*
 	gpio_tlmm_config(GPIO_CFG(NFC_GPIO_FIRM, 0, GPIO_CFG_OUTPUT,
 				GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(NFC_GPIO_VEN, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(NFC_GPIO_IRQ, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE); 
	gpio_tlmm_config(GPIO_CFG(NFC_GPIO_I2C_SDA, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE); 
	gpio_tlmm_config(GPIO_CFG(NFC_GPIO_I2C_SCL, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE); 
*/

	gpio_tlmm_config(GPIO_CFG(NFC_GPIO_FIRM, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(NFC_GPIO_VEN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	gpio_set_value(NFC_GPIO_VEN, 1);
	gpio_tlmm_config(GPIO_CFG(NFC_GPIO_IRQ, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
    nfc_i2c_bdinfo->irq = MSM_GPIO_TO_INT(NFC_GPIO_IRQ);
	nfc_i2c_device.id = bus_num;

	ret = lge_init_gpio_i2c_pin(&nfc_i2c_pdata, nfc_i2c_pin[0],	&nfc_i2c_bdinfo[0]);
	
	ret = i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID, &nfc_i2c_bdinfo[0], 1);
	
	platform_device_register(&nfc_i2c_device);	
}
#endif
#if 0
static struct msm_handset_platform_data hs_platform_data = {
	.hs_name = "7k_handset",
	.pwr_key_delay_ms = 500, /* 0 will disable end key */
};

static struct platform_device hs_pdev = {
	.name   = "msm-handset",
	.id     = -1,
	.dev    = {
		.platform_data = &hs_platform_data,
	},
};

#define FT5X06_IRQ_GPIO		48
#define FT5X06_RESET_GPIO	26

static ssize_t
ft5x06_virtual_keys_register(struct kobject *kobj,
			     struct kobj_attribute *attr,
			     char *buf)
{
	return snprintf(buf, 200,
	__stringify(EV_KEY) ":" __stringify(KEY_MENU)  ":40:510:80:60"
	":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":120:510:80:60"
	":" __stringify(EV_KEY) ":" __stringify(KEY_SEARCH) ":200:510:80:60"
	":" __stringify(EV_KEY) ":" __stringify(KEY_BACK)   ":280:510:80:60"
	"\n");
}

static struct kobj_attribute ft5x06_virtual_keys_attr = {
	.attr = {
		.name = "virtualkeys.ft5x06_ts",
		.mode = S_IRUGO,
	},
	.show = &ft5x06_virtual_keys_register,
};

static struct attribute *ft5x06_virtual_key_properties_attrs[] = {
	&ft5x06_virtual_keys_attr.attr,
	NULL,
};

static struct attribute_group ft5x06_virtual_key_properties_attr_group = {
	.attrs = ft5x06_virtual_key_properties_attrs,
};

struct kobject *ft5x06_virtual_key_properties_kobj;

static struct ft5x06_ts_platform_data ft5x06_platformdata = {
	.x_max		= 320,
	.y_max		= 480,
	.reset_gpio	= FT5X06_RESET_GPIO,
	.irq_gpio	= FT5X06_IRQ_GPIO,
	.irqflags	= IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
};

static struct i2c_board_info ft5x06_device_info[] __initdata = {
	{
		I2C_BOARD_INFO("ft5x06_ts", 0x38),
		.platform_data = &ft5x06_platformdata,
		.irq = MSM_GPIO_TO_INT(FT5X06_IRQ_GPIO),
	},
};

static void __init ft5x06_touchpad_setup(void)
{
	int rc;

	rc = gpio_tlmm_config(GPIO_CFG(FT5X06_IRQ_GPIO, 0,
			GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
			GPIO_CFG_8MA), GPIO_CFG_ENABLE);
	if (rc)
		pr_err("%s: gpio_tlmm_config for %d failed\n",
			__func__, FT5X06_IRQ_GPIO);

	rc = gpio_tlmm_config(GPIO_CFG(FT5X06_RESET_GPIO, 0,
			GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN,
			GPIO_CFG_8MA), GPIO_CFG_ENABLE);
	if (rc)
		pr_err("%s: gpio_tlmm_config for %d failed\n",
			__func__, FT5X06_RESET_GPIO);

	ft5x06_virtual_key_properties_kobj =
			kobject_create_and_add("board_properties", NULL);

	if (ft5x06_virtual_key_properties_kobj)
		rc = sysfs_create_group(ft5x06_virtual_key_properties_kobj,
				&ft5x06_virtual_key_properties_attr_group);

	if (!ft5x06_virtual_key_properties_kobj || rc)
		pr_err("%s: failed to create board_properties\n", __func__);

	i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
				ft5x06_device_info,
				ARRAY_SIZE(ft5x06_device_info));
}

/* SKU3/SKU7 keypad device information */
#define KP_INDEX_SKU3(row, col) ((row)*ARRAY_SIZE(kp_col_gpios_sku3) + (col))
static unsigned int kp_row_gpios_sku3[] = {31, 32};
static unsigned int kp_col_gpios_sku3[] = {36, 37};

static const unsigned short keymap_sku3[] = {
	[KP_INDEX_SKU3(0, 0)] = KEY_VOLUMEUP,
	[KP_INDEX_SKU3(0, 1)] = KEY_VOLUMEDOWN,
	[KP_INDEX_SKU3(1, 1)] = KEY_CAMERA,
};

static struct gpio_event_matrix_info kp_matrix_info_sku3 = {
	.info.func      = gpio_event_matrix_func,
	.keymap         = keymap_sku3,
	.output_gpios   = kp_row_gpios_sku3,
	.input_gpios    = kp_col_gpios_sku3,
	.noutputs       = ARRAY_SIZE(kp_row_gpios_sku3),
	.ninputs        = ARRAY_SIZE(kp_col_gpios_sku3),
	.settle_time.tv64 = 40 * NSEC_PER_USEC,
	.poll_time.tv64 = 20 * NSEC_PER_MSEC,
	.flags          = GPIOKPF_LEVEL_TRIGGERED_IRQ | GPIOKPF_DRIVE_INACTIVE |
				GPIOKPF_PRINT_UNMAPPED_KEYS,
};

static struct gpio_event_info *kp_info_sku3[] = {
	&kp_matrix_info_sku3.info,
};
static struct gpio_event_platform_data kp_pdata_sku3 = {
	.name           = "7x27a_kp",
	.info           = kp_info_sku3,
	.info_count     = ARRAY_SIZE(kp_info_sku3)
};

static struct platform_device kp_pdev_sku3 = {
	.name   = GPIO_EVENT_DEV_NAME,
	.id     = -1,
	.dev    = {
		.platform_data  = &kp_pdata_sku3,
	},
};

static struct led_info ctp_backlight_info = {
	.name           = "button-backlight",
	.flags          = PM_MPP__I_SINK__LEVEL_40mA << 16 | PM_MPP_7,
};

static struct led_platform_data ctp_backlight_pdata = {
	.leds = &ctp_backlight_info,
	.num_leds = 1,
};

static struct platform_device pmic_mpp_leds_pdev = {
	.name   = "pmic-mpp-leds",
	.id     = -1,
	.dev    = {
		.platform_data  = &ctp_backlight_pdata,
	},
};
#endif

// LGE_CHANGE_S,narasimha.chikka@lge.com,Add PMIC Key LED device
#if defined(CONFIG_LEDS_PMIC_MPP)
static struct led_info ctp_backlight_info = {
	.name           = "button-backlight",
	.flags          = PM_MPP__I_SINK__LEVEL_5mA << 16 | PM_MPP_5,
};

static struct led_platform_data ctp_backlight_pdata = {
	.leds = &ctp_backlight_info,
	.num_leds = 1,
};

static struct platform_device pmic_mpp_leds_pdev = {
	.name   = "pmic-mpp-leds",
	.id     = -1,
	.dev    = {
		.platform_data  = &ctp_backlight_pdata,
	},
};
#endif
// LGE_CHANGE_E,narasimha.chikka@lge.com,Add PMIC Key LED device

void __init msm7627a_add_io_devices(void)
{
	/*LGE_CHANGE_S : seven.kim@lge.com JB 2035.2B Migration*/
	/* ignore end key as this target doesn't need it */
	hs_platform_data.ignore_end_key = true;
	/*LGE_CHANGE_E : seven.kim@lge.com JB 2035.2B Migration*/
	
platform_add_devices(
	vee7_input_devices, ARRAY_SIZE(vee7_input_devices));
platform_add_devices(
	vee7_gpio_input_devices, ARRAY_SIZE(vee7_gpio_input_devices));

// LGE_CHANGE_S,narasimha.chikka@lge.com,Add PMIC Key LED device
#if defined(CONFIG_LEDS_PMIC_MPP)
platform_device_register(&pmic_mpp_leds_pdev);
#endif
// LGE_CHANGE_E,narasimha.chikka@lge.com,Add PMIC Key LED device

/*LGE_CHANGE_S : byungyong.hwang@lge.com touch - Synaptics s3203 panel	for V7*/
#if defined(CONFIG_LGE_TOUCHSCREEN_SYNAPTICS_I2C_RMI4)
		lge_add_gpio_i2c_device(synaptics_init_i2c_touch);
#else
/*LGE_CHANGE_E : byungyong.hwang@lge.com touch - Synaptics s3203 panel for V7*/

	/* touchscreen */
	if (machine_is_msm7625a_surf() || machine_is_msm7625a_ffa()) {
		atmel_ts_pdata.min_x = 0;
		atmel_ts_pdata.max_x = 480;
		atmel_ts_pdata.min_y = 0;
		atmel_ts_pdata.max_y = 320;
	}

	i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
				atmel_ts_i2c_info,
				ARRAY_SIZE(atmel_ts_i2c_info));
#endif
//[LGSI_SP4_BSP][kirankumar.vm@lge.com] Proximity driver using i2c bus no 5
	lge_add_gpio_i2c_device(vee7_init_i2c_prox);

/*[LGE_BSP_S][yunmo.yang@lge.com] LP5521 RGB Driver*/
#ifdef CONFIG_LEDS_LP5521	
	lge_add_gpio_i2c_device(lp5521_init_i2c_rgb);
#endif	
/*[LGE_BSP_E][yunmo.yang@lge.com] LP5521 RGB Driver*/	

#if defined (CONFIG_SENSORS_BMM050) ||defined(CONFIG_SENSORS_BMA250)
	lge_add_gpio_i2c_device(vee7_init_i2c_sensor);
#endif

#ifdef CONFIG_LGE_NFC
	lge_add_gpio_i2c_device(v7_init_i2c_nfc);
#endif

#if 0
	/* keypad */
	platform_device_register(&kp_pdev);
	/* headset */
	platform_device_register(&hs_pdev);

	/* LED: configure it as a pdm function */
	if (gpio_tlmm_config(GPIO_CFG(LED_GPIO_PDM, 3,
				GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL,
				GPIO_CFG_8MA), GPIO_CFG_ENABLE))
		pr_err("%s: gpio_tlmm_config for %d failed\n",
			__func__, LED_GPIO_PDM);
	else
		platform_device_register(&led_pdev);
#endif
	/* Vibrator */
// LGE_CHANGE_S,narasimha.chikka@lge.com,PMIC Vibrator for Rev B
#ifdef CONFIG_MSM_RPC_VIBRATOR
#if (CONFIG_LGE_PCB_REVISION >= REV_B)
		msm_init_pmic_vibrator();
#endif
#endif	//CONFIG_MSM_RPC_VIBRATOR
// LGE_CHANGE_E,narasimha.chikka@lge.com,PMIC Vibrator for Rev B

}

void __init qrd7627a_add_io_devices(void)
{
#if 0
	int rc;

	/* touchscreen */
	if (machine_is_msm7627a_qrd1()) {
		i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
					synaptic_i2c_clearpad3k,
					ARRAY_SIZE(synaptic_i2c_clearpad3k));
	} else if (machine_is_msm7627a_evb() || machine_is_msm8625_evb() ||
			machine_is_msm8625_evt()) {
		/* Use configuration data for EVT */
		if (machine_is_msm8625_evt()) {
			mxt_config_array[0].config = mxt_config_data_evt;
			mxt_config_array[0].config_length =
					ARRAY_SIZE(mxt_config_data_evt);
			mxt_platform_data.panel_maxy = 875;
			mxt_vkey_setup();
		}

		rc = gpio_tlmm_config(GPIO_CFG(MXT_TS_IRQ_GPIO, 0,
				GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
				GPIO_CFG_8MA), GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("%s: gpio_tlmm_config for %d failed\n",
				__func__, MXT_TS_IRQ_GPIO);
		}

		rc = gpio_tlmm_config(GPIO_CFG(MXT_TS_RESET_GPIO, 0,
				GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN,
				GPIO_CFG_8MA), GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("%s: gpio_tlmm_config for %d failed\n",
				__func__, MXT_TS_RESET_GPIO);
		}

		i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
					mxt_device_info,
					ARRAY_SIZE(mxt_device_info));
	} else if (machine_is_msm7627a_qrd3() || machine_is_msm8625_qrd7()) {
		ft5x06_touchpad_setup();
	}

	/* headset */
	platform_device_register(&hs_pdev);

	/* vibrator */
#ifdef CONFIG_MSM_RPC_VIBRATOR
	msm_init_pmic_vibrator();
#endif

	/* keypad */
	if (machine_is_msm8625_evt())
		kp_matrix_info_8625.keymap = keymap_8625_evt;

	if (machine_is_msm7627a_evb() || machine_is_msm8625_evb() ||
			machine_is_msm8625_evt())
		platform_device_register(&kp_pdev_8625);
	else if (machine_is_msm7627a_qrd3() || machine_is_msm8625_qrd7())
		platform_device_register(&kp_pdev_sku3);

	/* leds */
	if (machine_is_msm7627a_evb() || machine_is_msm8625_evb()) {
		rc = gpio_tlmm_config(GPIO_CFG(LED_RED_GPIO_8625, 0,
				GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP,
				GPIO_CFG_16MA), GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("%s: gpio_tlmm_config for %d failed\n",
				__func__, LED_RED_GPIO_8625);
		}

		rc = gpio_tlmm_config(GPIO_CFG(LED_GREEN_GPIO_8625, 0,
				GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP,
				GPIO_CFG_16MA), GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("%s: gpio_tlmm_config for %d failed\n",
				__func__, LED_GREEN_GPIO_8625);
		}

		platform_device_register(&gpio_leds_8625);
		platform_device_register(&pmic_mpp_leds_pdev);
	}
#endif
}
