##
## This file is part of the libpayload project.
##
## Copyright (C) 2008 Advanced Micro Devices, Inc.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
## 3. The name of the author may not be used to endorse or promote products
##    derived from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
## ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
## FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
## DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
## OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
## LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
## OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
## SUCH DAMAGE.
##

ifeq ($(CONFIG_LP_TINYCURSES),y)
INCLUDES += -Icurses
endif

libcurses-$(CONFIG_LP_TINYCURSES) += keyboard.c
libcurses-$(CONFIG_LP_TINYCURSES) += tinycurses.c
libcurses-$(CONFIG_LP_TINYCURSES) += colors.c

includes-$(CONFIG_LP_TINYCURSES) += curses.h

ifeq ($(CONFIG_LP_PDCURSES),y)
PDCURSES := PDCurses
INCLUDES += -D_LP64=0 -Icurses/$(PDCURSES) -Icurses/pdcurses-backend -Icurses/menu -Icurses/form
endif

libcurses-$(CONFIG_LP_PDCURSES) += pdcurses-backend/pdcdisp.c
libcurses-$(CONFIG_LP_PDCURSES) += pdcurses-backend/pdcgetsc.c
libcurses-$(CONFIG_LP_PDCURSES) += pdcurses-backend/pdckbd.c
libcurses-$(CONFIG_LP_PDCURSES) += pdcurses-backend/pdcscrn.c
libcurses-$(CONFIG_LP_PDCURSES) += pdcurses-backend/pdcsetsc.c
libcurses-$(CONFIG_LP_PDCURSES) += pdcurses-backend/pdcutil.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/addch.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/move.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/overlay.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/refresh.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/terminfo.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/window.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/util.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/inopts.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/addstr.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/keyname.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/instr.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/clear.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/addchstr.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/kernel.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/pad.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/insstr.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/border.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/getyx.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/getstr.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/getch.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/termattr.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/outopts.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/color.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/deleteln.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/initscr.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/slk.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/delch.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/touch.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/mouse.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/scanw.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/scroll.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/printw.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/bkgd.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/inch.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/attr.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/insch.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/inchstr.c
libcurses-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/beep.c

includes-$(CONFIG_LP_PDCURSES) += pdcurses-backend/nc_alloc.h
includes-$(CONFIG_LP_PDCURSES) += pdcurses-backend/ncurses_cfg.h
includes-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/curses.h
includes-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/term.h
includes-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/panel.h
includes-$(CONFIG_LP_PDCURSES) += menu/eti.h
includes-$(CONFIG_LP_PDCURSES) += menu/menu.h
includes-$(CONFIG_LP_PDCURSES) += menu/mf_common.h
includes-$(CONFIG_LP_PDCURSES) += form/form.h

libpanel-$(CONFIG_LP_PDCURSES) += $(PDCURSES)/pdcurses/panel.c

libmenu-$(CONFIG_LP_PDCURSES) += menu/m_req_name.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_item_nam.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_pad.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_cursor.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_item_new.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_attribs.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_item_opt.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_format.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_post.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_userptr.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_item_cur.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_driver.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_sub.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_win.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_global.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_item_vis.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_new.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_scale.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_spacing.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_opts.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_pattern.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_item_val.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_hook.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_item_use.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_items.c
libmenu-$(CONFIG_LP_PDCURSES) += menu/m_item_top.c
libform-$(CONFIG_LP_PDCURSES) += form/frm_page.c
libform-$(CONFIG_LP_PDCURSES) += form/frm_opts.c
libform-$(CONFIG_LP_PDCURSES) += form/frm_def.c
libform-$(CONFIG_LP_PDCURSES) += form/frm_req_name.c
libform-$(CONFIG_LP_PDCURSES) += form/fty_alpha.c
libform-$(CONFIG_LP_PDCURSES) += form/frm_driver.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_user.c
libform-$(CONFIG_LP_PDCURSES) += form/frm_win.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_newftyp.c
#libform-$(CONFIG_LP_PDCURSES) += form/fty_regex.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_stat.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_pad.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_current.c
libform-$(CONFIG_LP_PDCURSES) += form/frm_post.c
#libform-$(CONFIG_LP_PDCURSES) += form/f_trace.c
libform-$(CONFIG_LP_PDCURSES) += form/fty_generic.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_page.c
libform-$(CONFIG_LP_PDCURSES) += form/frm_hook.c
libform-$(CONFIG_LP_PDCURSES) += form/frm_scale.c
libform-$(CONFIG_LP_PDCURSES) += form/fty_int.c
libform-$(CONFIG_LP_PDCURSES) += form/fty_alnum.c
libform-$(CONFIG_LP_PDCURSES) += form/frm_cursor.c
#libform-$(CONFIG_LP_PDCURSES) += form/fty_ipv4.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_link.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_arg.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_move.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_def.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_type.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_max.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_ftlink.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_ftchoice.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_info.c
libform-$(CONFIG_LP_PDCURSES) += form/frm_user.c
#libform-$(CONFIG_LP_PDCURSES) += form/fty_num.c
libform-$(CONFIG_LP_PDCURSES) += form/frm_sub.c
libform-$(CONFIG_LP_PDCURSES) += form/fty_enum.c
libform-$(CONFIG_LP_PDCURSES) += form/frm_data.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_opts.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_attr.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_dup.c
libform-$(CONFIG_LP_PDCURSES) += form/fld_just.c
