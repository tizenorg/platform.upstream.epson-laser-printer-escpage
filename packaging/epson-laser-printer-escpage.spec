# epson-laser-printer-escpage.spec.in -- an rpm spec file templete
# Epson Inkjet Printer Driver (ESC/Page) for Linux

#%define _unpackaged_files_terminate_build 0
%define cupsfilterdir   /usr/lib/cups/filter
%define cupsppddir      /opt/etc/cups/ppd/Epson

Name: epson-laser-printer-escpage
Version: 1.0.1
Release: 8
Source0: %{name}-%{version}.tar.gz
License: LGPL
Vendor: Seiko Epson Corporation
Packager: Seiko Epson Corporation <linux-printer@epson.jp>
BuildRoot: %{_tmppath}/%{name}-%{version}
BuildRequires: cups-devel
Requires: cups
Group: Applications/System
Summary: Epson Laser Printer Driver (ESC/Page) for Linux
# Bug fix for Tizen
# Structure values are unused in Tizen
Patch0: tizen_disable_unused_value.patch
Patch1: tizen_add_job_media_progress.patch
Patch2: tizen_report_page_info.patch
Patch3: tizen_fix_ignore_sigpipe.patch

%description
This software is a filter program used with Common UNIX Printing
System (CUPS) from the Linux. This can supply the high quality print
with Seiko Epson laser Printers.

This product supports only EPSON ESC/Page printers. This package can be
used for all EPSON ESC/Page printers.

%prep
%setup -q
%patch0
%patch1 -p1
%patch2 -p1
%patch3 -p1

%build
%configure \
	--with-cupsfilterdir=%{cupsfilterdir} \
	--with-cupsppddir=%{cupsppddir}
make

%install
rm -rf ${RPM_BUILD_ROOT}
make install-strip DESTDIR=${RPM_BUILD_ROOT}

mkdir -p %{buildroot}/usr/share/license
cp %{_builddir}/%{buildsubdir}/COPYING %{buildroot}/usr/share/license/%{name}

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%clean
make clean
rm -rf ${RPM_BUILD_ROOT}

%files
%manifest epson-laser-printer-escpage.manifest
%defattr(-,root,root)
#%doc AUTHORS COPYING NEWS README README.ja
/usr/share/license/%{name}
%{cupsfilterdir}/*
%attr(644,-,-) %{_libdir}/libescpage*.so*
%exclude %{_libdir}/libescpage.a
%exclude %{_libdir}/libescpage.la
%exclude %{cupsppddir}
