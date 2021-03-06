%define name	cpuinfo
%define version	1.0
%define release	0.2
#define svndate	DATE

# Define to build shared libraries
%define build_shared 0
%{expand: %{?_with_shared:		%%global build_shared 1}}
%{expand: %{?_without_shared:	%%global build_shared 0}}

# Define to build perl bindings
%define build_perl 0
%{expand: %{?_with_perl:		%%global build_perl 1}}
%{expand: %{?_without_perl:		%%global build_perl 0}}

%define perlarchlib %(perl -e 'use Config; print $Config{sitearch}')

Summary:	Print CPU information
Name:		%{name}
Version:	%{version}
Release:	%{release}
Source0:	%{name}-%{version}%{?svndate:-%{svndate}}.tar.bz2
License:	GPL/LGPL
Group:		System/Kernel and hardware
Url:		http://gwenole.beauchesne/projects/cpuinfo
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-buildroot

%description
cpuinfo gathers some interesting information concerning your CPU. In
particular, it determines useful processor features and cache
hierarchy along with the usual brand and model names.

%package devel
Summary:	Development files for cpuinfo
Group:		Development/C

%description devel
This package contains headers and libraries needed to use cpuinfo
processor characterisation features.

%package -n perl-Cpuinfo
Summary:	Perl bindings for cpuinfo
Group:		Development/Perl

%description -n perl-Cpuinfo
Provides a Perl API to the cpuinfo library.

%prep
%setup -q

%build
mkdir objs
pushd objs
../configure \
	--install-sdk \
%if %{build_shared}
	--enable-shared \
%endif
%if %{build_perl}
	--enable-perl \
%endif
	--prefix=%{_prefix} --libdir=%{_libdir}
make
popd

%install
rm -rf $RPM_BUILD_ROOT

make -C objs install DESTDIR=$RPM_BUILD_ROOT

# nuke unpackaged files
find $RPM_BUILD_ROOT -name cpuinfo.pl -exec rm -f {} \;
find $RPM_BUILD_ROOT -name perllocal.pod -exec rm -f {} \;
find $RPM_BUILD_ROOT -name .packlist -exec rm -f {} \;

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README COPYING NEWS
%{_bindir}/cpuinfo
%if %{build_shared}
%{_libdir}/libcpuinfo.so.*
%endif

%files devel
%defattr(-,root,root)
%{_includedir}/cpuinfo.h
%{_libdir}/libcpuinfo.a
%if %{build_shared}
%{_libdir}/libcpuinfo.so
%endif

%if %{build_perl}
%files -n perl-Cpuinfo
%defattr(-,root,root)
%doc src/bindings/perl/cpuinfo.pl
%{perlarchlib}/Cpuinfo.pm
%dir %{perlarchlib}/auto/Cpuinfo
%{perlarchlib}/auto/Cpuinfo/*
%endif

%changelog
* Sun Jul 15 2007 Gwenole Beauchesne <gb.public@free.fr> 1.0-0.2
- relicense the library under LGPL
- add Perl bindings
- add new ppc CPUs (PA6T-1682M, PowerPC 750CL and 970GX)
- fix detection of VIA processors (C3, C7)
- fix detection of early AMD processors (K5, K6)
- fix detection of early Intel processors (Pentium III)
- fix caches detection for HP-UX/ia64 (use `machinfo')
- fix caches detection for some VIA C3 & AMD Duron (errata)
- add more ia64 standard feature flags (simd, popcount)
- add more x86 feature flags (3dnow, sse4, popcnt, abm, power, nx)
- add more ppc feature flags (gfxopt, mfcrf, popcntb, fprnd, mfpgpr)

* Sun Apr 15 2007 Gwenole Beauchesne <gb.public@free.fr> 1.0-0.1
- initial packaging
