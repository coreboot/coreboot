with HW.GFX;
with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX;
use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

with GMA.Mainboard;

package body GMA
is

   function vbe_mode_info_valid return Interfaces.C.int
   is
   begin
      return 0;
   end vbe_mode_info_valid;

   procedure fill_lb_framebuffer (framebuffer : out lb_framebuffer)
   is
   begin
      null;
   end fill_lb_framebuffer;

   ----------------------------------------------------------------------------

   procedure gfxinit
     (mmio_base   : in     word64;
      linear_fb   : in     word64;
      phys_fb     : in     word32;
      lightup_ok  :    out Interfaces.C.int)
   is
      ports : Port_List;
      configs : Pipe_Configs;

      success : boolean;

      -- from pc80/vga driver
      procedure vga_io_init;
      pragma Import (C, vga_io_init, "vga_io_init");
      procedure vga_textmode_init;
      pragma Import (C, vga_textmode_init, "vga_textmode_init");
   begin
      lightup_ok := 0;

      HW.GFX.GMA.Initialize
        (MMIO_Base   => mmio_base,
         Success     => success);

      if success then
         ports := Mainboard.ports;
         HW.GFX.GMA.Display_Probing.Scan_Ports
           (Configs  => configs,
            Ports    => ports,
            Max_Pipe => Primary);

         if configs (Primary).Port /= Disabled then
            vga_io_init;
            vga_textmode_init;

            configs (Primary).Framebuffer :=
              (Width    => 640,
               Height   => 400,
               BPC      => Auto_BPC,   -- ignored for VGA plane
               Stride   => 320,        -- ignored
               Offset   => VGA_PLANE_FRAMEBUFFER_OFFSET);

            HW.GFX.GMA.Dump_Configs (configs);
            HW.GFX.GMA.Update_Outputs (configs);

            lightup_ok := 1;
         end if;
      end if;
   end gfxinit;

end GMA;
