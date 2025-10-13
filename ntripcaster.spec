%define debuginfo 0

Summary:        BKG professional Ntrip caster - Streaming DGPS data server
Name:           ntripcaster
Version:        2.0.46
Release:        1
Packager:       Dirk Stöcker <ntripcaster@dstoecker.de>
Group:          Productivity/Other
License:        GPL-2.0+
URL:            https://igs.bkg.bund.de/ntrip/bkgcaster
Source:         %{name}-%{version}.tar.bz2
BuildRequires:  openssl-devel
BuildRequires:  systemd
BuildRequires:  systemd-devel
BuildRoot:      %{_tmppath}/%{name}-%{version}-root

%if %debuginfo == 1
%if 0%{?suse_version}
%debug_package
%define cflags -O1 -g
%endif
%else
%define debug_package %{nil}
%define cflags %{nil}
%endif

%description
Ntripcaster is a streaming DGPS data server.
The BKG Professional NtripCaster is meant for service providers handling
several hundred incoming streams in support of thousand or more simultaneously
listening clients. The BKG Professional NtripCaster software follows NTRIP
Version 2. The main advantages over NTRIP Version 1.0 include:

* Full HTTP compatibility, cleared and fixed design problems and protocol
  violations
* Replaced non standard directives
* Adds chunked transfer encoding
* Improves header records
* Provides for sourcetable filtering
* Optional support of RTSP/RTP and UDP

%prep
%setup

%build
export CFLAGS="%{optflags} %{cflags}"
if [ ! -f configure ]; then
  ./autogen.sh --enable-fsstd
else
  %configure --enable-fsstd
fi
make

%install
make DESTDIR=%{buildroot} install

# copy the configuration files to be the right name so that it works without
# _having_ to change them (though they should), and leave the defaults so
# they don't screw with them
cp %{buildroot}%{_sysconfdir}/%{name}/%{name}.conf.dist %{buildroot}%{_sysconfdir}/%{name}/%{name}.conf
cp %{buildroot}%{_sysconfdir}/%{name}/groups.aut.dist %{buildroot}%{_sysconfdir}/%{name}/groups.aut
cp %{buildroot}%{_sysconfdir}/%{name}/sourcemounts.aut.dist %{buildroot}%{_sysconfdir}/%{name}/sourcemounts.aut
cp %{buildroot}%{_sysconfdir}/%{name}/clientmounts.aut.dist %{buildroot}%{_sysconfdir}/%{name}/clientmounts.aut
cp %{buildroot}%{_sysconfdir}/%{name}/users.aut.dist %{buildroot}%{_sysconfdir}/%{name}/users.aut
cp %{buildroot}%{_sysconfdir}/%{name}/sourcetable.dat.dist %{buildroot}%{_sysconfdir}/%{name}/sourcetable.dat
install -d -m 755 %{buildroot}%{_sysconfdir}/%{name}/templates
cp -r templates/*.html %{buildroot}%{_sysconfdir}/%{name}/templates/
cp -r templates/*.txt %{buildroot}%{_sysconfdir}/%{name}/templates/
cp -r templates/*.ico %{buildroot}%{_sysconfdir}/%{name}/templates/
install -d -m 755 %{buildroot}%{_unitdir}
install -m 644 scripts/%{name}.service %{buildroot}%{_unitdir}/%{name}.service
ln -s service %{buildroot}/%{_sbindir}/rc%{name}
install -d -m 755 %{buildroot}%{_tmpfilesdir}
echo "d /run/%{name} 0755 root root -" >%{buildroot}%{_tmpfilesdir}/%{name}.conf

%pre %service_add_pre %{name}.service
%post %service_add_post %{name}.service
%preun %service_del_preun %{name}.service
%postun %service_del_postun %{name}.service

%files
%defattr(-,root,root)
%doc CHANGES
%doc COPYING
%doc FAQ
%doc README
%doc %{name}_manual.html
%exclude %{_bindir}/%{name}
%exclude %{_bindir}/casterwatch
%{_sbindir}/ntripdaemon
/usr/share/%{name}/
%{_sysconfdir}/%{name}/groups.aut.dist
%{_sysconfdir}/%{name}/sourcemounts.aut.dist
%{_sysconfdir}/%{name}/clientmounts.aut.dist
%{_sysconfdir}/%{name}/users.aut.dist
%{_sysconfdir}/%{name}/%{name}.conf.dist
%{_sysconfdir}/%{name}/sourcetable.dat.dist
%dir %{_sysconfdir}/%{name}/templates
%config(noreplace) %{_sysconfdir}/%{name}/templates/*.html
%config(noreplace) %{_sysconfdir}/%{name}/templates/*.txt
%config(noreplace) %{_sysconfdir}/%{name}/templates/*.ico
%config(noreplace) %{_sysconfdir}/%{name}/groups.aut
%config(noreplace) %{_sysconfdir}/%{name}/sourcemounts.aut
%config(noreplace) %{_sysconfdir}/%{name}/clientmounts.aut
%config(noreplace) %{_sysconfdir}/%{name}/users.aut
%config(noreplace) %{_sysconfdir}/%{name}/%{name}.conf
%config(noreplace) %{_sysconfdir}/%{name}/sourcetable.dat
%dir %{_sysconfdir}/%{name}
%dir /var/log/%{name}
%{_unitdir}/%{name}.service
%{_sbindir}/rc%{name}
%{_tmpfilesdir}/%{name}.conf

%changelog
* Thu Apr 8 2010 Dirk Stöcker <stoecker@alberding.eu> 2.0.10
- update for 2.0.10

* Tue Mar 21 2000 Jeremy Katz <katzj@ntripcaster.org>
- clean up the spec file a little

* Thu Dec 9 1999 Jeremy Katz <katzj@ntripcaster.org>
- First official rpm build, using 2.0.0-beta
