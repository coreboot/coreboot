/**
Copyright (c) 2019-2020, Intel Corporation. All rights reserved.<BR>

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.

**/


#ifndef _GPIO_FSP_H_
#define _GPIO_FSP_H_

//
// Below defines are based on GPIO_CONFIG structure fields
//
#define GPIO_CONF_PAD_MODE_MASK 0xF
#define GPIO_CONF_PAD_MODE_BIT_POS 0
#define GPIO_CONF_HOST_OWN_MASK 0x3
#define GPIO_CONF_HOST_OWN_BIT_POS 0
#define GPIO_CONF_DIR_MASK 0x7
#define GPIO_CONF_DIR_BIT_POS 0
#define GPIO_CONF_INV_MASK 0x18
#define GPIO_CONF_INV_BIT_POS 3
#define GPIO_CONF_OUTPUT_MASK 0x3
#define GPIO_CONF_OUTPUT_BIT_POS 0
#define GPIO_CONF_INT_ROUTE_MASK 0x1F
#define GPIO_CONF_INT_ROUTE_BIT_POS 0
#define GPIO_CONF_INT_TRIG_MASK 0xE0
#define GPIO_CONF_INT_TRIG_BIT_POS 5
#define GPIO_CONF_RESET_MASK 0x7
#define GPIO_CONF_RESET_BIT_POS 0
#define GPIO_CONF_TERM_MASK 0x1F
#define GPIO_CONF_TERM_BIT_POS 0
#define GPIO_CONF_PADTOL_MASK 0x60
#define GPIO_CONF_PADTOL_BIT_POS 5
#define GPIO_CONF_LOCK_MASK 0x7
#define GPIO_CONF_LOCK_BIT_POS 0
#define GPIO_CONF_RXRAW_MASK 0x3
#define GPIO_CONF_RXRAW_BIT_POS 0

typedef enum { GpioHardwareDefault = 0x0 } GPIO_HARDWARE_DEFAULT;

///
/// GPIO Pad Mode
///
typedef enum {
	GpioPadModeGpio = 0x1,
	GpioPadModeNative1 = 0x3,
	GpioPadModeNative2 = 0x5,
	GpioPadModeNative3 = 0x7,
	GpioPadModeNative4 = 0x9
} GPIO_PAD_MODE;

///
/// Host Software Pad Ownership modes
///
typedef enum {
	GpioHostOwnDefault = 0x0, ///< Leave ownership value unmodified
	GpioHostOwnAcpi = 0x1,    ///< Set HOST ownership to ACPI
	GpioHostOwnGpio = 0x3     ///< Set HOST ownership to GPIO
} GPIO_HOSTSW_OWN;

///
/// GPIO Direction
///
typedef enum {
	GpioDirDefault = 0x0, ///< Leave pad direction setting unmodified
	GpioDirInOut =
		(0x1 | (0x1 << 3)), ///< Set pad for both output and input
	GpioDirInInvOut = (0x1 | (0x3 << 3)), ///< Set pad for both output and
					      ///input with inversion
	GpioDirIn = (0x3 | (0x1 << 3)),       ///< Set pad for input only
	GpioDirInInv = (0x3 | (0x3 << 3)), ///< Set pad for input with inversion
	GpioDirOut = 0x5,		   ///< Set pad for output only
	GpioDirNone = 0x7		   ///< Disable both output and input
} GPIO_DIRECTION;

///
/// GPIO Output State
///
typedef enum {
	GpioOutDefault = 0x0, ///< Leave output value unmodified
	GpioOutLow = 0x1,     ///< Set output to low
	GpioOutHigh = 0x3     ///< Set output to high
} GPIO_OUTPUT_STATE;

///
/// GPIO interrupt configuration
/// This setting is applicable only if GPIO is in input mode.
/// GPIO_INT_CONFIG allows to choose which interrupt is generated
/// (IOxAPIC/SCI/SMI/NMI)
/// and how it is triggered (edge or level).
/// Field from GpioIntNmi to GpioIntApic can be OR'ed with GpioIntLevel to
/// GpioIntBothEdgecan
/// to describe an interrupt e.g. GpioIntApic | GpioIntLevel
/// If GPIO is set to cause an SCI then also Gpe is enabled for this pad.
/// Not all GPIO are capable of generating an SMI or NMI interrupt
///

