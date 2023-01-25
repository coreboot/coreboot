/* SPDX-License-Identifier: GPL-2.0-or-later */

#undef PRTID
#define PRTID      AR12

#undef DEVPREFIX
#define DEVPREFIX  DI
#undef RESPREFIX
#define RESPREFIX  DT
#include "pci_resource.asl"

#undef DEVPREFIX
#define DEVPREFIX  PM
#undef RESPREFIX
#define RESPREFIX  MT
#include "pci_resource.asl"

#undef DEVPREFIX
#define DEVPREFIX  HQ
#undef RESPREFIX
#define RESPREFIX  HT
#include "pci_resource.asl"

#undef DEVPREFIX
#define DEVPREFIX  PN
#undef RESPREFIX
#define RESPREFIX  MU
#include "pci_resource.asl"

#undef DEVPREFIX
#define DEVPREFIX  HR
#undef RESPREFIX
#define RESPREFIX  HU
#include "pci_resource.asl"
