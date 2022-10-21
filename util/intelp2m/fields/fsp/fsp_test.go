package fsp_test

import (
	"testing"

	"review.coreboot.org/coreboot.git/util/intelp2m/fields/fsp"
	"review.coreboot.org/coreboot.git/util/intelp2m/fields/test"
)

func TestFSPFields(t *testing.T) {
	referenceSlice := []string{
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioPlatformReset, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioHostDeepReset, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLvlEdgDis, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntEdge, GpioResetPwrGood, GpioTolerance1v8 | GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInvOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntApic | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntSci | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntSmi | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntNmi | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, INVALID,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeNative4, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermWpd20K,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeNative2, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermWpd5K,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeNative1, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, INVALID,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirIn, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutLow, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
		"{ GPIO_SKL_H_, { GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInOut, GpioOutHigh, GpioIntDis | GpioIntLevel, GpioResetPwrGood, GpioTermNone,  GpioPadConfigLock } },",
	}
	test.SlidingOneTestSuiteCreate(referenceSlice).Run(t, "SLIDING-ONE-TEST", fsp.FieldMacros{})
}
