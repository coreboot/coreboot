-- This file is part of the coreboot project.
--
-- This program is free software; you can redistribute it and/or
-- modify it under the terms of the GNU General Public License as
-- published by the Free Software Foundation; version 2 of
-- the License.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   -- For a three-pipe setup, bandwidth is shared between the 2nd and
   -- the 3rd pipe (if it's not eDP). Thus, probe ports that likely
   -- have a high-resolution display attached first, `Internal` last.

   ports : constant Port_List :=
     (DP2,
      DP3,
      HDMI1,
      HDMI2,
      HDMI3,
      Analog,
      Internal,
      others => Disabled);

end GMA.Mainboard;
