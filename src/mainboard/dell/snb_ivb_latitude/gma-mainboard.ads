-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   ports : constant Port_List :=
     (
      HDMI1, -- mainboard HDMI
      HDMI2, -- dock DVI
      HDMI3, -- dock DVI
      DP2, -- dock DP
      DP3, -- dock DP
      Analog, -- mainboard/dock VGA (muxed)
      LVDS,
      others => Disabled);

end GMA.Mainboard;
