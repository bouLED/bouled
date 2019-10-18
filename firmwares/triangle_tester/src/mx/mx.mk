C_SOURCES += $(MX)/gpio.c \
			 $(MX)/dma.c \
			 $(MX)/rtc.c \
			 $(MX)/error.c \
			 $(MX)/spi.c \
			 $(MX)/timcallback.c \
			 $(MX)/sysclk.c \
			 $(MX)/stm32f7xx_it.c \
			 $(MX)/stm32f7xx_hal_msp.c \
			 $(MX)/system_stm32f7xx.c \
			 $(MX)/freertos.c \
			 $(MX)/stm32f7xx_hal_timebase_TIM.c \

C_INCLUDES += -I$(MX)
