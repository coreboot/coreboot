-- SPDX-License-Identifier: GPL-2.0-only

with Interfaces.C;

package body HW.Time.Timer
   with Refined_State => (Timer_State => null,
                          Abstract_Time => null)
is

   procedure Timer_Monotonic_Get (MT : out Interfaces.C.long);
   pragma Import (C, Timer_Monotonic_Get, "timer_monotonic_get");

   function Raw_Value_Min return T
   with
      SPARK_Mode => Off
   is
      Microseconds : Interfaces.C.long;
   begin
      Timer_Monotonic_Get (Microseconds);
      return T (Microseconds);
   end Raw_Value_Min;

   function Raw_Value_Max return T
   is
   begin
      return Raw_Value_Min + 1;
   end Raw_Value_Max;

   function Hz return T
   is
   begin
      return 1_000_000;
   end Hz;

end HW.Time.Timer;
