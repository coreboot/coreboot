with HW.GFX;
with HW.GFX.Framebuffer_Filler;
with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX;
use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

with GMA.Mainboard;

package body GMA
is

   fb_valid : boolean := false;

   linear_fb_addr : word64;

   fb : Framebuffer_Type;

   function fill_lb_framebuffer
     (framebuffer : in out lb_framebuffer)
      return Interfaces.C.int
   is
      use type word32;
      use type Interfaces.C.int;
   begin
      if fb_valid then
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
         return 0;
      else
         return -1;
      end if;
   end fill_lb_framebuffer;

   ----------------------------------------------------------------------------

   procedure gfxinit (lightup_ok : out Interfaces.C.int)
   is
      use type pos32;
      use type word64;

      ports : Port_List;
      configs : Pipe_Configs;

      success : boolean;

      min_h : pos16 := pos16'last;
      min_v : pos16 := pos16'last;
   begin
      lightup_ok := 0;

      HW.GFX.GMA.Initialize (Success => success);

      if success then
         ports := Mainboard.ports;
         HW.GFX.GMA.Display_Probing.Scan_Ports (configs, ports);

         if configs (Primary).Port /= Disabled then
            for i in Pipe_Index loop
               exit when configs (i).Port = Disabled;

               min_h := pos16'min (min_h, configs (i).Mode.H_Visible);
               min_v := pos16'min (min_v, configs (i).Mode.V_Visible);
            end loop;

            fb :=
               (Width   => Width_Type (min_h),
                Height  => Height_Type (min_v),
                BPC     => 8,
                Stride  => Div_Round_Up (Width_Type (min_h), 16) * 16,
                Offset  => 0);
            for i in Pipe_Index loop
               exit when configs (i).Port = Disabled;

               configs (i).Framebuffer := fb;
            end loop;

            HW.GFX.GMA.Dump_Configs (configs);

            HW.GFX.GMA.Setup_Default_FB
              (FB       => fb,
               Clear    => true,
               Success  => success);

            if success then
               HW.GFX.GMA.Update_Outputs (configs);

               HW.GFX.GMA.Map_Linear_FB (linear_fb_addr, fb);
               fb_valid := linear_fb_addr /= 0;

               lightup_ok := (if fb_valid then 1 else 0);
            end if;
         end if;
      end if;
   end gfxinit;

end GMA;