typedef enum {
	GpioIntDefault = 0x0, ///< Leave value of interrupt routing unmodified
	GpioIntDis = 0x1, ///< Disable IOxAPIC/SCI/SMI/NMI interrupt generation
	GpioIntNmi = 0x3, ///< Enable NMI interrupt only
	GpioIntSmi = 0x5, ///< Enable SMI interrupt only
	GpioIntSci = 0x9, ///< Enable SCI interrupt only
	GpioIntApic = 0x11,	///< Enable IOxAPIC interrupt only
	GpioIntLevel = (0x1 << 5), ///< Set interrupt as level triggered
	GpioIntEdge = (0x3 << 5),  ///< Set interrupt as edge triggered (type of
				   ///edge depends on input inversion)
	GpioIntLvlEdgDis = (0x5 << 5), ///< Disable interrupt trigger
	GpioIntBothEdge = (0x7 << 5)   ///< Set interrupt as both edge triggered
} GPIO_INT_CONFIG;

///
/// GPIO Power Configuration
/// GPIO_RESET_CONFIG allows to set GPIO Reset (used to reset the specified
/// Pad Register fields).
///
typedef enum {
	GpioResetDefault = 0x0, ///< Leave value of pad reset unmodified
	GpioResetPwrGood = 0x1, ///< Powergood reset
	GpioResetDeep = 0x3,    ///< Deep GPIO Reset
	GpioResetNormal = 0x5,  ///< GPIO Reset
	GpioResetResume = 0x7 ///< Resume Reset (applicable only for GPD group)
} GPIO_RESET_CONFIG;

///
/// GPIO Electrical Configuration
/// Set GPIO termination and Pad Tolerance (applicable only for some pads)
/// Field from GpioTermDefault to GpioTermNative can be OR'ed with
/// GpioTolerance1v8.
///
typedef enum {
	GpioTermDefault = 0x0,  ///< Leave termination setting unmodified
	GpioTermNone = 0x1,     ///< none
	GpioTermWpd5K = 0x5,    ///< 5kOhm weak pull-down
	GpioTermWpd20K = 0x9,   ///< 20kOhm weak pull-down
	GpioTermWpu1K = 0x13,   ///< 1kOhm weak pull-up
	GpioTermWpu2K = 0x17,   ///< 2kOhm weak pull-up
	GpioTermWpu5K = 0x15,   ///< 5kOhm weak pull-up
	GpioTermWpu20K = 0x19,  ///< 20kOhm weak pull-up
	GpioTermWpu1K2K = 0x1B, ///< 1kOhm & 2kOhm weak pull-up
	GpioTermNative = 0x1F,  ///< Native function controls pads termination
	GpioNoTolerance1v8 = (0x1 << 5), ///< Disable 1.8V pad tolerance
	GpioTolerance1v8 = (0x3 << 5)    ///< Enable 1.8V pad tolerance
} GPIO_ELECTRICAL_CONFIG;

///
/// GPIO LockConfiguration
/// Set GPIO configuration lock and output state lock
/// GpioLockPadConfig and GpioLockOutputState can be OR'ed
///
typedef enum {
	GpioLockDefault = 0x0,    ///< Leave lock setting unmodified
	GpioPadConfigLock = 0x3,  ///< Lock Pad Configuration
	GpioOutputStateLock = 0x5 ///< Lock GPIO pad output value
} GPIO_LOCK_CONFIG;

///
/// Other GPIO Configuration
/// GPIO_OTHER_CONFIG is used for less often settings and for future extensions
/// Supported settings:
///  - RX raw override to '1' - allows to override input value to '1'
///     This setting is applicable only if in input mode (both in GPIO and
///     native usage).
///     The override takes place at the internal pad state directly from buffer
///     and before the RXINV.
///
typedef enum {
	GpioRxRaw1Default = 0x0, ///< Use default input override value
	GpioRxRaw1Dis = 0x1,     ///< Don't override input
	GpioRxRaw1En = 0x3       ///< Override input to '1'
} GPIO_OTHER_CONFIG;

//
// Possible values of Pad Ownership
//
typedef enum {
	GpioPadOwnHost = 0x0,
	GpioPadOwnCsme = 0x1,
	GpioPadOwnIsh = 0x2,
} GPIO_PAD_OWN;

#endif
