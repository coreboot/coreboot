##

Name:    nvramtool
Version: 2.1
Release: 0

Summary: coreboot utility program
Group:   System Environment/Base
License: GPL
Provides: nvramtool

BuildRoot: %{_tmppath}/%{name}-%{version}

Source0: %{name}-%{version}.tgz

%description
nvramtool is a utility for reading/writing coreboot parameters and displaying
information from the coreboot table.

At boot time, coreboot places a table (known as the coreboot table) in low
physical memory.  The contents of this table are preserved even after
coreboot transfers control to the kernel and the kernel initializes itself.
The coreboot table contains various system information such as the type of
mainboard in use.  It also specifies locations in the CMOS (nonvolatile RAM)
where the coreboot parameters are stored.

%prep
%setup -n %{name}-%{version}

%build
make

%install
rm -rf "$RPM_BUILD_ROOT"
mkdir -p "$RPM_BUILD_ROOT/usr/bin"
mkdir -p "$RPM_BUILD_ROOT/usr/man/man8"
cp nvramtool "$RPM_BUILD_ROOT/usr/bin"
cp nvramtool.8.gz $RPM_BUILD_ROOT/usr/man/man8

%clean
rm -rf "$RPM_BUILD_ROOT"

%files
%defattr(-,root,root,0755)
%doc ChangeLog README
%doc README
/usr/bin/nvramtool
/usr/man/man8/nvramtool.8.gz
