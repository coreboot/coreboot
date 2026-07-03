-- SPDX-License-Identifier: GPL-2.0-only

with Interfaces.C;
with System;
use System;

package body HW.Time.Timer
   with Refined_State => (Timer_State => null,
                          Abstract_Time => null)
is

   --  The type used here must be consistent with `struct mono_time' and
   --  timer_monotonic_get() in ../include/timer.h.
   procedure Timer_Monotonic_Get (MT : out Word64);
   pragma Import (C, Timer_Monotonic_Get, "timer_monotonic_get");

   procedure Thread_Yield;
   pragma Import (C, Thread_Yield, "thread_yield");
   pragma Weak_External (Thread_Yield);

   function Raw_Value_Min return T
   with
      SPARK_Mode => Off
   is
      Microseconds : Word64;
   begin
      -- Since this procedure is only called when waiting on something
      -- switch tasks now. This lets coreboot run other tasks, like
      -- preloading CBFS files.
      if Thread_Yield'Address /= System.Null_Address then
         Thread_Yield;
      end if;
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
