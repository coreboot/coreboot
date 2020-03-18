-- SPDX-License-Identifier: GPL-2.0-or-later
-- This file is part of the coreboot project.

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   ports : constant Port_List :=
     (HDMI1,	-- DVI-D port on rear
      HDMI3,	-- real HDMI port on rear
      Analog,	-- VGA port on rear
      others => Disabled);

end GMA.Mainboard;
