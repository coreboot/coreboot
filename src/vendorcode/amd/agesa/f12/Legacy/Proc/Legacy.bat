@echo off
goto Begin
:Documentation
echo off
echo **************************************************************************
echo * Legacy.Bat
echo *     This batchfile establishes the build environment for an AGESA(TM)
echo *     legacy build.
echo *
echo * Syntax:
echo *           Legacy.bat   Solution  [DevTipPath]  [DevKey] [DoxKey] [Release^|Debug] [Internal]
echo * Parameters:
echo *   %% 1  Solution name. This is the AMD Platform Solution name that
echo *             identifies the delivery package. No default, this
echo *             parameter MUST be specified. This parameter MUST
echo *             be specified before the DevTip Path parameter.
echo *   %% 2  Path to the platform specific build directory. This is
echo *             where the options file %%Solution%%Options.c is located,
echo *             which contains the platform specific build option settings.
echo *             If left blank, the default %%Solution%%Options.c file from
echo *             the AGESA\Addendum directory is used.
echo *   %% 3  Developer's key - open an interactive MSVS window. This parameter is
echo *             optional, but when used it requires the first two parameters.
echo *   %% 4  Documentation Key - requests a compilation of the DOxygen
echo *             documentation files. key is "DOX". This parameter is
echo *             optional, but when used it requires the first two parameters.
echo *   %% 5  Build Configuration. There are two choices, "Release" and "Debug".
echo *             Debug is the default choice if not specified. This parameter is
echo *             optional, but when used it requires the first two parameters.
echo *   %% 6  Internal solution file select - choose a internal solution file.This parameter
echo *             is optional, but when used it requires the first two parameters.
echo *    NOTE: The paths MUST have a trailing '\' character.
echo *
echo *   The AGESA build environment requires the following environment
echo *   variables MUST be defined for proper operation:
echo *     $(AGESA_ROOT) - identifies the directory where the AGESA code
echo *                    is located. The top most dir; where AGESA.h is located.
echo *                    This must be the full path, eg D:\myPlatform\AGESA\
echo *           **** This var is expected to be set by the caller ****
echo *
echo *     $(Solution) - identifies the Platform Solution being targeted
echo *     $(AGESA_OptsDir) - identifies the directory where the platform
echo *                    options and customization files are located.
echo *                    This must be the full path
echo *
echo *     Optional build environment variables
echo *
echo *        set AGESA_B1_ADDRESS=0xFFF?????
echo *            Sets the B1 image base address in the ROM (otherwise 0xFFFD0000).
echo *        set AGESA_B2_ADDRESS=0xFFF?????
echo *            Sets the B2 image base address in the ROM (otherwise 0xFFFA0000).
echo *
echo *    NOTE: The paths MUST have a trailing '\' character.
echo **************************************************************************
goto Exit

:Begin
rem  Clear working vars...
set DevKey=
set DoxKey=
set Solution=
set Configuration=

rem set default to external file prefix none
set SolutionType=

rem Validate the AGESA_ROOT variable
IF "%AGESA_ROOT%"==""  goto Else001
  rem User has set a path for Agesa_Root, verify that path...
  IF EXIST %AGESA_ROOT%Agesa.h  goto EndIf002
    rem Try adding trailing '\'
    set AGESA_ROOT=%AGESA_ROOT%\
    IF EXIST %AGESA_ROOT%Agesa.h  goto EndIf003
      echo Build ERROR: Environment variable AGESA_ROOT is invalid,
      echo Build ERROR: Could not find file Agesa.h at: %AGESA_ROOT%
      pause
      goto Documentation
    :EndIf003
  :EndIf002
  goto EndIf001
:Else001
  echo Build ERROR: Required environment variable missing: AGESA_ROOT
  pause
  goto Documentation
:EndIf001

