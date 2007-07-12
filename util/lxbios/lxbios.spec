##
# $Id: lxbios.spec,v 1.3 2006/01/24 00:25:40 dsp_llnl Exp $
##

Name:    lxbios
Version: 2.0.1
Release: 0

Summary: LinuxBIOS utility program
Group:   System Environment/Base
License: GPL
Provides: lxbios

BuildRoot: %{_tmppath}/%{name}-%{version}

Source0: %{name}-%{version}.tgz

%description
lxbios is a utility for reading/writing LinuxBIOS parameters and displaying
information from the LinuxBIOS table.

At boot time, LinuxBIOS places a table (known as the LinuxBIOS table) in low
physical memory.  The contents of this table are preserved even after
LinuxBIOS transfers control to the kernel and the kernel initializes itself.
The LinuxBIOS table contains various system information such as the type of
mainboard in use.  It also specifies locations in the CMOS (nonvolatile RAM)
where the LinuxBIOS parameters are stored.

%prep
%setup -n %{name}-%{version}

%build
make

%install
rm -rf "$RPM_BUILD_ROOT"
mkdir -p "$RPM_BUILD_ROOT/usr/bin"
mkdir -p "$RPM_BUILD_ROOT/usr/man/man1"
cp lxbios "$RPM_BUILD_ROOT/usr/bin"
cp lxbios.1.gz $RPM_BUILD_ROOT/usr/man/man1

%clean
rm -rf "$RPM_BUILD_ROOT"

%files
%defattr(-,root,root,0755)
%doc ChangeLog README
%doc README
/usr/bin/lxbios
/usr/man/man1/lxbios.1.gz
