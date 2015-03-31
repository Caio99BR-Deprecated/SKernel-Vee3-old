/*
 *  arch/arm/mach-msm/lge/lge_ats_input.c
 *
 *  Copyright (c) 2010 LGE.
 *  
 *  All source code in this file is licensed under the following license
 *  except where indicated.
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  version 2 as published by the Free Software Foundation.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, you can find it at http://www.fsf.org
 */

#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/module.h>

#define DRIVER_NAME "ats_input"
// BEGIN: 0010583 alan.park@lge.com 2010.11.06
// ADD 0010583: [ETA/MTC] ETA Capture, Key, Touch, Logging / MTC Key, Logging 		
#if 0
extern int touch_get_x_max (void);
extern int touch_get_y_max(void);
// END: 0010583 alan.park@lge.com 2010.11.06 
#endif  
static struct input_dev *ats_input_dev;

/* add interface get ATS_INPUT_DEVICE [younchan.kim, 2010-06-11] */
struct input_dev *get_ats_input_dev(void)
{
	return ats_input_dev;
}



static int __devinit ats_input_probe(struct platform_device *pdev)
{
	int rc = 0;
	int i;

	ats_input_dev = input_allocate_device();
	if (!ats_input_dev) {
		printk(KERN_ERR "%s: not enough memory for input device\n", __func__);
		return -ENOMEM;
	}
	ats_input_dev->name = "ats_input";

	/* 2012-10-23 JongWook-Park(blood9874@lge.com) [V3] DIAG Key Input Code test Patch [START] */ 
	#if 0 /*seven temporally blocked */
	for(i=0; i<EV_CNT; i++)
		set_bit(i, ats_input_dev->evbit);
	for(i=0; i<KEY_CNT; i++)
		set_bit(i, ats_input_dev->keybit);
	set_bit(ABS_MT_TOUCH_MAJOR, ats_input_dev->absbit);
	clear_bit(EV_REP, ats_input_dev->evbit);
	#else
	set_bit(EV_ABS, ats_input_dev->evbit);
	set_bit(EV_KEY, ats_input_dev->evbit);
	ats_input_dev->evbit[0] = BIT_MASK(EV_ABS) | BIT_MASK(EV_KEY);

	for(i = 0; i<= KEY_0; i++)	
		ats_input_dev->keybit[BIT_WORD(i)] |= BIT_MASK(i);

	for(i = KEY_VOLUMEDOWN; i<= KEY_POWER; i++)	
		ats_input_dev->keybit[BIT_WORD(i)] |= BIT_MASK(i);

	ats_input_dev->keybit[BIT_WORD(KEY_SEND)] |= BIT_MASK(KEY_SEND);
	ats_input_dev->keybit[BIT_WORD(KEY_END)] |= BIT_MASK(KEY_END);

	for(i = KEY_NUMERIC_0; i<= KEY_NUMERIC_POUND; i++)	
		ats_input_dev->keybit[BIT_WORD(i)] |= BIT_MASK(i);

	set_bit(ABS_MT_TOUCH_MAJOR, ats_input_dev->absbit);
	clear_bit(EV_REP, ats_input_dev->evbit);
	#endif /*seven temporally blocked*/
	/* 2012-10-23 JongWook-Park(blood9874@lge.com) [V3] DIAG Key Input Code test Patch [END] */ 

	rc = input_register_device(ats_input_dev);
	if (rc)
		printk(KERN_ERR"%s : input_register_device failed\n", __func__);

	/* FIXME: Touch resolution should be given by platform data */
// BEGIN: 0010583 alan.park@lge.com 2010.11.06
// MOD 0010583: [ETA/MTC] ETA Capture, Key, Touch, Logging / MTC Key, Logging  
#if 0
	input_set_abs_params(ats_input_dev, ABS_MT_POSITION_X, 0, touch_get_x_max(), 0, 0);
	input_set_abs_params(ats_input_dev, ABS_MT_POSITION_Y, 0, touch_get_y_max(), 0, 0);
#endif 
// END: 0010583 alan.park@lge.com 2010.11.06 
	return rc;
}

static int ats_input_remove(struct platform_device *pdev)
{
	input_unregister_device(ats_input_dev);
	return 0;
}

static struct platform_driver ats_input_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.probe	 = ats_input_probe,
	.remove = ats_input_remove,
};

static int __init ats_input_init(void)
{
	return platform_driver_register(&ats_input_driver);
}


static void __exit ats_input_exit(void)
{
	platform_driver_unregister(&ats_input_driver);
}


EXPORT_SYMBOL(get_ats_input_dev);
//#LGE_CHANGE : 2012-10-24 Sanghun,Lee(eee3114.@lge.com) sensor change from bmc150 to bmc050
#if 1
late_initcall(ats_input_init);
#else
module_init(ats_input_init);
#endif
module_exit(ats_input_exit);

MODULE_AUTHOR("LG Electronics Inc.");
MODULE_DESCRIPTION("ATS_INPUT driver");
MODULE_LICENSE("GPL v2");
