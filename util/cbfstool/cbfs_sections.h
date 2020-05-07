/* fmap_sections.h, track which sections of the image will contain CBFSes */
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CBFS_SECTIONS_H_
#define CBFS_SECTIONS_H_

#include "fmd.h"

#include <stdbool.h>

#define SECTION_NAME_FMAP		"FMAP"
#define SECTION_NAME_PRIMARY_CBFS	"COREBOOT"
#define SECTION_NAME_BOOTBLOCK		"BOOTBLOCK"

#define SECTION_ANNOTATION_CBFS		"CBFS"

typedef const struct descriptor_node *cbfs_section_iterator_t;

/** @return Iterator pointing to first CBFS section, or NULL if none exist */
cbfs_section_iterator_t cbfs_sections_iterator(void);

/**
 * Advance iterator to point to the next CBFS section.
 * If it was already pointing to the last such section, it will be set to NULL.
 *
 * @param it (Non-NULL) pointer to (possibly NULL) iterator to be updated
 * @return   Whether it was successfully advanced (wasn't already NULL)
 */
bool cbfs_sections_iterator_advance(cbfs_section_iterator_t *it);

/**
 * @param it Iterator, which must currently be non-NULL
 * @return   Section to which it points
 */
const struct flashmap_descriptor *cbfs_sections_iterator_deref(
						cbfs_section_iterator_t it);

/** @return Whether a section named SECTION_NAME_PRIMARY_CBFS is in the list. */
bool cbfs_sections_primary_cbfs_accounted_for(void);

/** Reclaim the space used to store knowledge of which sections are CBFSes. */
void cbfs_sections_cleanup(void);

#endif
