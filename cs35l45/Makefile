# SPDX-License-Identifier: GPL-2.0
snd-soc-cs35l45-objs := cs35l45.o cs35l45-tables.o
snd-soc-cs35l45-spi-objs := cs35l45-spi.o
snd-soc-cs35l45-i2c-objs := cs35l45-i2c.o

obj-$(CONFIG_SND_SOC_CS35L45)		+= snd-soc-cs35l45.o
obj-$(CONFIG_SND_SOC_CS35L45_SPI)	+= snd-soc-cs35l45-spi.o
obj-$(CONFIG_SND_SOC_CS35L45_I2C)	+= snd-soc-cs35l45-i2c.o

KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build
M ?= $(shell pwd)

EXTRA_CFLAGS += -I$(KERNEL_SRC)/$(M)/include/
EXTRA_CFLAGS += -I$(KERNEL_SRC)/../private/google-modules/amplifiers/snd_soc_wm_adsp
EXTRA_SYMBOLS += $(OUT_DIR)/../private/google-modules/amplifiers/snd_soc_wm_adsp/Module.symvers

KBUILD_OPTIONS += CONFIG_SND_SOC_CS35L45=m \
		  CONFIG_SND_SOC_CS35L45_SPI=m \
		  CONFIG_SND_SOC_CS35L45_I2C=m

modules modules_install clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) W=1 \
	$(KBUILD_OPTIONS) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" KBUILD_EXTRA_SYMBOLS="$(EXTRA_SYMBOLS)" $(@)
