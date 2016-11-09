########################
AM_CFLAGS= -I./include 
ACLOCAL_AMFLAGS= -I m4 
AUTOMAKE_OPTIONS = subdir-objects 
########################
include_HEADERS =  
dorothy_SOURCES =  ./src/main.cpp ./src/global.cpp ./src/os.cpp ./src/dorothy.cpp
dorothy_LIBTOOLFLAGS = 
dorothy_CPPFLAGS = -DGIT_VERSION="\"$(VERSION)\""  -std=c++11
dorothy_LDADD = 
dorothy_LDFLAGS = 
bin_PROGRAMS =  dorothy
lib_LIBRARIES = 
lib_LTLIBRARIES = 
########################
EXTRA_DIST = $(srcdir)/version.txt package_info.txt Makefile.txt configure.txt autoreconf.bsh   src/os.hpp src/main.hpp src/dorothy.cpp src/utf8.h src/main.cpp src/utf8/unchecked.h src/utf8/core.h src/utf8/checked.h src/global.hpp src/dorothy.hpp src/os.cpp src/global.cpp 
########################
GET_GIT_VERSION = git --git-dir=$(srcdir)/.git describe --always --tags
VERSION = $(shell [[ -d .git ]] && $(GET_GIT_VERSION) || ( [[ -r $(srcdir)/version.txt ]] && cat $(srcdir)/version.txt || echo  v1.0  ) )
PACKAGE_STRING = $(PACKAGE_NAME) $(VERSION)
PACKAGE_VERSION = $(VERSION)
$(srcdir)/version.txt: .FORCE
	[[ -d .git ]] && ( $(GET_GIT_VERSION) > $(srcdir)/version.txt ) || echo 

.FORCE:
########################
########################
Auto_Tool:
	echo "Nic do zrobienia..."
########################
########################
