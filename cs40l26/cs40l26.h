/* SPDX-License-Identifier: GPL-2.0
 *
 * cs40l26.h -- CS40L26 Boosted Haptic Driver with Integrated DSP and
 * Waveform Memory with Advanced Closed Loop Algorithms and LRA protection
 *
 * Copyright 2022 Cirrus Logic, Inc.
 *
 * Author: Fred Treven <fred.treven@cirrus.com>
 */

#ifndef __CS40L26_H__
#define __CS40L26_H__

#include <linux/input.h>
#include <linux/regmap.h>
#include <linux/mfd/core.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/string.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/regulator/consumer.h>
#include <linux/delay.h>
#include <linux/completion.h>
#include <linux/firmware.h>
#include <linux/sysfs.h>
#include <linux/bitops.h>
#include <linux/pm_runtime.h>
#include <linux/debugfs.h>
#include <linux/timer.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <sound/tlv.h>
#include <trace/hooks/systrace.h>

#include "cl_dsp.h"

#define CS40L26_LASTREG					0x3C7DFE8

#define CS40L26_DEVID					0x0
#define CS40L26_REVID					0x4
#define CS40L26_TEST_KEY_CTRL				0x40
#define CS40L26_GLOBAL_ENABLES				0x2014
#define CS40L26_BLOCK_ENABLES2				0x201C
#define CS40L26_ERROR_RELEASE				0x2034
#define CS40L26_PWRMGT_CTL				0x2900
#define CS40L26_PWRMGT_STS				0x290C
#define CS40L26_REFCLK_INPUT				0x2C04
#define CS40L26_GLOBAL_SAMPLE_RATE			0x2C0C
#define CS40L26_PLL_REFCLK_DETECT_0			0x2C28
#define CS40L26_VBST_CTL_1				0x3800
#define CS40L26_VBST_CTL_2				0x3804
#define CS40L26_BST_IPK_CTL				0x3808
#define CS40L26_BST_DCM_CTL				0x381C
#define CS40L26_TEST_LBST				0x391C
#define CS40L26_MONITOR_FILT				0x4008
#define CS40L26_SPKMON_VMON_DEC_OUT_DATA		0x41B4
#define CS40L26_ENABLES_AND_CODES_DIG			0x4308
#define CS40L26_ASP_ENABLES1				0x4800
#define CS40L26_ASP_CONTROL2				0x4808
#define CS40L26_ASP_FRAME_CONTROL5			0x4820
#define CS40L26_ASP_DATA_CONTROL5			0x4840
#define CS40L26_DACPCM1_INPUT				0x4C00
#define CS40L26_ASPTX1_INPUT				0x4C20
#define CS40L26_DSP1RX1_INPUT				0x4C40
#define CS40L26_DSP1RX5_INPUT				0x4C50
#define CS40L26_NGATE1_INPUT				0x4C60
#define CS40L26_VPBR_CONFIG				0x6404
#define CS40L26_VBBR_CONFIG				0x6408
#define CS40L26_VPBR_STATUS				0x640C
#define CS40L26_VBBR_STATUS				0x6410
#define CS40L26_NG_CONFIG				0x6808
#define CS40L26_DIGPWM_CONFIG2				0x7068
#define CS40L26_TST_DAC_MSM_CONFIG			0x7404
#define CS40L26_IRQ1_CFG				0x10000
#define CS40L26_IRQ1_STATUS				0x10004
#define CS40L26_IRQ1_EINT_1				0x10010
#define CS40L26_IRQ1_EINT_2				0x10014
#define CS40L26_IRQ1_STS_1				0x10090
#define CS40L26_IRQ1_STS_2				0x10094
#define CS40L26_IRQ1_MASK_1				0x10110
#define CS40L26_IRQ1_MASK_2				0x10114
#define CS40L26_MIXER_NGATE_CH1_CFG			0x12004
#define CS40L26_DSP_MBOX_1				0x13000
#define CS40L26_DSP_MBOX_2				0x13004
#define CS40L26_DSP_MBOX_3				0x13008
#define CS40L26_DSP_MBOX_4				0x1300C
#define CS40L26_DSP_MBOX_5				0x13010
#define CS40L26_DSP_MBOX_6				0x13014
#define CS40L26_DSP_MBOX_7				0x13018
#define CS40L26_DSP_MBOX_8				0x1301C
#define CS40L26_DSP_VIRTUAL1_MBOX_1			0x13020
#define CS40L26_DSP1_XMEM_PACKED_0			0x2000000
#define CS40L26_DSP1_XMEM_PACKED_6143			0x2005FFC
#define CS40L26_DSP1_XROM_PACKED_0			0x2006000
#define CS40L26_DSP1_XROM_PACKED_4604			0x200A7F0
#define CS40L26_DSP1_XMEM_UNPACKED32_0			0x2400000
#define CS40L26_DSP1_XROM_UNPACKED32_3070		0x2406FF8
#define CS40L26_DSP1_XMEM_UNPACKED24_0			0x2800000
#define CS40L26_DSP1_XMEM_UNPACKED24_8191		0x2807FFC
#define CS40L26_DSP1_XROM_UNPACKED24_0			0x2808000
#define CS40L26_DSP1_XROM_UNPACKED24_6141		0x280DFF4
#define CS40L26_DSP1_CCM_CORE_CONTROL			0x2BC1000
#define CS40L26_DSP1_YMEM_PACKED_0			0x2C00000
#define CS40L26_DSP1_YMEM_PACKED_1532			0x2C017F0
#define CS40L26_DSP1_YMEM_UNPACKED32_0			0x3000000
#define CS40L26_DSP1_YMEM_UNPACKED32_1022		0x3000FF8
#define CS40L26_DSP1_YMEM_UNPACKED24_0			0x3400000
#define CS40L26_DSP1_YMEM_UNPACKED24_2045		0x3401FF4
#define CS40L26_DSP1_PMEM_0				0x3800000
#define CS40L26_DSP1_PMEM_5114				0x3804FE8
#define CS40L26_DSP1_PROM_0				0x3C60000
#define CS40L26_DSP1_PROM_30714				0x3C7DFE8

#define CS40L26_MAX_I2C_READ_SIZE_WORDS			32

/* Register default changes */
#define CS40L26_TST_DAC_MSM_CONFIG_DEFAULT_CHANGE_VALUE_FULL	0x11330000
#define CS40L26_TST_DAC_MSM_CONFIG_DEFAULT_CHANGE_VALUE_H16 (\
		CS40L26_TST_DAC_MSM_CONFIG_DEFAULT_CHANGE_VALUE_FULL >> 16)
#define CS40L26_SPK_DEFAULT_HIZ_MASK				BIT(28)
#define CS40L26_SPK_DEFAULT_HIZ_SHIFT				28

/* Device */
#define CS40L26_MAX_DEVICES		2
#define CS40L26_MAX_PROBE_RETRY		1
#define CS40L26_DEV_NAME		"CS40L26"
#define CS40L26_INPUT_DEV_NAME		"cs40l26_input"

#define CS40L26_DEVID_A			0x40A260
#define CS40L26_DEVID_B			0x40A26B
#define CS40L26_DEVID_L27_A		0x40A270
#define CS40L26_DEVID_L27_B		0x40A27B
#define CS40L26_DEVID_MASK		GENMASK(23, 0)
#define CS40L26_NUM_DEVS		4

#define CS40L26_REVID_A1		0xA1
#define CS40L26_REVID_B0		0xB0
#define CS40L26_REVID_B1		0xB1
#define CS40L26_REVID_B2		0xB2
#define CS40L26_REVID_MASK		GENMASK(7, 0)

#define CS40L26_ID_L26A_A1	((CS40L26_DEVID_A << 8) | CS40L26_REVID_A1)
#define CS40L26_ID_L27A_A1	((CS40L26_DEVID_L27_A << 8) | CS40L26_REVID_A1)
#define CS40L26_ID_L26A_B0	((CS40L26_DEVID_A << 8) | CS40L26_REVID_B0)
#define CS40L26_ID_L27A_B0	((CS40L26_DEVID_L27_A << 8) | CS40L26_REVID_B0)
#define CS40L26_ID_L27A_B1	((CS40L26_DEVID_L27_A << 8) | CS40L26_REVID_B1)
#define CS40L26_ID_L27A_B2	((CS40L26_DEVID_L27_A << 8) | CS40L26_REVID_B2)

