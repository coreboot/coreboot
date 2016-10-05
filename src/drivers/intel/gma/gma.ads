with Interfaces.C;

with HW;
use HW;

package GMA
is

   procedure gfxinit
     (mmio_base   : in     word64;
      linear_fb   : in     word64;
      phys_fb     : in     word32;
      lightup_ok  :    out Interfaces.C.int);
   pragma Export (C, gfxinit, "gma_gfxinit");

   ----------------------------------------------------------------------------

   function vbe_mode_info_valid return Interfaces.C.int;
   pragma Export (C, vbe_mode_info_valid, "vbe_mode_info_valid");

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

   procedure fill_lb_framebuffer (framebuffer : out lb_framebuffer);
   pragma Export (C, fill_lb_framebuffer, "fill_lb_framebuffer");

end GMA;
