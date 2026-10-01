-- SPDX-License-Identifier: GPL-2.0-only

package body HW.Time.Timer
   with Refined_State => (Timer_State => null,
                          Abstract_Time => null)
is

   --  The type used here must be consistent with `struct mono_time' and
   --  timer_monotonic_get() in ../include/timer.h.
   procedure Timer_Monotonic_Get (MT : out Word64);
   pragma Import (C, Timer_Monotonic_Get, "timer_monotonic_get");

   function Raw_Value_Min return T
   with
      SPARK_Mode => Off
   is
      Microseconds : Word64;
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
