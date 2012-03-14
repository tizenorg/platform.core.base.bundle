Name:       bundle
Summary:    String key-val dictionary ADT
Version:    0.1.16
Release:    1.1
Group:      TO_BE/FILLED_IN
License:    TO BE FILLED IN
Source0:    bundle-%{version}.tar.bz2
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
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
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}


make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install




%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig





%files
%{_libdir}/libbundle.so.*


%files devel
%{_includedir}/bundle.h
%{_includedir}/SLP_bundle_PG.h
%{_libdir}/pkgconfig/bundle.pc
%{_libdir}/libbundle.so

