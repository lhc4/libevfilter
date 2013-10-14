Name:		evfilter
License:	GPL v2 or later
Group:		System/Console
Autoreqprov:	on
Version:	0.1
Release:	1
Source0:	%{name}-%{version}.tar.bz2
BuildRoot:	%{_tmppath}/%{name}-%{version}-build
Summary:	EVent FILTER library and daemon
Provides:	libevfilter evfd
Url:		https://github.com/lhc4/libevfilter

%description
EVFilter provides a daemon that grabs low level events from input devices,
and does modifications on them using a set of prepared filters.

This allows replacing one key with another, emulating a mouse with keyboard,
calibrating touchscreen/tablets and much more.

%prep
%setup -n %{name}

%build
make 

%install
export DESTDIR=$RPM_BUILD_ROOT
make install

%clean
#rm -rf $RPM_BUILD_ROOT

%post

%files
%defattr(-, root, root)
/usr/lib/libevfilter.so
/usr/lib/libevfilter.so.0
/usr/include/evfilter
/usr/sbin/evfd
#%doc COPYING CREDITS HISTORY MANIFEST README
#%doc ...
#%dir ...


%changelog -n evfilter
* Fri Oct 11 2013 - vmarsik@suse.cz
- created RPM