rem Start processing the input parameters...
:DoLoop
  rem Switch (%1)
  IF  /I "%1"=="DevKey"         goto Case001
  IF  /I "%1"=="Maranello"      goto Case002
  IF  /I "%1"=="DevTest"        goto Case002
  IF  /I "%1"=="Danube"         goto Case002
  IF  /I "%1"=="DanNi"          goto Case002
  IF  /I "%1"=="Deccan"         goto Case002
  IF  /I "%1"=="Nile"           goto Case002
  IF  /I "%1"=="Dragon"         goto Case002
  IF  /I "%1"=="SanMarino"      goto Case002
  IF  /I "%1"=="Scorpius"       goto Case002
  IF  /I "%1"=="Sabine"         goto Case002
  IF  /I "%1"=="Lynx"           goto Case002
  IF  /I "%1"=="Brazos"         goto Case002
  IF  /I "%1"=="Comal"          goto Case002
  IF  /I "%1"=="Virgo"          goto Case002
  IF  /I "%1"=="DOX"            goto Case009
  IF  /I "%1"=="Debug"          goto Case011
  IF  /I "%1"=="Release"        goto Case011
  IF  /I "%1"=="Internal"       goto Case012
  IF  "%1"==""                  goto Case006
  goto Case004
  :Case001
      rem  Case 1: Special command param
      set DevKey=DevKey
      goto EndSwitch
  :Case002
      rem  Case 2: Solution name parameter
      rem    Set the Platform Solution name for the build environment
      rem    Validation of the names was done by the IF's at the top of DoLoop
      set Solution=%1
      goto EndSwitch
  :Case004
      rem  Case 4: Path param, check for validity
      rem  Check if parameter matches the default
      rem  Check for shortcut versions of the default. Also
      rem   checking for missing trailing '\'
      IF /I "%1"=="Addendum"         goto Then040
      IF /I "%1"=="Addendum\"        goto Then040
      IF /I "%1"=="..\Addendum"      goto Then040
      IF /I "%1"=="..\Addendum\"     goto Then040
      IF /I "%1"=="..\..\Addendum"   goto Then040
      IF /I "%1"=="..\..\Addendum\"  goto Then040
      goto Else040
      :Then040
        set AGESA_OptsDir=%AGESA_ROOT%Addendum\
        Echo  Build WARNING: User selected the default options path.
        goto EndIf040
      :Else040
        set AGESA_OptsDir=%1
        rem  Non-default path given,
        rem  Validate the AGESA_OptsDir parameter ( %%1 = %1 )...
        IF /I "%Solution%"=="" goto Else042
          IF EXIST %AGESA_OptsDir%%Solution%Options.c  goto EndIf044
            rem Check if param was missing trailing '\'...
            set   AGESA_OptsDir=%AGESA_OptsDir%\
            IF EXIST %AGESA_OptsDir%%Solution%Options.c  goto EndIf045
              rem  If using the default, assume the release file is in place.
              rem  Could not find options file, post an error
              Echo  Build ERROR: Path to AGESA options file is not valid...
              Echo  .    .   Could not find options file: %AGESA_OptsDir%%Solution%Options.c
              set AGESA_OptsDir=
              set Solution=
              exit /B 5
            :EndIf045
          :EndIf044
          goto EndIf042
        :Else042
          rem Else042 - parameter ordering error, need to specify solution before tip path
          Echo  Build ERROR: The 'Solution' name was not recognized or
          Echo               The Solution must be specified before the Development tip path
          Echo  .    .    .  Legacy.bat  Solution  [DevTipPath]  [DevKey]  [DoxKey]
          exit /B 4
        :EndIf042
      :EndIf040
      goto EndSwitch
  :Case006
      rem  Case 6: Parameter is blank, end of list
      rem    Validate the required Environment Variables
      IF /I "%Solution%"=="" goto Else066
        IF NOT "%AGESA_OptsDir%"=="" goto Else060
          Rem  No path param specified. Use the default file shipped with the AGESA package...
          Echo  Build WARNING: Using default options file.
          set AGESA_OptsDir=%AGESA_ROOT%Addendum\
          goto EndIf060
        :Else060
          rem Else060 - tip path may have been set by caller prior to call,
          rem  Validate the AGESA_OptsDir parameter ( %%AGESA_OptsDir = %AGESA_OptsDir% )...
          IF EXIST %AGESA_OptsDir%%Solution%Options.c  goto EndIf061
            rem Check if param was missing trailing '\'...
            set   AGESA_OptsDir=%AGESA_OptsDir%\
            IF EXIST %AGESA_OptsDir%%Solution%Options.c  goto EndIf062
              rem  If using the default, assume the release file is in place.
              rem  Could not find options file, post an error
              Echo  Build ERROR: Path to AGESA options file is not valid...
              Echo  .    .   Could not find options file: %AGESA_OptsDir%%Solution%Options.c
              set AGESA_OptsDir=
              set Solution=
              exit /B 4
            :EndIf062
          :EndIf061
        :EndIf060
	goto EndIf066
      :Else066
        rem Else066 - parameter missing error, need to specify 'solution'
        Echo  Build ERROR: The Platform Solution name must be specified:
        Echo  .    .    .  Legacy.bat  Solution  [DevTipPath]  [DevKey]  [DoxKey]
        exit /B 4
      :EndIf066
      goto EndDo
  :Case009
      rem Case 9: Documentation generation key
      set DoxKey=DoxKey
      goto EndSwitch
  :Case011
      rem Case 11: Build Configuration
      set Configuration=%1
      goto EndSwitch
  :Case012
      rem  Case 12: Select Internal solution file prefix
  		set SolutionType=%1
      goto EndSwitch
  :EndSwitch
  SHIFT
  goto DoLoop
