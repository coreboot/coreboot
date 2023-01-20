-- SPDX-License-Identifier: GPL-2.0-only

with Interfaces.C;

with HW;
use HW;

package GMA.GFX_Init
is

   procedure gfxinit (lightup_ok : out Interfaces.C.int);
   pragma Export (C, gfxinit, "gma_gfxinit");

   procedure gfxstop;
   pragma Export (C, gfxstop, "gma_gfxstop");

   ----------------------------------------------------------------------------

   function c_fb_add_framebuffer_info
     (fb_addr: Interfaces.C.size_t;
      x_resolution : word32;
      y_resolution : word32;
      bytes_per_line : word32;
      bits_per_pixel : word8)
      return Interfaces.C.size_t;

   pragma import (C, c_fb_add_framebuffer_info, "fb_add_framebuffer_info");

end GMA.GFX_Init;
