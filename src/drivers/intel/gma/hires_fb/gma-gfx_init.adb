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

   function Default_Framebuffer_Pipe return Pipe_Index
   is
   begin
      -- Display probing may leave pipes disabled in front of active ones,
      -- e.g. on i945 LVDS has to use pipe B, and on Braswell port D can
      -- only be driven by pipe C.
      for i in Pipe_Index loop
         if configs (i).Port /= Disabled then
            return i;
         end if;
      end loop;
      return Primary;
   end Default_Framebuffer_Pipe;

   -- The framebuffer is shared by all active pipes, so it has to fit
   -- the smallest of their modes.
   procedure Setup_Framebuffer (fb : out Framebuffer_Type)
   is
      use type pos32;
      use type word32;

      min_h : pos32 := Config.LINEAR_FRAMEBUFFER_MAX_WIDTH;
      min_v : pos32 := Config.LINEAR_FRAMEBUFFER_MAX_HEIGHT;
   begin
      for i in Pipe_Index loop
         if configs (i).Port /= Disabled then
            min_h := pos32'min (min_h, configs (i).Mode.H_Visible);
            min_v := pos32'min (min_v, configs (i).Mode.V_Visible);
         end if;
      end loop;

      fb := configs (Default_Framebuffer_Pipe).Framebuffer;
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
         if configs (i).Port /= Disabled then
            configs (i).Framebuffer := fb;
         end if;
      end loop;
   end Setup_Framebuffer;

   -- Disables pipes whose output failed to come up, e.g. a DP sink
   -- that answered on AUX but failed link training.
   procedure Drop_Failed_Outputs (dropped : out Boolean)
   is
      active : Pipe_Configs;
   begin
      dropped := False;
      HW.GFX.GMA.Get_Active_Configs (active);
      for i in Pipe_Index loop
         if configs (i).Port /= Disabled and active (i).Port = Disabled then
            configs (i).Port := Disabled;
            dropped := True;
         end if;
      end loop;
   end Drop_Failed_Outputs;

   procedure gfxinit (lightup_ok : out Interfaces.C.int)
   is
      use type word64;
      use type word32;
      use type Interfaces.C.size_t;
      use type Interfaces.C.int;

      ports : Port_List;

      success : boolean;

      linear_fb_addr : word64;

      fb, resized_fb : Framebuffer_Type;

      fbinfo : Interfaces.C.int;

      dropped : boolean;

   begin
      lightup_ok := 0;

      HW.GFX.GMA.Initialize (Success => success);

      if success then
         ports := Mainboard.ports;
         HW.GFX.GMA.Display_Probing.Scan_Ports (configs, ports);

         if configs (Default_Framebuffer_Pipe).Port /= Disabled then
            Setup_Framebuffer (fb);

            pragma Debug (HW.GFX.GMA.Dump_Configs (configs));

            HW.GFX.GMA.Setup_Default_FB
              (FB       => fb,
               Clear    => true,
               Success  => success);

            if success then
               HW.GFX.GMA.Update_Outputs (configs);

               -- A failed output may have constrained the framebuffer
               -- size, resize it to fit the remaining ones only.
               Drop_Failed_Outputs (dropped);
               if dropped and
                  configs (Default_Framebuffer_Pipe).Port /= Disabled
               then
                  Setup_Framebuffer (resized_fb);
                  if resized_fb /= fb then
                     fb := resized_fb;

                     pragma Debug (HW.GFX.GMA.Dump_Configs (configs));

                     HW.GFX.GMA.Setup_Default_FB
                       (FB       => fb,
                        Clear    => true,
                        Success  => success);

                     if success then
                        HW.GFX.GMA.Update_Outputs (configs);
                     end if;
                  end if;
               end if;
            end if;

            if success then
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
      fb_pipe : constant Pipe_Index := Default_Framebuffer_Pipe;
   begin
      if configs (fb_pipe).Port /= Disabled then
         for i in Pipe_Index loop
            configs (i).Port := Disabled;
         end loop;
         HW.GFX.GMA.Update_Outputs (configs);
      end if;
   end gfxstop;

end GMA.GFX_Init;
