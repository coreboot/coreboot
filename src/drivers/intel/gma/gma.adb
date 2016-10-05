with HW.GFX;
with HW.GFX.Framebuffer_Filler;
with HW.GFX.GMA;

use HW.GFX;
use HW.GFX.GMA;

with GMA.Mainboard;

package body GMA
is

   vbe_valid : boolean := false;

   linear_fb_addr : word64;

   fb : Framebuffer_Type;

   function vbe_mode_info_valid return Interfaces.C.int
   is
   begin
      return (if vbe_valid then 1 else 0);
   end vbe_mode_info_valid;

   procedure fill_lb_framebuffer (framebuffer : out lb_framebuffer)
   is
      use type word32;
   begin
      framebuffer :=
        (tag                  =>  0,
         size                 =>  0,
         physical_address     => linear_fb_addr,
         x_resolution         => word32 (fb.Width),
         y_resolution         => word32 (fb.Height),
         bytes_per_line       => 4 * word32 (fb.Stride),
         bits_per_pixel       => 32,
         reserved_mask_pos    => 24,
         reserved_mask_size   =>  8,
         red_mask_pos         => 16,
         red_mask_size        =>  8,
         green_mask_pos       =>  8,
         green_mask_size      =>  8,
         blue_mask_pos        =>  0,
         blue_mask_size       =>  8);
   end fill_lb_framebuffer;

   ----------------------------------------------------------------------------

   procedure gfxinit
     (mmio_base   : in     word64;
      linear_fb   : in     word64;
      phys_fb     : in     word32;
      lightup_ok  :    out Interfaces.C.int)
   is
      use type pos32;

      ports : Port_List;
      configs : Configs_Type;

      success : boolean;

      stride : Width_Type;
      max_h : pos16 := 1;
      max_v : pos16 := 1;
   begin
      lightup_ok := 0;

      HW.GFX.GMA.Initialize
        (MMIO_Base   => mmio_base,
         Success     => success);

      if success then
         ports := Mainboard.ports;
         HW.GFX.GMA.Scan_Ports (configs, ports);

         if configs (Primary).Port /= Disabled then
            for i in Config_Index loop
               exit when configs (i).Port = Disabled;

               max_h := pos16'max (max_h, configs (i).Mode.H_Visible);
               max_v := pos16'max (max_v, configs (i).Mode.V_Visible);
            end loop;

            stride := ((Width_Type (max_h) + 63) / 64) * 64;
            for i in Config_Index loop
               exit when configs (i).Port = Disabled;

               configs (i).Framebuffer :=
                 (Width    => Width_Type (configs (i).Mode.H_Visible),
                  Height   => Height_Type (configs (i).Mode.V_Visible),
                  BPC      => 8,
                  Stride   => stride,
                  Offset   => 0);
            end loop;

            HW.GFX.GMA.Dump_Configs (configs);

            fb :=
               (Width   => Width_Type (max_h),
                Height  => Height_Type (max_v),
                BPC     => 8,
                Stride  => stride,
                Offset  => 0);
            HW.GFX.GMA.Setup_Default_GTT (fb, phys_fb);
            HW.GFX.Framebuffer_Filler.Fill (linear_fb, fb);

            HW.GFX.GMA.Update_Outputs (configs);

            linear_fb_addr := linear_fb;
            vbe_valid := true;

            lightup_ok := 1;
         end if;
      end if;
   end gfxinit;

end GMA;