:EndDo

rem # Ensure that Microsoft Visual Studio 2005 is installed on this machine.
IF NOT "%VS80COMNTOOLS%"=="" goto EndIf95
echo ---------------------------------------------------------------------
echo -   Building AGESA ARCH2008 requires Microsoft Visual Studio 2005   -
echo -         Install Visual Studio 2005 and confirm environment        -
echo -         variable VS80COMNTOOLS is present.                        -
echo ---------------------------------------------------------------------
pause
exit
:EndIf95

rem set the Visual Studio 32-bit development environment
call "%VS80COMNTOOLS%vsvars32.bat"

rem Launch the documentation generator if selected by the user
IF NOT "%DoxKey%"=="DoxKey" goto EndIf100
  devenv  %AGESA_ROOT%Proc\AgesaDoc.sln /Rebuild "Release|Win32"
:EndIf100

rem Use the default build configuration if none was specified.
IF NOT "%Configuration%"=="" goto EndIf110
  set Configuration=Debug
:EndIf110

rem Check image bases
IF NOT "%AGESA_B1_ADDRESS%"=="" goto EndIf120
  set AGESA_B1_ADDRESS=0xFFFD0000
:EndIf120
IF NOT "%AGESA_B2_ADDRESS%"=="" goto EndIf130
  set AGESA_B2_ADDRESS=0xFFFA0000
:EndIf130

rem Check if the 'secret' developers' key was used, open an interactive VS window
IF NOT "%DevKey%"=="DevKey"  goto Else140
  set DevKey=
  rem This is the command to open an interactive development window ...
  start /NORMAL devenv.exe  %AGESA_ROOT%Legacy\Proc\%Solution%%SolutionType%Legacy.sln /ProjectConfig "%Configuration%|Win32"
  goto EndIf140
:Else140
  rem Else140 - This is the command to perform a background or automated build ...
  devenv  %AGESA_ROOT%Legacy\Proc\%Solution%%SolutionType%Legacy.sln /Rebuild "%Configuration%|Win32"
:EndIf140
:Exit
exit /B 0
