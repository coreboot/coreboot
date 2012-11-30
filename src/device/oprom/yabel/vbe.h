/******************************************************************************
 * Copyright (c) 2004, 2008 IBM Corporation
 * All rights reserved.
 * This program and the accompanying materials
 * are made available under the terms of the BSD License
 * which accompanies this distribution, and is available at
 * http://www.opensource.org/licenses/bsd-license.php
 *
 * Contributors:
 *     IBM Corporation - initial implementation
 *****************************************************************************/

#ifndef _BIOSEMU_VBE_H_
#define _BIOSEMU_VBE_H_

struct lb_framebuffer;

void vbe_set_graphics(void);
int vbe_mode_info_valid(void);
void fill_lb_framebuffer(struct lb_framebuffer *framebuffer);
void vbe_textmode_console(void);

#endif
