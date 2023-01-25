/* SPDX-License-Identifier: GPL-2.0-or-later */

#undef PRTID
#define PRTID AR10

#undef DEVPREFIX
#define DEVPREFIX  UC
#undef RESPREFIX
#define RESPREFIX  UT
#include "pci_resource.asl"

#undef PRTID
#define PRTID AR11

#undef DEVPREFIX
#define DEVPREFIX  UD
#undef RESPREFIX
#define RESPREFIX  UU
#include "pci_resource.asl"
