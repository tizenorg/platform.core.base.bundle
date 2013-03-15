Name:       bundle
Summary:    String key-val dictionary ADT
Version:    0.1.30
Release:    1
Group:      System/Libraries
License:    Apache License, Version 2.0
Source0:    bundle-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(dlog)
%description
Simple string key-val dictionary ADT

%package devel
Summary:    String key-val dictionary ADT (devel)
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Simple string key-val dictionary ADT (devel)


%prep
%setup -q -n %{name}-%{version}


%build
%cmake .
make %{?jobs:-j%jobs}

%install
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%manifest bundle.manifest
%defattr(-,root,root,-)
%{_libdir}/libbundle.so.*


%files devel
%defattr(-,root,root,-)
%{_includedir}/bundle.h
%{_includedir}/SLP_bundle_PG.h
%{_libdir}/pkgconfig/bundle.pc
%{_libdir}/libbundle.so

