/*
 * MMS100S ISC Updater�� public ����Դϴ�.
 * !!����!!
 * ������ �����Ͻø� �ȵ˴ϴ�.
 */
#ifndef __MMS100S_ISC_Updater_H__
#define __MMS100S_ISC_Updater_H__

/*
 * Return values
 */
typedef enum
{
	MRET_NONE = -1,
	MRET_SUCCESS = 0,
	MRET_FILE_OPEN_ERROR,
	MRET_FILE_CLOSE_ERROR,
	MRET_FILE_FORMAT_ERROR,
	MRET_WRITE_BUFFER_ERROR,
	MRET_I2C_ERROR,
	MRET_MASS_ERASE_ERROR,
	MRET_FIRMWARE_WRITE_ERROR,
	MRET_FIRMWARE_VERIFY_ERROR,
	MRET_UPDATE_MODE_ENTER_ERROR,
	MRET_LIMIT
} eMFSRet_t;

/*
 * Boolean ���� type �� define.
 * �״�� �μŵ� �ǰ�, system�� �°� ���� �ּŵ� �˴ϴ�.
 */
typedef int mfs_bool_t;
#define MFS_TRUE		(0==0)
#define MFS_FALSE		(0!=0)

/*
 * Interfaces
 */
/* extern void MFS_open_bins(void); */
extern eMFSRet_t MFS_ISC_update(void);
/* extern mfs_bool_t MFS_close_bins(void); */

extern unsigned short MELFAS_binary_nLength;
extern unsigned char MELFAS_binary[];

/* 2012-10-23 JongWook-Park(blood9874@lge.com) [V3] Single Touch Bring Up */ 
extern unsigned short MELFAS_SINGLE_binary_nLength;
extern unsigned char MELFAS_SINGLE_binary[];

#endif /* __MMS100S_ISC_Updater_H__ */
