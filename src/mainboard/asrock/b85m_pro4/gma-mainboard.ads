-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   ports : constant Port_List :=
     (HDMI1,  -- DVI-D
      HDMI3,  -- HDMI
      -- FIXME: Haswell VGA support in libgfxinit is broken
      -- Analog, -- VGA
      others => Disabled);

end GMA.Mainboard;
