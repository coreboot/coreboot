with HW.GFX.GMA;

use HW.GFX.GMA;

private package GMA.Mainboard is

   -- For a three-pipe setup, bandwidth is shared between the 2nd and
   -- the 3rd pipe (if it's not eDP). Thus, probe ports that likely
   -- have a high-resolution display attached first, `Internal` last.

   ports : constant Port_List :=
     (DP2,
      DP3,
      Digital2,
      Digital3,
      Analog,
      Internal,
      others => Disabled);

end GMA.Mainboard;
