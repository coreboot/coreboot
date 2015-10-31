/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Nicholas Sielicki <sielicki@nicky.io>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef LOGLEVEL_H
#define LOGLEVEL_H

/**
 * @file loglevel.h
 *
 * \brief Definitions of the log levels to be used in printk calls.
 *
 * Safe for inclusion in assembly.
 *
 */

/**
 * \brief BIOS_EMERG - Emergency / Fatal
 *
 * Log level for when the system is entirely unusable. To be used when execution
 * is halting as a result of the failure. No further instructions should run.
 *
 * Example - End of all debug output / death notice.
 *
 * @{
 */
#define BIOS_EMERG      0
/** @} */

/**
 * \brief BIOS_ALERT - Dying / Unrecoverable
 *
 * Log level for when the system is certainly in the process of dying.
 * To be used when execution will eventually halt as a result of the
 * failure, but the system can still output valuable debugging
 * information.
 *
 * Example - Ram initialization fails, dumping relevant POST codes and
 * information
 *
 * @{
 */
#define BIOS_ALERT      1
/** @} */

/**
 * \brief BIOS_CRIT - Recovery unlikely
 *
 * Log level for when the system has experienced a dire issue in essential
 * components. To be used when boot will probably be unsuccessful as a
 * result of the failure, but recovery/retry can be attempted.
 *
 * Example - MSR failures, SMM/SMI failures.
 * or
 *
 * @{
 */
#define BIOS_CRIT       2
/** @} */

/**
 * \brief BIOS_ERR - System in incomplete state.
 *
 * Log level for when the system has experienced an issue that may not preclude
 * a successful boot. To be used when coreboot execution may still succeed,
 * but the error places some non-essential portion of the machine in a broken
 * state that will be noticed downstream.
 *
 * Example - Payload could still load, but will be missing access to integral
 * components such as drives.
 *
 * @{
 */
#define BIOS_ERR        3
/** @} */

/**
 * \brief BIOS_WARNING - Bad configuration
 *
 * Log level for when the system has noticed an issue that most likely will
 * not preclude a successful boot. To be used when something is wrong, and
 * would likely be noticed by an end user.
 *
 * Example - Bad ME firmware, bad microcode, mis-clocked CPU
 *
 * @{
 */
#define BIOS_WARNING    4
/** @} */

/**
 * \brief BIOS_NOTICE - Unexpected but relatively insignificant
 *
 * Log level for when the system has noticed an issue that is an edge case,
 * but is handled and is recoverable. To be used when an end-user would likely
 * not notice.
 *
 * Example - Hardware was misconfigured, but is promptly fixed.
 *
 * @{
 */
#define BIOS_NOTICE     5
/** @} */

/**
 * \brief BIOS_INFO - Expected events.
 *
 * Log level for when the system has experienced some typical event.
 * Messages should be superficial in nature.
 *
 * Example - Success messages. Status messages.
 *
 * @{
 */
#define BIOS_INFO       6
/** @} */

/**
 * \brief BIOS_DEBUG - Verbose output
 *
 * Log level for details of a method. Messages may be dense,
 * but should not be excessive. Messages should be detailed enough
 * that this level provides sufficient details to diagnose a problem,
 * but not necessarily enough to fix it.
 *
 * Example - Printing of important variables.
 *
 * @{
 */
#define BIOS_DEBUG      7
/** @} */

/**
 * \brief BIOS_SPEW - Excessively verbose output
 *
 * Log level for intricacies of a method. Messages might contain raw
 * data and will produce large logs. Developers should try to make sure
 * that this level is not useful to anyone besides developers.
 *
 * Example - Data dumps.
 *
 * @{
 */
#define BIOS_SPEW       8
/** @} */

/**
 * \brief BIOS_NEVER - Muted log level.
 *
 * Roughly equal to commenting out a printk statement. Because a user
 * should not set their log level higher than 8, these statements
 * are never printed.
 *
 * Example - A developer might locally define MY_LOGLEVEL to BIOS_SPEW,
 * and later replace it with BIOS_NEVER as to mute their debug output.
 *
 * @{
 */
#define BIOS_NEVER	9
/** @} */

#endif /* LOGLEVEL_H */