#define CS40L26_GLOBAL_EN_MASK		BIT(0)

#define CS40L26_DSP_CCM_CORE_KILL	0x00000080
#define CS40L26_DSP_CCM_CORE_RESET	0x00000281

#define CS40L26_GLOBAL_FS_MASK		GENMASK(4, 0)
#define CS40L26_GLOBAL_FS_48K		0x03
#define CS40L26_GLOBAL_FS_96K		0x04

#define CS40L26_MEM_RDY_MASK		BIT(1)
#define CS40L26_MEM_RDY_SHIFT		1

#define CS40L26_DSP_HALO_STATE_RUN	2

#define CS40L26_NUM_PCT_MAP_VALUES	101

#define CS40L26_TEST_KEY_UNLOCK_CODE1	0x00000055
#define CS40L26_TEST_KEY_UNLOCK_CODE2	0x000000AA
#define CS40L26_TEST_KEY_LOCK_CODE	0x00000000

/* DSP State */
#define CS40L26_DSP_STATE_HIBERNATE	0
#define CS40L26_DSP_STATE_SHUTDOWN	1
#define CS40L26_DSP_STATE_STANDBY	2
#define CS40L26_DSP_STATE_ACTIVE	3

#define CS40L26_DSP_STATE_MASK		GENMASK(7, 0)

#define CS40L26_DSP_STATE_ATTEMPTS	5

#define CS40L26_DSP_LOCK3_OFFSET	8
#define CS40L26_DSP_LOCK3_MASK		BIT(1)

#define CS40L26_DSP_SHUTDOWN_MAX_ATTEMPTS	10

/* Algorithms */
#define CS40L26_A2H_ALGO_ID		0x00040110
#define CS40L26_BUZZGEN_ALGO_ID		0x0001F202
#define CS40L26_DYNAMIC_F0_ALGO_ID	0x0001F21B
#define CS40L26_EVENT_HANDLER_ALGO_ID	0x0001F200
#define CS40L26_F0_EST_ALGO_ID		0x0001F20C
#define CS40L26_GPIO_ALGO_ID		0x0001F201
#define CS40L26_MAILBOX_ALGO_ID		0x0001F203
#define CS40L26_MDSYNC_ALGO_ID		0x0001F20F
#define CS40L26_PM_ALGO_ID		0x0001F206
#define CS40L26_SVC_ALGO_ID		0x0001F207
#define CS40L26_VIBEGEN_ALGO_ID		0x000100BD
#define CS40L26_LOGGER_ALGO_ID		0x0004013D
#define CS40L26_EVENT_LOGGER_ALGO_ID	0x0004F222
#define CS40L26_EXT_ALGO_ID		0x0004013C
#define CS40L26_DVL_ALGO_ID		0x00040140
#define CS40L26_EP_ALGO_ID		0x00040141
#define CS40L26_LF0T_ALGO_ID		0x00040143

/* DebugFS */
#define CS40L26_ALGO_ID_MAX_STR_LEN	12
#define CS40L26_HW_STR_LEN		16

/* Power management */
#define CS40L26_PSEQ_MAX_WORDS			129
#define CS40L26_PSEQ_NUM_OPS			8
#define CS40L26_PSEQ_STR_LINE_LEN		64
#define CS40L26_PSEQ_OP_MASK			GENMASK(23, 16)
#define CS40L26_PSEQ_OP_SHIFT			16
#define CS40L26_PSEQ_OP_WRITE_FULL		0x00
#define CS40L26_PSEQ_OP_WRITE_FULL_WORDS	3
#define CS40L26_PSEQ_OP_WRITE_FIELD		0x01
#define CS40L26_PSEQ_OP_WRITE_FIELD_WORDS	4
#define CS40L26_PSEQ_OP_WRITE_ADDR8		0x02
#define CS40L26_PSEQ_OP_WRITE_ADDR8_WORDS	2
#define CS40L26_PSEQ_OP_WRITE_INCR		0x03
#define CS40L26_PSEQ_OP_WRITE_INCR_WORDS	2
#define CS40L26_PSEQ_OP_WRITE_L16		0x04
#define CS40L26_PSEQ_OP_WRITE_H16		0x05
#define CS40L26_PSEQ_OP_WRITE_X16_WORDS		2
#define CS40L26_PSEQ_OP_DELAY			0xFE
#define CS40L26_PSEQ_OP_DELAY_WORDS		1
#define CS40L26_PSEQ_OP_END			0xFF
#define CS40L26_PSEQ_OP_END_WORDS		1
#define CS40L26_PSEQ_OP_END_ADDR		0xFFFFFF
#define CS40L26_PSEQ_OP_END_DATA		0xFFFFFF
#define CS40L26_PSEQ_INVALID_ADDR		0xFF000000
#define CS40L26_PSEQ_WRITE_FULL_LOWER_ADDR_SHIFT	8
#define CS40L26_PSEQ_WRITE_FULL_UPPER_ADDR_SHIFT	16
#define CS40L26_PSEQ_WRITE_FULL_LOWER_ADDR_MASK		GENMASK(15, 0)
#define CS40L26_PSEQ_WRITE_FULL_UPPER_ADDR_MASK		GENMASK(31, 0)
#define CS40L26_PSEQ_WRITE_FULL_UPPER_DATA_SHIFT	24
#define CS40L26_PSEQ_WRITE_FULL_LOWER_DATA_MASK		GENMASK(23, 0)
#define CS40L26_PSEQ_WRITE_FULL_UPPER_DATA_MASK		GENMASK(31, 24)
#define CS40L26_PSEQ_WRITE_X16_LOWER_ADDR_SHIFT		16
#define CS40L26_PSEQ_WRITE_X16_LOWER_ADDR_MASK		GENMASK(7, 0)
#define CS40L26_PSEQ_WRITE_X16_UPPER_ADDR_SHIFT		8
#define CS40L26_PSEQ_WRITE_X16_UPPER_ADDR_MASK		GENMASK(23, 8)
#define CS40L26_PSEQ_WRITE_X16_UPPER_DATA_SHIFT		0
#define CS40L26_PSEQ_WRITE_X16_UPPER_DATA_MASK		GENMASK(31, 0)
#define CS40L26_PSEQ_WRITE_ADDR8_ADDR_SHIFT		8
#define CS40L26_PSEQ_WRITE_ADDR8_ADDR_MASK		GENMASK(7, 0)
#define CS40L26_PSEQ_WRITE_ADDR8_UPPER_DATA_SHIFT	24
#define CS40L26_PSEQ_WRITE_ADDR8_UPPER_DATA_MASK	GENMASK(31, 24)
#define CS40L26_PSEQ_WRITE_ADDR8_LOWER_DATA_MASK	GENMASK(23, 0)

#define CS40L26_PSEQ_FULL_ADDR_GET(x, y)	((((x) & 0xFFFF) << 16) |\
						(((y) & 0xFFFF00) >> 8))
#define CS40L26_PSEQ_FULL_DATA_GET(x, y)	((((x) & 0xFF) << 24) |\
						((y) & 0xFFFFFF))
#define CS40L26_PSEQ_X16_ADDR_GET(x, y)		((((x) & 0xFFFF) << 8) |\
						(((y) & 0xFF0000) >> 16))
#define CS40L26_PSEQ_L16_DATA_GET(x)		((x) & 0xFFFF)
#define CS40L26_PSEQ_H16_DATA_GET(x)		(CS40L26_PSEQ_L16_DATA_GET((x)) << 16)
#define CS40L26_PSEQ_ADDR8_ADDR_GET(x)		(((x) & 0xFF00) >> 8)
#define CS40L26_PSEQ_ADDR8_DATA_GET(x, y)	((((x) & 0xFF) << 24) |\
						((y) & 0xFFFFFF))

