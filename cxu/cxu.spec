Name:		cxu	
Version:	%{fixver}
Release:	%{repver}%{?dist}
Summary:	C++ adapter for Unix

#Group:		
License:	zetasys.net
#URL:		
Source0:	cxu-%{fixver}-%{repver}.tar.gz
Source1:	zmk-%{fixver}-%{repver}.tar.gz

#BuildRequires:	
#Requires:	


%description
C++ adapter for Unix

%prep
%setup -b 1 -T -D -a 0
%setup -b 1 -T -D -a 1

%build
make 

%install
mkdir -p %{buildroot}/usr/lib 
mkdir -p %{buildroot}/usr/include 
DESTDIR=%{buildroot} make install

%files

%doc


%changelog

