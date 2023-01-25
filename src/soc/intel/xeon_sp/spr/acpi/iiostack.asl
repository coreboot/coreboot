/* SPDX-License-Identifier: GPL-2.0-only */

/* ***** PCI Stacks **** */
#undef DEVPREFIX
#define DEVPREFIX  PC
#undef RESPREFIX
#define RESPREFIX  PT
#undef STPREFIX
#define STPREFIX   ST

#undef PRTID
#define PRTID      AR00
#undef STACK
#define STACK 0
#include "pci_resource.asl"

#undef PRTID
#define PRTID      AR01
#undef STACK
#define STACK 1
#include "pci_resource.asl"

#undef STACK
#define STACK 2
#include "pci_resource.asl"

#undef STACK
#define STACK 3
#include "pci_resource.asl"

#undef STACK
#define STACK 4
#include "pci_resource.asl"

#undef STACK
#define STACK 5
#include "pci_resource.asl"

/* ***** CXL Stacks **** */
#undef DEVPREFIX
#define DEVPREFIX  CX
#undef RESPREFIX
#define RESPREFIX  CT

#undef STACK
#define STACK 1
#include "cxl_resource.asl"

#undef STACK
#define STACK 2
#include "cxl_resource.asl"

#undef STACK
#define STACK 3
#include "cxl_resource.asl"

#undef STACK
#define STACK 4
#include "cxl_resource.asl"

#undef STACK
#define STACK 5
#include "cxl_resource.asl"

/* ***** DINO,CPM,HQM Stacks **** */
#undef STACK
#define STACK 8
#include "dino_resource.asl"

#undef STACK
#define STACK 9
#include "dino_resource.asl"

#undef STACK
#define STACK A
#include "dino_resource.asl"

#undef STACK
#define STACK B
#include "dino_resource.asl"

/* ***** Uncore Stacks **** */
#undef STACK
#define STACK D
#include "ubox_resource.asl"