#define CS40L26_PM_STDBY_TIMEOUT_OFFSET		16
#define CS40L26_PM_STDBY_TIMEOUT_MS_MIN		100
#define CS40L26_PM_TIMEOUT_MS_MAX		10000
#define CS40L26_PM_ACTIVE_TIMEOUT_OFFSET	24
#define CS40L26_PM_ACTIVE_TIMEOUT_MS_DEFAULT	250
#define CS40L26_PM_ACTIVE_TIMEOUT_MS_MIN	0
#define CS40L26_PM_TIMEOUT_TICKS_LOWER_MASK	GENMASK(23, 0)
#define CS40L26_PM_TIMEOUT_TICKS_UPPER_MASK	GENMASK(7, 0)
#define CS40L26_PM_TIMEOUT_TICKS_UPPER_SHIFT	24
#define CS40L26_PM_TICKS_PER_SEC		32768

#define CS40L26_AUTOSUSPEND_DELAY_MS		2000

#define CS40L26_WKSRC_STS_MASK			GENMASK(9, 4)
#define CS40L26_WKSRC_STS_SHIFT			4

#define CS40L26_WKSRC_GPIO_POL_MASK		GENMASK(3, 0)

#define CS40L26_WKSRC_STS_EN			BIT(7)


#define CS40L26_NG_THRESHOLD_MASK		GENMASK(2, 0)
#define CS40L26_NG_DELAY_MASK			GENMASK(6, 4)
#define CS40L26_NG_ENABLE_MASK			GENMASK(13, 8)
#define CS40L26_NG_THRESHOLD_DEFAULT		3
#define CS40L26_NG_THRESHOLD_MIN		0
#define CS40L26_NG_THRESHOLD_MAX		7
#define CS40L26_NG_DELAY_DEFAULT		3
#define CS40L26_NG_DELAY_MIN			0
#define CS40L26_NG_DELAY_MAX			7

#define CS40L26_AUX_NG_THLD_MASK		GENMASK(2, 0)
#define CS40L26_AUX_NG_HOLD_MASK		GENMASK(11, 8)
#define CS40L26_AUX_NG_EN_MASK			BIT(16)
#define CS40L26_AUX_NG_THLD_DEFAULT		3
#define CS40L26_AUX_NG_THLD_MAX			7
#define CS40L26_AUX_NG_HOLD_DEFAULT		3
#define CS40L26_AUX_NG_HOLD_MAX			15

/* DSP mailbox controls */
#define CS40L26_DSP_TIMEOUT_US_MIN		1000
#define CS40L26_DSP_TIMEOUT_US_MAX		1100
#define CS40L26_DSP_TIMEOUT_COUNT		3

#define CS40L26_DSP_MBOX_CMD_HIBER		0x02000001
#define CS40L26_DSP_MBOX_CMD_WAKEUP		0x02000002
#define CS40L26_DSP_MBOX_CMD_PREVENT_HIBER	0x02000003
#define CS40L26_DSP_MBOX_CMD_ALLOW_HIBER	0x02000004
#define CS40L26_DSP_MBOX_CMD_SHUTDOWN		0x02000005
#define CS40L26_DSP_MBOX_PM_CMD_BASE		CS40L26_DSP_MBOX_CMD_HIBER

#define CS40L26_DSP_MBOX_CMD_START_I2S		0x03000002
#define CS40L26_DSP_MBOX_CMD_STOP_I2S		0x03000003
#define CS40L26_DSP_MBOX_CMD_LOGGER_MAX_RESET	0x03000004
#define CS40L26_DSP_MBOX_CMD_A2H_REINIT		0x03000007
#define CS40L26_DSP_MBOX_CMD_OWT_PUSH		0x03000008
#define CS40L26_DSP_MBOX_CMD_OWT_RESET		0x03000009

#define CS40L26_DSP_MBOX_CMD_LE_EST		0x07000004

#define CS40L26_DSP_MBOX_CMD_OWT_DELETE_BASE	0x0D000000
#define CS40L26_DSP_MBOX_CMD_HE_TIME_BASE	0x0E000000

#define CS40L26_DSP_MBOX_CMD_INDEX_MASK		GENMASK(28, 24)
#define CS40L26_DSP_MBOX_CMD_INDEX_SHIFT	24

#define CS40L26_DSP_MBOX_CMD_PAYLOAD_MASK	GENMASK(23, 0)

#define CS40L26_DSP_MBOX_CMD_INDEX_CALIBRATION_CONTROL 0x7

#define CS40L26_DSP_MBOX_BUFFER_NUM_REGS		4

#define CS40L26_DSP_MBOX_COMPLETE_MBOX		0x01000000
#define CS40L26_DSP_MBOX_COMPLETE_GPIO		0x01000001
#define CS40L26_DSP_MBOX_COMPLETE_I2S		0x01000002
#define CS40L26_DSP_MBOX_TRIGGER_CP		0x01000010
#define CS40L26_DSP_MBOX_TRIGGER_GPIO		0x01000011
#define CS40L26_DSP_MBOX_TRIGGER_I2S		0x01000012
#define CS40L26_DSP_MBOX_INIT			0x02000000
#define CS40L26_DSP_MBOX_PM_AWAKE		0x02000002
#define CS40L26_DSP_MBOX_F0_EST_START		0x07000011
#define CS40L26_DSP_MBOX_F0_EST_DONE		0x07000021
#define CS40L26_DSP_MBOX_REDC_EST_START		0x07000012
#define CS40L26_DSP_MBOX_REDC_EST_DONE		0x07000022
#define CS40L26_DSP_MBOX_LE_EST_START		0x07000014
#define CS40L26_DSP_MBOX_LE_EST_DONE		0x07000024
#define CS40L26_DSP_MBOX_PEQ_CALCULATION_START	0x07000018
#define CS40L26_DSP_MBOX_PEQ_CALCULATION_DONE	0x07000028
#define CS40L26_DSP_MBOX_LS_CALIBRATION_START	0x07000010
#define CS40L26_DSP_MBOX_LS_CALIBRATION_DONE	0x07000030
#define CS40L26_DSP_MBOX_LS_CALIBRATION_ERROR	0x07000040

#define CS40L26_DSP_MBOX_SYS_ACK		0x0A000000
#define CS40L26_DSP_MBOX_PANIC			0x0C000000
#define CS40L26_DSP_MBOX_WATERMARK		0x0D000000

#define CS40L26_DSP_MBOX_HE_PAYLOAD_MAX_MS	GENMASK(22, 0)
#define CS40L26_DSP_MBOX_HE_PAYLOAD_OVERFLOW	BIT(23)

/* Firmware Mode */
#define CS40L26_FW_FILE_NAME		"cs40l26.wmfw"
#define CS40L26_FW_CALIB_NAME		"cs40l26-calib.wmfw"

#define CS40L26_MAX_TUNING_FILES	7

#define CS40L26_WT_FILE_NAME		"cs40l26.bin"
#define CS40L26_WT_FILE_PREFIX		"cs40l26-wt"
#define CS40L26_WT_FILE_PREFIX_LEN	11
#define CS40L26_SVC_FILE_PREFIX		"cs40l26-svc"
#define CS40L26_SVC_FILE_PREFIX_LEN	12
#define CS40L26_SVC_FILE_NAME		"cs40l26-svc.bin"
#define CS40L26_A2H_FILE_NAME		"cs40l26-a2h.bin"
#define CS40L26_FILE_NAME_MAX_LEN	20
#define CS40L26_FILE_SUFFIX		".bin"
#define CS40L26_FILE_SUFFIX_LEN		4
#define CS40L26_DVL_FILE_NAME		"cs40l26-dvl.bin"
#define CS40L26_CALIB_FILE_NAME		"cs40l26-calib.bin"
#define CS40L26_EP_FILE_NAME		"cs40l26-ep.bin"
#define CS40L26_LF0T_FILE_NAME		"cs40l26-lf0t.bin"
#define CS40L26_DBC_FILE_NAME		"cs40l26-dbc.bin"

#define CS40L26_SVC_LE_EST_TIME_US	8000
#define CS40L26_SVC_LE_MAX_ATTEMPTS	2
#define CS40L26_SVC_DT_PREFIX		"svc-le"

