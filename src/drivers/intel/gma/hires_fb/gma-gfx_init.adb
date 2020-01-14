-- SPDX-License-Identifier: GPL-2.0-only

with CB.Config;

use CB;

with HW.GFX;
with HW.GFX.Framebuffer_Filler;
with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX;
use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

with GMA.Mainboard;

package body GMA.GFX_Init
is

   configs : Pipe_Configs;
   ----------------------------------------------------------------------------

   procedure Screen_Rotation (rotation : out Rotation_Type)
   is
   begin
      rotation :=
        (case Config.DEFAULT_SCREEN_ROTATION_INT is
            when     90 => Rotated_90,
            when    180 => Rotated_180,
            when    270 => Rotated_270,
            when others => No_Rotation);
   end Screen_Rotation;

   procedure gfxinit (lightup_ok : out Interfaces.C.int)
   is
      use type pos32;
      use type word64;
      use type word32;
      use type Interfaces.C.size_t;
      use type Interfaces.C.int;

      ports : Port_List;

      success : boolean;

      linear_fb_addr : word64;

      fb : Framebuffer_Type;

      min_h : pos32 := Config.LINEAR_FRAMEBUFFER_MAX_WIDTH;
      min_v : pos32 := Config.LINEAR_FRAMEBUFFER_MAX_HEIGHT;

      fbinfo : Interfaces.C.int;

   begin
      lightup_ok := 0;

      HW.GFX.GMA.Initialize (Success => success);

      if success then
         ports := Mainboard.ports;
         HW.GFX.GMA.Display_Probing.Scan_Ports (configs, ports);

         if configs (Primary).Port /= Disabled then
            for i in Pipe_Index loop
               exit when configs (i).Port = Disabled;

               min_h := pos32'min (min_h, configs (i).Mode.H_Visible);
               min_v := pos32'min (min_v, configs (i).Mode.V_Visible);
            end loop;

            fb := configs (Primary).Framebuffer;
            Screen_Rotation (fb.Rotation);

            if fb.Rotation = Rotated_90 or fb.Rotation = Rotated_270 then
               fb.Width    := Width_Type (min_v);
               fb.Height   := Height_Type (min_h);
               fb.Stride   := Div_Round_Up (fb.Width, 32) * 32;
               fb.V_Stride := Div_Round_Up (fb.Height, 32) * 32;
               fb.Tiling   := Y_Tiled;
               fb.Offset   := word32 (GTT_Rotation_Offset) * GTT_Page_Size;
            else
               fb.Width    := Width_Type (min_h);
               fb.Height   := Height_Type (min_v);
               fb.Stride   := Div_Round_Up (fb.Width, 16) * 16;
               fb.V_Stride := fb.Height;
            end if;

            for i in Pipe_Index loop
               exit when configs (i).Port = Disabled;

               configs (i).Framebuffer := fb;
            end loop;

            pragma Debug (HW.GFX.GMA.Dump_Configs (configs));

            HW.GFX.GMA.Setup_Default_FB
              (FB       => fb,
               Clear    => true,
               Success  => success);

            if success then
               HW.GFX.GMA.Update_Outputs (configs);

               HW.GFX.GMA.Map_Linear_FB (linear_fb_addr, fb);
               if linear_fb_addr /= 0 then
                  fbinfo := C_Fb_Add_Framebuffer_Info_Simple
                     (fb_addr        => Interfaces.C.size_t (linear_fb_addr),
                      x_resolution   => word32 (fb.Width),
                      y_resolution   => word32 (fb.Height),
                      bytes_per_line => word32 (fb.Stride) * 4,
                      bits_per_pixel => 32);
                  if fbinfo /= 0 then
                     lightup_ok := 1;
                  end if;
               end if;
            end if;
         end if;
      end if;
   end gfxinit;

   procedure gfxstop
   is
   begin
      if configs (Primary).Port /= Disabled then
         for i in Pipe_Index loop
            configs (i).Port := Disabled;
         end loop;
         HW.GFX.GMA.Update_Outputs (configs);
      end if;
   end gfxstop;

end GMA.GFX_Init;
