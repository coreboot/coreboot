/* SPDX-License-Identifier: GPL-2.0-or-later */

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

#ifndef __ASSEMBLER__

/*
 * When printing logs, lines should be printed with the following prefixes in
 * front of them according to the BIOS_LOG_PREFIX_PATTERN printf() pattern.
 */
#define BIOS_LOG_PREFIX_PATTERN "[%.5s]  "
#define BIOS_LOG_PREFIX_MAX_LEVEL BIOS_SPEW
static const char __nonstring bios_log_prefix[BIOS_LOG_PREFIX_MAX_LEVEL + 1][5] = {
	/* Note: These strings are *not* null-terminated to save space. */
	[BIOS_EMERG]   = "EMERG",
	[BIOS_ALERT]   = "ALERT",
	[BIOS_CRIT]    = "CRIT ",
	[BIOS_ERR]     = "ERROR",
	[BIOS_WARNING] = "WARN ",
	[BIOS_NOTICE]  = "NOTE ",
	[BIOS_INFO]    = "INFO ",
	[BIOS_DEBUG]   = "DEBUG",
	[BIOS_SPEW]    = "SPEW ",
};

/*
 * When printing to terminals supporting ANSI escape sequences, the following
 * escape sequences can be printed to highlight the respective log levels
 * according to the BIOS_LOG_ESCAPE_PATTERN printf() pattern. At the end of a
 * line, highlighting should be reset with the BIOS_LOG_ESCAPE_RESET sequence.
 *
 * The escape sequences used here set flags with the following meanings:
 * 1 = bold, 4 = underlined, 5 = blinking, 7 = inverted
 */
#define BIOS_LOG_ESCAPE_PATTERN "\x1b[%sm"
#define BIOS_LOG_ESCAPE_RESET "\x1b[0m"
static const char bios_log_escape[BIOS_LOG_PREFIX_MAX_LEVEL + 1][8] = {
	[BIOS_EMERG]   = "1;4;5;7",
	[BIOS_ALERT]   = "1;4;7",
	[BIOS_CRIT]    = "1;7",
	[BIOS_ERR]     = "7",
	[BIOS_WARNING] = "1;4",
	[BIOS_NOTICE]  = "1",
	[BIOS_INFO]    = "0",
	[BIOS_DEBUG]   = "0",
	[BIOS_SPEW]    = "0",
};

/*
 * When storing console logs somewhere for later retrieval, log level prefixes
 * and escape sequences should not be stored raw to preserve space. Instead, a
 * non-printable control character marker is inserted into the log to indicate
 * the log level. Decoders reading this character should translate it back into
 * the respective escape sequence and prefix. If a decoder doesn't support this
 * feature, the non-printable character should usually be harmless.
 */
#define BIOS_LOG_MARKER_START 0x10
#define BIOS_LOG_MARKER_END (BIOS_LOG_MARKER_START + BIOS_LOG_PREFIX_MAX_LEVEL)
#define BIOS_LOG_IS_MARKER(c) ((c) >= BIOS_LOG_MARKER_START && (c) <= BIOS_LOG_MARKER_END)
#define BIOS_LOG_LEVEL_TO_MARKER(level) (BIOS_LOG_MARKER_START + (level))
#define BIOS_LOG_MARKER_TO_LEVEL(c) ((c) - BIOS_LOG_MARKER_START)

#endif /* __ASSEMBLER__ */

#endif /* LOGLEVEL_H */