#define CS40L26_FW_ID				0x1800D4
#define CS40L26_FW_MIN_REV			0x07022B
#define CS40L26_FW_BRANCH			0x07
#define CS40L26_FW_CALIB_ID			0x1800DA
#define CS40L26_FW_CALIB_MIN_REV		0x010123
#define CS40L26_FW_CALIB_BRANCH			0x01
#define CS40L26_FW_MAINT_MIN_REV		0x270216
#define CS40L26_FW_MAINT_BRANCH			0x27
#define CS40L26_FW_MAINT_CALIB_MIN_REV		0x21010D
#define CS40L26_FW_MAINT_CALIB_BRANCH		0x21
#define CS40L26_FW_B2_MIN_REV			0x090000
#define CS40L26_FW_B2_BRANCH			0x09
#define CS40L26_FW_GPI_TIMEOUT_MIN_REV		0x07022A
#define CS40L26_FW_GPI_TIMEOUT_CALIB_MIN_REV	0x010122
#define CS40L26_FW_BRANCH_MASK			GENMASK(23, 21)

#define CS40L26_CCM_CORE_RESET		0x00000200
#define CS40L26_CCM_CORE_ENABLE		0x00000281

#define CS40L26_COEFF_NAME_MAX_LEN	64

/* Wavetable */
#define CS40L26_WT_NAME_XM	"WAVE_XM_TABLE"
#define CS40L26_WT_NAME_YM	"WAVE_YM_TABLE"

/* Power supplies */
#define CS40L26_VP_SUPPLY		0
#define CS40L26_VA_SUPPLY		1
#define CS40L26_NUM_SUPPLIES		2
#define CS40L26_VP_SUPPLY_NAME		"VP"
#define CS40L26_VA_SUPPLY_NAME		"VA"

#define CS40L26_MIN_RESET_PULSE_WIDTH		1500
#define CS40L26_CONTROL_PORT_READY_DELAY	6000

/* Haptic triggering */
#define CS40L26_STOP_PLAYBACK			0x05000000

#define CS40L26_MAX_INDEX_MASK			0x0000FFFF

#define CS40L26_CUSTOM_DATA_SIZE		2

#define CS40L26_RAM_INDEX_START			0x01000000
#define CS40L26_RAM_INDEX_END			0x0100007F

#define CS40L26_ROM_INDEX_START			0x01800000
#define CS40L26_ROM_INDEX_END			0x01800026

#define CS40L26_OWT_INDEX_START			0x01400000
#define CS40L26_OWT_INDEX_END			0x01400010

#define CS40L26_RAM_BANK_ID			0
#define CS40L26_ROM_BANK_ID			1
#define CS40L26_OWT_BANK_ID			2
#define CS40L26_BUZ_BANK_ID			3

#define CS40L26_BUZZGEN_NUM_CONFIGS	(CS40L26_BUZZGEN_INDEX_END - CS40L26_BUZZGEN_INDEX_START)

#define CS40L26_BUZZGEN_INDEX_START		0x01800080
#define CS40L26_BUZZGEN_INDEX_END		0x01800085

#define CS40L26_BUZZGEN_PER_MAX			10 /* ms */
#define CS40L26_BUZZGEN_PER_MIN			4

#define CS40L26_BUZZGEN_LEVEL_MIN               0x00
#define CS40L26_BUZZGEN_LEVEL_MAX               0xFF

#define CS40L26_AMP_CTRL_VOL_PCM_MASK		GENMASK(13, 3)
#define CS40L26_AMP_CTRL_VOL_PCM_SHIFT		3

#define CS40L26_AMP_VOL_PCM_MAX			0x07FF

#define CS40L26_ERASE_BUFFER_MS			500
#define CS40L26_MAX_WAIT_VIBE_COMPLETE_MS	10000

/* GPI Triggering */
#define CS40L26_GPIO1			1
#define CS40L26_EVENT_MAP_INDEX_MASK	GENMASK(8, 0)
#define CS40L26_EVENT_MAP_NUM_GPI_REGS	4
#define CS40L26_EVENT_MAP_GPI_DISABLE	0x1FF

#define CS40L26_BTN_INDEX_MASK	GENMASK(7, 0)
#define CS40L26_BTN_BUZZ_MASK	BIT(7)
#define CS40L26_BTN_BUZZ_SHIFT	7
#define CS40L26_BTN_BANK_MASK	BIT(8)
#define CS40L26_BTN_BANK_SHIFT	8
#define CS40L26_BTN_NUM_MASK	GENMASK(14, 12)
#define CS40L26_BTN_NUM_SHIFT	12
#define CS40L26_BTN_EDGE_MASK	BIT(15)
#define CS40L26_BTN_EDGE_SHIFT	15
#define CS40L26_BTN_OWT_MASK	BIT(16)
#define CS40L26_BTN_OWT_SHIFT	16

/* Interrupts */
#define CS40L26_IRQ_STATUS_MASK		BIT(0)
#define CS40L26_GPIO1_RISE_MASK		BIT(0)
#define CS40L26_GPIO1_FALL_MASK		BIT(1)
#define CS40L26_GPIO2_RISE_MASK		BIT(2)
#define CS40L26_GPIO2_FALL_MASK		BIT(3)
#define CS40L26_GPIO3_RISE_MASK		BIT(4)
#define CS40L26_GPIO3_FALL_MASK		BIT(5)
#define CS40L26_GPIO4_RISE_MASK		BIT(6)
#define CS40L26_GPIO4_FALL_MASK		BIT(7)
#define CS40L26_WKSRC_STS_ANY_MASK	BIT(8)
#define CS40L26_WKSRC_STS_GPIO1_MASK	BIT(9)
#define CS40L26_WKSRC_STS_GPIO2_MASK	BIT(10)
#define CS40L26_WKSRC_STS_GPIO3_MASK	BIT(11)
#define CS40L26_WKSRC_STS_GPIO4_MASK	BIT(12)
#define CS40L26_WKSRC_STS_SPI_MASK	BIT(13)
#define CS40L26_WKSRC_STS_I2C_MASK	BIT(14)
#define CS40L26_BST_OVP_ERR_MASK	BIT(20)
#define CS40L26_BST_DCM_UVP_ERR_MASK	BIT(21)
#define CS40L26_BST_SHORT_ERR_MASK	BIT(22)
#define CS40L26_BST_IPK_FLAG_MASK	BIT(23)
#define CS40L26_TEMP_ERR_MASK		BIT(26)
#define CS40L26_AMP_ERR_MASK		BIT(27)
#define CS40L26_VIRTUAL2_MBOX_WR_MASK	BIT(31)
#define CS40L26_VPBR_FLAG_MASK		BIT(17)
#define CS40L26_VPBR_ATT_CLR_MASK	BIT(18)
#define CS40L26_VBBR_FLAG_MASK		BIT(19)
#define CS40L26_VBBR_ATT_CLR_MASK	BIT(20)
#define CS40L26_IRQ(_irq, _name, _hand)	\
	{				\
		.irq = CS40L26_ ## _irq ## _IRQ,		\
		.name = _name,		\
		.handler = _hand,	\
	}
