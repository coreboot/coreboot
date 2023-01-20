-- SPDX-License-Identifier: GPL-2.0-only

with HW.GFX;
with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX;
use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

with GMA.Mainboard;

package body GMA.GFX_Init
is
   configs : Pipe_Configs;

   procedure gfxinit (lightup_ok : out Interfaces.C.int)
   is
      ports : Port_List;

      success : boolean;

      -- from pc80/vga driver
      procedure vga_io_init;
      pragma Import (C, vga_io_init, "vga_io_init");
      procedure vga_textmode_init;
      pragma Import (C, vga_textmode_init, "vga_textmode_init");
   begin
      lightup_ok := 0;

      HW.GFX.GMA.Initialize (Success => success);

      if success then
         ports := Mainboard.ports;
         HW.GFX.GMA.Display_Probing.Scan_Ports
           (Configs  => configs,
            Ports    => ports,
            Max_Pipe => Primary);

         if configs (Primary).Port /= Disabled then
            HW.GFX.GMA.Power_Up_VGA;
            vga_io_init;
            vga_textmode_init;

            -- override probed framebuffer config
            configs (Primary).Framebuffer.Width    := 640;
            configs (Primary).Framebuffer.Height   := 400;
            configs (Primary).Framebuffer.Offset   :=
               VGA_PLANE_FRAMEBUFFER_OFFSET;

            pragma Debug (HW.GFX.GMA.Dump_Configs (configs));
            HW.GFX.GMA.Update_Outputs (configs);

            lightup_ok := 1;
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
