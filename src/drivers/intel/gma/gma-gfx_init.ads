-- SPDX-License-Identifier: GPL-2.0-only

with Interfaces.C;

with HW;
use HW;

package GMA.GFX_Init
is

   procedure gfxinit (lightup_ok : out Interfaces.C.int);
   pragma Export (C, gfxinit, "gma_gfxinit");

   ----------------------------------------------------------------------------

   type lb_framebuffer is record
      tag                  : word32;
      size                 : word32;

      physical_address     : word64;
      x_resolution         : word32;
      y_resolution         : word32;
      bytes_per_line       : word32;
      bits_per_pixel       : word8;
      red_mask_pos         : word8;
      red_mask_size        : word8;
      green_mask_pos       : word8;
      green_mask_size      : word8;
      blue_mask_pos        : word8;
      blue_mask_size       : word8;
      reserved_mask_pos    : word8;
      reserved_mask_size   : word8;
   end record;

   function fill_lb_framebuffer
     (framebuffer : in out lb_framebuffer)
      return Interfaces.C.int;
   pragma Export (C, fill_lb_framebuffer, "fill_lb_framebuffer");

end GMA.GFX_Init;
