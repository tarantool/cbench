Name: tarantool-cbench
Version: 1.0.0
Release: 1%{?dist}
Summary: Low-level Tarantool benchmark
Group: Applications/Databases
License: BSD
URL: https://github.com/tarantool/cbench
Source0: https://github.com/tarantool/%{name}/archive/%{version}/%{name}-%{version}.tar.gz
BuildRequires: cmake >= 2.8
BuildRequires: gcc >= 4.5
BuildRequires: tarantool-devel >= 1.6.8.0
BuildRequires: msgpuck-devel >= 1.0.0
Requires: tarantool >= 1.6.8.0

%description
Tarantool C Bench is a simple tool to benchmark Tarantool internal API.
The tool is used internally by Tarantool team to check for performance
regressions during development cycle. All workloads are written in C++.
Lua (FFI) is only used to load & run workloads and display results.

%prep
%setup -q -n %{name}-%{version}

%build
%cmake . -DCMAKE_BUILD_TYPE=RelWithDebInfo
make %{?_smp_mflags}

%install
%make_install

%files
%{_libdir}/tarantool/*/
%{_datarootdir}/tarantool/*/
%doc README.md
%{!?_licensedir:%global license %doc}
%license LICENSE AUTHORS

%changelog
* Wed Feb 17 2016 Roman Tsisyk <roman@tarantool.org> 1.0.0-1
- Initial version of the RPM spec