#define CS40L26_REG_IRQ(_reg, _irq)					\
	[CS40L26_ ## _irq ## _IRQ] = {					\
		.reg_offset = (CS40L26_ ## _reg) - CS40L26_IRQ1_EINT_1,	\
		.mask = CS40L26_ ## _irq ## _MASK			\
	}


/* temp monitoring */
#define CS40L26_TEMP_RESULT_FILT_MASK		GENMASK(24, 16)
#define CS40L26_TEMP_RESULT_FILT_SHIFT		16

/* BST */
#define CS40L26_BST_DCM_EN_DEFAULT		1
#define CS40L26_BST_DCM_EN_MASK			BIT(0)
#define CS40L26_BST_DCM_EN_SHIFT		0

#define CS40L26_BST_IPK_UA_MAX			4800000
#define CS40L26_BST_IPK_UA_MIN			800000
#define CS40L26_BST_IPK_UA_STEP			50000
#define CS40L26_BST_IPK_UA_DEFAULT		4500000
#define CS40L26_BST_IPK_DEFAULT			0x4A

#define CS40L26_BST_UV_MIN			2500000
#define CS40L26_BST_UV_MAX			11000000
#define CS40L26_BST_UV_STEP			50000
#define CS40L26_BST_CTL_DEFAULT			0xAA
#define CS40L26_BST_CTL_VP			0x00
#define CS40L26_BST_CTL_MASK			GENMASK(7, 0)
#define CS40L26_BST_CTL_SEL_MASK		GENMASK(1, 0)
#define CS40L26_BST_CTL_SEL_FIXED		0x0

#define CS40L26_CLIP_LVL_UV_MAX			11000000
#define CS40L26_CLIP_LVL_UV_MIN			250000
#define CS40L26_CLIP_LVL_UV_STEP		250000
#define CS40L26_CLIP_LVL_DEFAULT		0x2C
#define CS40L26_CLIP_LVL_MASK			GENMASK(17, 12)
#define CS40L26_CLIP_LVL_SHIFT			12

#define CS40L26_BST_TIME_MIN_US		10000
#define CS40L26_BST_TIME_MAX_US		10100

#define CS40L26_BST_CTL_LIM_EN_MASK		BIT(2)
#define CS40L26_BST_CTL_LIM_EN_SHIFT		2

#define CS40L26_OVERPROTECTION_GAIN_MIN		BIT(20)
#define CS40L26_OVERPROTECTION_GAIN_MAX		0x7FFFFF

#define CS40L26_BOOST_DISABLE_DELAY_MAX		8388608

/* Brownout prevention */
#define CS40L26_VXBR_STATUS_DIV_STEP		625
#define CS40L26_VXBR_STATUS_MASK		GENMASK(7, 0)
#define CS40L26_VXBR_DEFAULT_MASK		GENMASK(31, 24)

#define CS40L26_VBBR_EN_MASK			BIT(13)
#define CS40L26_VBBR_EN_SHIFT			13

#define CS40L26_VPBR_EN_MASK			BIT(12)
#define CS40L26_VPBR_EN_SHIFT			12
#define CS40L26_VPBR_THLD_MASK			GENMASK(4, 0)

#define CS40L26_VPBR_THLD_MIN			0x02
#define CS40L26_VPBR_THLD_MAX			0x1F
#define CS40L26_VPBR_THLD_UV_DIV		47000
#define CS40L26_VPBR_THLD_UV_MIN		2497000
#define CS40L26_VPBR_THLD_UV_MAX		3874000
#define CS40L26_VPBR_THLD_UV_DEFAULT		2639000

#define CS40L26_VBBR_THLD_MASK			GENMASK(5, 0)
#define CS40L26_VBBR_THLD_MIN			0x02
#define CS40L26_VBBR_THLD_MAX			0x3F
#define CS40L26_VBBR_THLD_UV_DIV		55000
#define CS40L26_VBBR_THLD_UV_MIN		109000
#define CS40L26_VBBR_THLD_UV_MAX		3445000
#define CS40L26_VBBR_THLD_UV_DEFAULT		273000

#define CS40L26_VXBR_MAX_ATT_MASK		GENMASK(11, 8)
#define CS40L26_VXBR_MAX_ATT_SHIFT		8
#define CS40L26_VXBR_MAX_ATT_MAX		15
#define CS40L26_VXBR_MAX_ATT_MIN		0
#define CS40L26_VXBR_MAX_ATT_DEFAULT		9

#define CS40L26_VXBR_ATK_STEP_MASK		GENMASK(15, 12)
#define CS40L26_VXBR_ATK_STEP_SHIFT		12
#define CS40L26_VXBR_ATK_STEP_MIN		0
#define CS40L26_VXBR_ATK_STEP_MAX		7
#define CS40L26_VXBR_ATK_STEP_DEFAULT		1

#define CS40L26_VXBR_ATK_RATE_MASK		GENMASK(18, 16)
#define CS40L26_VXBR_ATK_RATE_SHIFT		16
#define CS40L26_VXBR_ATK_RATE_MIN		0
#define CS40L26_VXBR_ATK_RATE_MAX		7
#define CS40L26_VXBR_ATK_RATE_DEFAULT		2

#define CS40L26_VXBR_WAIT_MASK			GENMASK(20, 19)
#define CS40L26_VXBR_WAIT_SHIFT			19
#define CS40L26_VXBR_WAIT_MAX			3
#define CS40L26_VXBR_WAIT_MIN			0
#define CS40L26_VXBR_WAIT_DEFAULT		1

#define CS40L26_VXBR_REL_RATE_MASK		GENMASK(23, 21)
#define CS40L26_VXBR_REL_RATE_SHIFT		21
#define CS40L26_VXBR_REL_RATE_MAX		7
#define CS40L26_VXBR_REL_RATE_MIN		0
#define CS40L26_VXBR_REL_RATE_DEFAULT		5

/* Audio */
#define CS40L26_PLL_CLK_CFG_32768		0x00
#define CS40L26_PLL_CLK_CFG_1536000		0x1B
#define CS40L26_PLL_CLK_CFG_3072000		0x21
#define CS40L26_PLL_CLK_CFG_6144000		0x28
#define CS40L26_PLL_CLK_CFG_9600000		0x30
#define CS40L26_PLL_CLK_CFG_12288000		0x33

#define CS40L26_PLL_CLK_FRQ_32768		32768
#define CS40L26_PLL_CLK_FRQ_1536000		1536000
#define CS40L26_PLL_CLK_FRQ_3072000		3072000
#define CS40L26_PLL_CLK_FRQ_6144000		6144000
#define CS40L26_PLL_CLK_FRQ_9600000		9600000
#define CS40L26_PLL_CLK_FRQ_12288000		12288000

#define CS40L26_PLL_CLK_SEL_BCLK		0x0
#define CS40L26_PLL_CLK_SEL_FSYNC		0x1
#define CS40L26_PLL_CLK_SEL_MCLK		0x5

#define CS40L26_PLL_CLK_FREQ_MASK		GENMASK(31, 0)
#define CS40L26_PLL_CLK_CFG_MASK		GENMASK(5, 0)

#define CS40L26_FORMATS	(SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S24_LE)
#define CS40L26_RATES	(SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_96000)

#define CS40L26_ASP_RX_WIDTH_MASK		GENMASK(31, 24)
#define CS40L26_ASP_RX_WIDTH_SHIFT		24
#define CS40L26_ASP_FMT_MASK			GENMASK(10, 8)
#define CS40L26_ASP_FMT_SHIFT			8
#define CS40L26_ASP_BCLK_INV_MASK		BIT(6)
#define CS40L26_ASP_BCLK_INV_SHIFT		6
#define CS40L26_ASP_FSYNC_INV_MASK		BIT(2)
#define CS40L26_ASP_FSYNC_INV_SHIFT		2

#define CS40L26_ASP_FMT_TDM1_DSPA		0x0
#define CS40L26_ASP_FMT_I2S			0x2
#define CS40L26_ASP_FMT_TDM1P5			0x4

#define CS40L26_ASP_START_TIMEOUT		50 /* milliseconds */

#define CS40L26_PLL_REFCLK_BCLK			0x0
#define CS40L26_PLL_REFCLK_FSYNC		0x1
#define CS40L26_PLL_REFCLK_MCLK			0x5

#define CS40L26_PLL_REFCLK_SEL_MASK		GENMASK(2, 0)
#define CS40L26_PLL_REFCLK_EN_MASK		BIT(4)
#define CS40L26_PLL_REFCLK_EN_SHIFT		4
#define CS40L26_PLL_REFCLK_FREQ_MASK		GENMASK(10, 5)
#define CS40L26_PLL_REFCLK_FREQ_SHIFT		5
#define CS40L26_PLL_REFCLK_LOOP_MASK		BIT(11)
#define CS40L26_PLL_REFCLK_LOOP_SHIFT		11
#define CS40L26_PLL_REFCLK_SET_OPEN_LOOP	1
#define CS40L26_PLL_REFCLK_SET_CLOSED_LOOP	0
#define CS40L26_PLL_REFCLK_SET_ATTEMPTS		5
#define CS40L26_PLL_REFCLK_FORCE_EN_MASK	BIT(16)
#define CS40L26_PLL_REFCLK_FORCE_EN_SHIFT	16

#define CS40L26_ASP_RX_WL_MASK			GENMASK(5, 0)

#define CS40L26_DATA_SRC_ASPRX1			0x08
#define CS40L26_DATA_SRC_ASPRX2			0x09
#define CS40L26_DATA_SRC_VMON			0x18
#define CS40L26_DATA_SRC_DSP1TX1		0x32
#define CS40L26_DATA_SRC_DSP1TX2		0x33
#define CS40L26_DATA_SRC_DSP1TX4		0x35

#define CS40L26_DATA_SRC_MASK			GENMASK(6, 0)

#define CS40L26_ASP_TX1_EN_MASK		BIT(0)
#define CS40L26_ASP_TX2_EN_MASK		BIT(1)
#define CS40L26_ASP_TX2_EN_SHIFT	1
#define CS40L26_ASP_TX3_EN_MASK		BIT(2)
#define CS40L26_ASP_TX3_EN_SHIFT	2
#define CS40L26_ASP_TX4_EN_MASK		BIT(3)
#define CS40L26_ASP_TX4_EN_SHIFT	3
#define CS40L26_ASP_RX1_EN_MASK		BIT(16)
#define CS40L26_ASP_RX1_EN_SHIFT	16
#define CS40L26_ASP_RX2_EN_MASK		BIT(17)
#define CS40L26_ASP_RX2_EN_SHIFT	17
#define CS40L26_ASP_RX3_EN_MASK		BIT(18)
#define CS40L26_ASP_RX3_EN_SHIFT	18

#define CS40L26_ASP_RX1_SLOT_MASK	GENMASK(5, 0)
#define CS40L26_ASP_RX2_SLOT_MASK	GENMASK(13, 8)
#define CS40L26_ASP_RX2_SLOT_SHIFT	8

#define CS40L26_A2H_MAX_TUNINGS		5

#define CS40L26_A2H_LEVEL_MAX		0x7FFFFF
#define CS40L26_A2H_LEVEL_MIN		0x000001

#define CS40L26_I2S_ATTENUATION_MAX	0x1FFFFF

#define CS40L26_A2H_DELAY_MAX		0x190

#define CS40L26_VMON_DEC_OUT_DATA_MASK	GENMASK(23, 0)
#define CS40L26_VMON_OVFL_FLAG_MASK	BIT(31)
#define CS40L26_VMON_DEC_OUT_DATA_MAX	CS40L26_VMON_DEC_OUT_DATA_MASK

#define CS40L26_GAIN_FULL_SCALE		100

#define CS40L26_VIMON_DUAL_RATE_MASK	BIT(16)

/* OWT */
#define CS40L26_WT_HEADER_OFFSET		3
#define CS40L26_WT_METADATA_OFFSET		3
#define CS40L26_WT_HEADER_DEFAULT_FLAGS		0x0000
#define CS40L26_WT_HEADER_PWLE_SIZE		12
#define CS40L26_WT_HEADER_COMP_SIZE		20
#define CS40L26_WT_SVC_METADATA			BIT(10)
#define CS40L26_WT_TYPE12_IDENTIFIER		0xC00

#define CS40L26_WT_TYPE10_SECTION_BYTES_MIN	8
#define CS40L26_WT_TYPE10_SECTION_BYTES_MAX	12
#define CS40L26_WT_TYPE10_WAVELEN_MAX		0x3FFFFF
#define CS40L26_WT_TYPE10_WAVELEN_INDEF		0x400000
#define CS40L26_WT_TYPE10_WAVELEN_CALCULATED	0x800000
#define CS40L26_WT_TYPE10_COMP_DURATION_FLAG	0x80
#define CS40L26_WT_TYPE10_COMP_ROM_FLAG			0x40
#define CS40L26_WT_TYPE10_COMP_BUFFER		0x0000

/* F0 Offset represented as Q10.14 format */
#define CS40L26_F0_OFFSET_MAX		0x190000 /* +100 Hz */
#define CS40L26_F0_OFFSET_MIN		0xE70000 /* -100 Hz */

/* Calibration */
#define CS40L26_F0_EST_MIN			0xC8000
#define CS40L26_F0_EST_MAX			0x7FC000
#define CS40L26_Q_EST_MIN			0
#define CS40L26_Q_EST_MAX			0x7FFFFF

#define CS40L26_DVL_PEQ_COEFFICIENTS_NUM_REGS	6

#define CS40L26_F0_EST_FREQ_FRAC_BITS		14

#define CS40L26_SVC_INITIALIZATION_PERIOD_MS		6
#define CS40L26_REDC_CALIBRATION_BUFFER_MS		10
#define CS40L26_F0_AND_Q_CALIBRATION_MIN_MS		100
#define CS40L26_F0_AND_Q_CALIBRATION_MAX_MS		1800
#define CS40L26_F0_CHIRP_DURATION_FACTOR		3750
#define CS40L26_F0_FREQ_SPAN_HZ_MAX			120
#define CS40L26_F0_FREQ_SPAN_HZ_MIN			20
#define CS40L26_F0_FREQ_SPAN_MAX (CS40L26_F0_FREQ_SPAN_HZ_MAX << CS40L26_F0_EST_FREQ_FRAC_BITS)
#define CS40L26_F0_FREQ_SPAN_MIN (CS40L26_F0_FREQ_SPAN_HZ_MIN << CS40L26_F0_EST_FREQ_FRAC_BITS)
#define CS40L26_F0_FREQ_CENTRE_HZ_MAX			511
#define CS40L26_F0_FREQ_CENTRE_HZ_MIN			50
#define CS40L26_F0_FREQ_CENTRE_MAX (CS40L26_F0_FREQ_CENTRE_HZ_MAX << CS40L26_F0_EST_FREQ_FRAC_BITS)
#define CS40L26_F0_FREQ_CENTRE_MIN (CS40L26_F0_FREQ_CENTRE_HZ_MIN << CS40L26_F0_EST_FREQ_FRAC_BITS)

#define CS40L26_LOGGER_EN_MASK			BIT(0)

#define CS40L26_LOGGER_SRC_ID_BEMF		1
#define CS40L26_LOGGER_SRC_ID_VBST		2
#define CS40L26_LOGGER_SRC_ID_VMON		3
#define CS40L26_LOGGER_SRC_ID_EP		4

#define CS40L26_LOGGER_SRC_TYPE_XM_TO_XM	1

#define CS40L26_LOGGER_SRC_FF_OUT		2
#define CS40L26_LOGGER_SRC_PROTECTION_OUT	3

#define CS40L26_LOGGER_SRC_SIGN_MASK		BIT(23)
#define CS40L26_LOGGER_SRC_SIZE_MASK		BIT(22)
#define CS40L26_LOGGER_SRC_TYPE_MASK		GENMASK(21, 20)
#define CS40L26_LOGGER_SRC_ID_MASK		GENMASK(19, 16)
#define CS40L26_LOGGER_SRC_ADDR_MASK		GENMASK(15, 0)

#define CS40L26_LOGGER_DATA_MAX_STEP		12
#define CS40L26_LOGGER_DATA_MAX_OFFSET		4

#define CS40L26_UINT_24_BITS_MAX		16777215

#define CS40L26_CALIBRATION_TIMEOUT_MS	5000

/* Compensation */
#define CS40L26_COMP_EN_REDC_SHIFT  1
#define CS40L26_COMP_EN_F0_SHIFT    0

/* FW EXT */
#define CS40L26_SVC_EN_MASK	BIT(0)
#define CS40L26_FW_RAM_EXT_RELOC_REV 0x080102 /* FW rev relocating FW_RAM_EXT FW controls */

/* DBC */
#define CS40L26_DBC_ENABLE_MASK			BIT(1)
#define CS40L26_DBC_ENABLE_SHIFT		1
#define CS40L26_DBC_CONTROLS_MAX		0x7FFFFF
#define CS40L26_DBC_ENV_REL_COEF_MIN		8384414
#define CS40L26_DBC_ENV_REL_COEF_NAME		"DBC_ENV_REL_COEF"
#define CS40L26_DBC_RISE_HEADROOM_MIN		1432204
#define CS40L26_DBC_RISE_HEADROOM_NAME		"DBC_RISE_HEADROOM"
#define CS40L26_DBC_FALL_HEADROOM_MIN		750193
#define CS40L26_DBC_FALL_HEADROOM_NAME		"DBC_FALL_HEADROOM"
#define CS40L26_DBC_TX_LVL_THRESH_FS_MIN	839
#define CS40L26_DBC_TX_LVL_THRESH_FS_NAME	"DBC_TX_LVL_THRESH_FS"
#define CS40L26_DBC_TX_LVL_HOLD_OFF_MS_MAX	1000
#define CS40L26_DBC_TX_LVL_HOLD_OFF_MS_MIN	10
#define CS40L26_DBC_TX_LVL_HOLD_OFF_MS_NAME	"DBC_TX_LVL_HOLD_OFF_MS"
#define CS40L26_DBC_DEFAULT			0xFFFFFFFF

/* Errata */
#define CS40L26_ERRATA_A1_NUM_WRITES		5
#define CS40L26_ERRATA_A1_EXPL_EN_NUM_WRITES	1
#define CS40L26_PLL_REFCLK_DET_EN		0x00000001
#define CS40L26_DISABLE_EXPL_MODE		0x0100C080

/* MFD */
#define CS40L26_NUM_MFD_DEVS			1

/* macros */
#define CS40L26_MS_TO_US(n)	((n) * 1000)

/* enums */
enum cs40l26_brwnout_type {
	CS40L26_VBBR_THLD,
	CS40L26_VPBR_THLD,
	CS40L26_VXBR_MAX_ATT,
	CS40L26_VXBR_ATK_STEP,
	CS40L26_VXBR_ATK_RATE,
	CS40L26_VXBR_WAIT,
	CS40L26_VXBR_REL_RATE,
	CS40L26_NUM_BRWNOUT_TYPES,
};

enum cs40l26_gpio_map {
	CS40L26_GPIO_MAP_A_PRESS,
	CS40L26_GPIO_MAP_A_RELEASE,
	CS40L26_GPIO_MAP_NUM_AVAILABLE,
	CS40L26_GPIO_MAP_INVALID,
};

enum cs40l26_dbc_type {
	CS40L26_DBC_ENV_REL_COEF,
	CS40L26_DBC_RISE_HEADROOM,
	CS40L26_DBC_FALL_HEADROOM,
	CS40L26_DBC_TX_LVL_THRESH_FS,
	CS40L26_DBC_TX_LVL_HOLD_OFF_MS,
	CS40L26_DBC_NUM_CONTROLS,
};

enum cs40l26_vibe_state {
	CS40L26_VIBE_STATE_STOPPED,
	CS40L26_VIBE_STATE_HAPTIC,
	CS40L26_VIBE_STATE_ASP,
};

enum cs40l26_vibe_state_event {
	CS40L26_VIBE_STATE_EVENT_MBOX_PLAYBACK,
	CS40L26_VIBE_STATE_EVENT_MBOX_COMPLETE,
	CS40L26_VIBE_STATE_EVENT_GPIO_TRIGGER,
	CS40L26_VIBE_STATE_EVENT_GPIO_COMPLETE,
	CS40L26_VIBE_STATE_EVENT_ASP_START,
	CS40L26_VIBE_STATE_EVENT_ASP_STOP,
};

enum cs40l26_err_rls {
	CS40L26_RSRVD_ERR_RLS,/* 0 */
	CS40L26_AMP_SHORT_ERR_RLS,/* 1 */
	CS40L26_BST_SHORT_ERR_RLS,/* 2 */
	CS40L26_BST_OVP_ERR_RLS,/* 3 */
	CS40L26_BST_UVP_ERR_RLS,/* 4 */
	CS40L26_TEMP_WARN_ERR_RLS,/* 5 */
	CS40L26_TEMP_ERR_RLS,/* 6 */
};

enum cs40l26_pm_state {
	CS40L26_PM_STATE_HIBERNATE,
	CS40L26_PM_STATE_WAKEUP,
	CS40L26_PM_STATE_PREVENT_HIBERNATE,
	CS40L26_PM_STATE_ALLOW_HIBERNATE,
	CS40L26_PM_STATE_SHUTDOWN,
};

enum cs40l26_calibration_control_request {
	CS40L26_CALIBRATION_CONTROL_REQUEST_F0_AND_Q = 0x1,
	CS40L26_CALIBRATION_CONTROL_REQUEST_REDC = 0x2,
	CS40L26_CALIBRATION_CONTROL_REQUEST_DVL_PEQ = 0x8,
	CS40L26_CALIBRATION_CONTROL_REQUEST_LS_CALIBRATION = 0x10,
};

enum cs40l26_irq_list {
	CS40L26_GPIO1_RISE_IRQ,
	CS40L26_GPIO1_FALL_IRQ,
	CS40L26_GPIO2_RISE_IRQ,
	CS40L26_GPIO2_FALL_IRQ,
	CS40L26_GPIO3_RISE_IRQ,
	CS40L26_GPIO3_FALL_IRQ,
	CS40L26_GPIO4_RISE_IRQ,
	CS40L26_GPIO4_FALL_IRQ,
	CS40L26_WKSRC_STS_ANY_IRQ,
	CS40L26_WKSRC_STS_GPIO1_IRQ,
	CS40L26_WKSRC_STS_GPIO2_IRQ,
	CS40L26_WKSRC_STS_GPIO3_IRQ,
	CS40L26_WKSRC_STS_GPIO4_IRQ,
	CS40L26_WKSRC_STS_I2C_IRQ,
	CS40L26_BST_OVP_ERR_IRQ,
	CS40L26_BST_DCM_UVP_ERR_IRQ,
	CS40L26_BST_SHORT_ERR_IRQ,
	CS40L26_BST_IPK_FLAG_IRQ,
	CS40L26_TEMP_ERR_IRQ,
	CS40L26_AMP_ERR_IRQ,
	CS40L26_VIRTUAL2_MBOX_WR_IRQ,
	CS40L26_VPBR_FLAG_IRQ,
	CS40L26_VPBR_ATT_CLR_IRQ,
	CS40L26_VBBR_FLAG_IRQ,
	CS40L26_VBBR_ATT_CLR_IRQ
};

/* structs */
struct cs40l26_log_src {
	u8 sign;
	u8 size;
	u8 type;
	u8 id;
	u16 addr;
};

struct cs40l26_ls_cal_param {
	const char *calib_name;
	const char *runtime_name;
	int word_num;
};

struct cs40l26_irq {
	int irq;
	const char *name;
	irqreturn_t (*handler)(int irq, void *data);
};

struct cs40l26_brwnout_limits {
	u32 max;
	u32 min;
};

struct cs40l26_dbc {
	const char *const name;
	u32 max;
};

struct cs40l26_buzzgen_config {
	const char *duration_name;
	const char *freq_name;
	const char *level_name;
	int effect_id;
};

struct cs40l26_owt_section {
	u8 flags;
	u8 repeat;
	u8 amplitude;
	u8 index;
	u16 delay;
	u16 duration;
	u16 wvfrm_bank;
};

struct cs40l26_pseq_op {
	u8 size;
	u16 offset; /* offset in bytes from pseq_base */
	u8 operation;
	u32 words[3];
	struct list_head list;
};

struct cs40l26_svc_le {
	s32 gain_adjust;
	u32 min;
	u32 max;
	u32 n;
};

struct cs40l26_rom_regs {
	u32 pm_cur_state;
	u32 pm_state_locks;
	u32 pm_timeout_ticks;
	u32 dsp_halo_state;
	u32 event_map_table_event_data_packed;
	u32 p_vibegen_rom;
	u32 rom_pseq_end_of_script;
};

struct cs40l26_rom_data {
	u32 rom_wt_size_words;
	u32 rom_num_waves;
};

struct cs40l26_uploaded_effect {
	int id;
	u32 trigger_index;
	u16 wvfrm_bank;
	enum cs40l26_gpio_map mapping;
	struct list_head list;
};

struct cs40l26_brwnout {
	bool enable;
	u32 thld_uv;
	u32 max_att_db;
	u32 atk_step;
	u32 atk_rate;
	u32 wait;
	u32 rel_rate;
};

struct cs40l26_sysfs_fw {
	u32 algo_id;
	u32 block_type;
	char ctrl_name[CS40L26_COEFF_NAME_MAX_LEN];
};

struct cs40l26_private {
	struct device *dev;
	struct regmap *regmap;
	u32 devid : 24;
	u8 revid;
	struct mutex lock;
	struct gpio_desc *reset_gpio;
	struct input_dev *input;
	struct cl_dsp *dsp;
	struct list_head effect_head;
	unsigned int cur_index;
	struct ff_effect *trigger_effect;
	struct ff_effect upload_effect;
	struct ff_effect *erase_effect;
	s16 *raw_custom_data;
	int raw_custom_data_len;
	struct work_struct vibe_start_work;
	struct work_struct vibe_stop_work;
	struct work_struct set_gain_work;
	struct work_struct upload_work;
	struct work_struct erase_work;
	struct workqueue_struct *vibe_workqueue;
	int irq;
	bool vibe_init_success;
	int pseq_num_ops;
	u32 pseq_base;
	struct list_head pseq_op_head;
	enum cs40l26_pm_state pm_state;
	u32 fw_id;
	bool fw_defer;
	bool fw_rom_only;
	bool fw_loaded;
	bool calib_fw;
	enum cs40l26_vibe_state vibe_state;
	bool vibe_state_reporting;
	bool asp_enable;
	u8 last_wksrc_pol;
	u8 wksrc_sts;
	int cal_requested;
	u16 gain_pct;
	u16 gain_tmp;
	bool scaling_applied;
	u32 event_map_base;
	struct cs40l26_svc_le **svc_le_vals;
	int num_svc_le_vals;
	u32 delay_before_stop_playback_us;
	int upload_ret;
	int erase_ret;
	int effects_in_flight;
	bool comp_enable_pend;
	bool comp_enable_redc;
	bool comp_enable_f0;
	struct completion i2s_cont;
	struct completion erase_cont;
	struct completion cal_f0_cont;
	struct completion cal_redc_cont;
	struct completion cal_ls_cont;
	struct completion cal_dvl_peq_cont;
	unsigned int svc_le_est_stored;
	u32 *no_wait_ram_indices;
	ssize_t num_no_wait_ram_indices;
	struct timer_list hibernate_timer;
	ktime_t allow_hibernate_ts;
	bool allow_hibernate_sent;
	struct cl_dsp_owt_desc rom_wt;
	const struct cs40l26_rom_regs *rom_regs;
	const struct cs40l26_rom_data *rom_data;
#ifdef CONFIG_DEBUG_FS
	struct dentry *debugfs_root;
	struct dentry *debugfs_hw_node;
	u32 dbg_hw_reg;
	struct cl_dsp_debugfs *cl_dsp_db;
#endif
	struct cs40l26_sysfs_fw sysfs_fw;
	struct cs40l26_brwnout vbbr;
	struct cs40l26_brwnout vpbr;
	bool bst_dcm_en;
	u32 bst_ipk;
	u32 asp_scale_pct;
	u32 pm_active_timeout_ms;
	u32 pm_stdby_timeout_ms;
	u32 f0_default;
	u32 redc_default;
	u32 q_default;
	u32 bst_ctl;
	bool expl_mode_enabled;
	bool dbc_tuning_loaded;
	bool dbc_enable;
	u32 dbc_configs[CS40L26_DBC_NUM_CONTROLS];
	bool pwle_zero_cross;
	u32 press_idx;
	u32 release_idx;
	u32 clip_lvl;
	struct regmap_irq_chip_data *irq_data;
	struct cs40l26_log_src *log_srcs;
	u32 num_log_srcs;
	u32 ng_thld;
	u32 ng_delay;
	bool ng_enable;
	u32 aux_ng_thld;
	u32 aux_ng_delay;
	bool aux_ng_enable;
	u32 refclk_input;
	const char *device_name;
	bool cs40l26_not_probed;
	u8 device_id;
};

struct cs40l26_codec {
	struct cs40l26_private *core;
	struct device *dev;
	struct regmap *regmap;
	int sysclk_rate;
	int tuning;
	int tuning_prev;
	char *bin_file;
	u32 daifmt;
	int tdm_width;
	int tdm_slots;
	int tdm_slot[2];
	bool dsp_bypass;
};

struct cs40l26_pll_sysclk_config {
	u32 freq;
	u8 clk_cfg;
};

/* exported function prototypes */
int cs40l26_svc_le_estimate(struct cs40l26_private *cs40l26, unsigned int *le);
int cs40l26_set_pll_loop(struct cs40l26_private *cs40l26, unsigned int pll_loop);
int cs40l26_asp_start(struct cs40l26_private *cs40l26);
int cs40l26_num_waves(struct cs40l26_private *cs40l26);
int cs40l26_fw_swap(struct cs40l26_private *cs40l26, const u32 id);
void cs40l26_vibe_state_update(struct cs40l26_private *cs40l26,
		enum cs40l26_vibe_state_event event);
int cs40l26_pm_timeout_ms_set(struct cs40l26_private *cs40l26, unsigned int dsp_state,
		u32 timeout_ms);
int cs40l26_pm_timeout_ms_get(struct cs40l26_private *cs40l26, unsigned int dsp_state,
		u32 *timeout_ms);
int cs40l26_pm_state_transition(struct cs40l26_private *cs40l26, enum cs40l26_pm_state state);
int cs40l26_get_ram_ext_algo_id(struct cs40l26_private *cs40l26, unsigned int *algo_id);
int cs40l26_mailbox_write(struct cs40l26_private *cs40l26, u32 write_val);
int cs40l26_pm_enter(struct device *dev);
void cs40l26_pm_exit(struct device *dev);
void cs40l26_resume_error_handle(struct device *dev, int ret);
int cs40l26_resume(struct device *dev);
int cs40l26_sys_resume(struct device *dev);
int cs40l26_sys_resume_noirq(struct device *dev);
int cs40l26_suspend(struct device *dev);
int cs40l26_sys_suspend(struct device *dev);
int cs40l26_sys_suspend_noirq(struct device *dev);
int cs40l26_dsp_state_get(struct cs40l26_private *cs40l26, u8 *state);
int cs40l26_probe(struct cs40l26_private *cs40l26);
int cs40l26_remove(struct cs40l26_private *cs40l26);
bool cs40l26_precious_reg(struct device *dev, unsigned int ret);
bool cs40l26_readable_reg(struct device *dev, unsigned int reg);
bool cs40l26_volatile_reg(struct device *dev, unsigned int reg);
int cs40l26_pseq_write(struct cs40l26_private *cs40l26, u32 addr, u32 data, bool update,
		u8 op_code);
int cs40l26_copy_f0_est_to_dvl(struct cs40l26_private *cs40l26);
int cs40l26_rom_wt_init(struct cs40l26_private *cs40l26);

static int cs40l26_probed_retry_count[CS40L26_MAX_DEVICES];
void cs40l26_add_codec_devices(struct device *dev);

/* external tables */
extern struct regulator_bulk_data cs40l26_supplies[CS40L26_NUM_SUPPLIES];
extern const struct dev_pm_ops cs40l26_pm_ops;
extern const struct regmap_config cs40l26_regmap;
extern const struct mfd_cell cs40l26_devs[CS40L26_NUM_MFD_DEVS];
extern const u8 cs40l26_pseq_op_sizes[CS40L26_PSEQ_NUM_OPS][2];
extern const u32 cs40l26_attn_q21_2_vals[CS40L26_NUM_PCT_MAP_VALUES];
extern const struct reg_sequence cs40l26_a1_errata[CS40L26_ERRATA_A1_NUM_WRITES];

/* sysfs */
extern const struct attribute_group *cs40l26_attr_groups[];

/* debugfs */
#ifdef CONFIG_DEBUG_FS
void cs40l26_debugfs_init(struct cs40l26_private *cs40l26);
void cs40l26_debugfs_cleanup(struct cs40l26_private *cs40l26);
#endif /* CONFIG_DEBUG_FS */

#endif /* __CS40L26_H__ */
