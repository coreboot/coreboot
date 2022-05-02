/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(POST_IO)

/* POST code support, typically on port 80 */
OperationRegion (POST, SystemIO, CONFIG_POST_IO_PORT, 1)
Field (POST, ByteAcc, Lock, Preserve)
{
	DBG0, 8
}

#else

/* Dummy placeholder to avoid issues */
Name (DBG0, 0)

#endif
