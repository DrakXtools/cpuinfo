#!/bin/sh
#
#  cpuinfo Makefile (C) 2005-2007 Gwenole Beauchesne
#
-include config.mak

CFLAGS += -Wall

ifeq ($(SRC_PATH),)
SRC_PATH = .
endif

PACKAGE = cpuinfo
ifeq ($(VERSION),)
VERSION := $(shell sed < $(SRC_PATH)/$(PACKAGE).spec -n '/^\%define version[	]*/s///p')
endif
ifeq ($(RELEASE),)
RELEASE := $(shell sed < $(SRC_PATH)/$(PACKAGE).spec -n '/^\%define release[	]*/s///p')
endif
ifeq ($(SVNDATE),)
SVNDATE := $(shell sed < $(SRC_PATH)/$(PACKAGE).spec -n '/^\%define svndate[ 	]*/s///p')
endif
ifeq ($(SVNDATE),)
SVNDATE := $(shell date '+%Y%m%d')
endif
ifeq ($(SNAPSHOT),)
SNAPSHOT := $(shell echo "$(RELEASE)" | grep "^0")
ifeq ($(SNAPSHOT),$(RELEASE))
SNAPSHOT := 1
endif
endif
ifeq ($(SNAPSHOT),1)
VERSION_SUFFIX = -$(SVNDATE)
endif

ifneq ($(DONT_STRIP), yes)
STRIP_OPT = -s
endif

AR		= ar
CC		= gcc
CFLAGS		= -O2
CPPFLAGS	= -I. -I$(SRC_PATH)

libcpuinfo_a = libcpuinfo.a
libcpuinfo_a_SOURCES = cpuinfo-common.c cpuinfo-x86.c cpuinfo-ppc.c cpuinfo-dmi.c
libcpuinfo_a_OBJECTS = $(libcpuinfo_a_SOURCES:%.c=%.o)

cpuinfo_PROGRAM	= cpuinfo
cpuinfo_SOURCES	= cpuinfo.c
cpuinfo_OBJECTS	= $(cpuinfo_SOURCES:%.c=%.o)

TARGETS		= $(cpuinfo_PROGRAM)

archivedir	= files/
SRCARCHIVE	= $(PACKAGE)-$(VERSION)$(VERSION_SUFFIX).tar
FILES		= configure Makefile $(PACKAGE).spec
FILES		+= $(wildcard src/*.c)
FILES		+= $(wildcard src/*.h)

all: $(TARGETS)

clean:
	rm -f $(TARGETS) *.o *.os
	rm -f $(libcpuinfo_a) $(libcpuinfo_a_OBJECTS)

$(cpuinfo_PROGRAM): $(cpuinfo_OBJECTS) $(libcpuinfo_a)
	$(CC) -o $@ $(cpuinfo_OBJECTS) $(libcpuinfo_a) $(LDFLAGS)

install: install.dirs install.bins
install.dirs:
	mkdir -p $(DESTDIR)$(bindir)

install.bins: $(PROGS)
	install -m 755 $(STRIP_OPT) $(PROGS) $(DESTDIR)$(bindir)/

$(libcpuinfo_a): $(libcpuinfo_a_OBJECTS)
	$(AR) rc $@ $(libcpuinfo_a_OBJECTS)

$(archivedir)::
	[ -d $(archivedir) ] || mkdir $(archivedir) > /dev/null 2>&1

tarball:
	$(MAKE) -C $(SRC_PATH) do_tarball
do_tarball: $(archivedir) $(archivedir)$(SRCARCHIVE).bz2

$(archivedir)$(SRCARCHIVE): $(archivedir) $(FILES)
	BUILDDIR=`mktemp -d /tmp/buildXXXXXXXX`						; \
	mkdir -p $$BUILDDIR/$(PACKAGE)-$(VERSION)					; \
	(cd $(SRC_PATH) && tar c $(FILES)) | tar x -C $$BUILDDIR/$(PACKAGE)-$(VERSION)	; \
	[ "$(SNAPSHOT)" = "1" ] && svndate_def="%" || svndate_def="#"			; \
	sed -e "s/^[%#]define svndate.*/$${svndate_def}define svndate $(SVNDATE)/" 	  \
	  < $(SRC_PATH)/$(PACKAGE).spec							  \
	  > $$BUILDDIR/$(PACKAGE)-$(VERSION)/$(PACKAGE).spec				; \
	(cd $$BUILDDIR && tar cvf $(SRCARCHIVE) $(PACKAGE)-$(VERSION))			; \
	mv -f $$BUILDDIR/$(SRCARCHIVE) $(archivedir)					; \
	rm -rf $$BUILDDIR
$(archivedir)$(SRCARCHIVE).bz2: $(archivedir)$(SRCARCHIVE)
	bzip2 -9vf $(archivedir)$(SRCARCHIVE)

RPMBUILD = \
	RPMDIR=`mktemp -d`								; \
	mkdir -p $$RPMDIR/{SPECS,SOURCES,BUILD,RPMS,SRPMS}				; \
	rpmbuild --define "_topdir $$RPMDIR" -ta $(2) $(1) &&				  \
	find $$RPMDIR/ -name *.rpm -exec mv -f {} $(archivedir) \;			; \
	rm -rf $$RPMDIR

localrpm: $(archivedir)$(SRCARCHIVE).bz2
	$(call RPMBUILD,$<)

changelog: ../common/authors.xml
	svn_prefix=`svn info .|sed -n '/^URL *: .*\/svn\/\(.*\)$$/s//\1\//p'`; \
	svn2cl --strip-prefix=$$svn_prefix --authors=../common/authors.xml || :
	svn commit -m "Generated by svn2cl." ChangeLog

%.o: $(SRC_PATH)/src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)
