# Project: dungeon_gen AI-lib
# Makefile configurations

DBGFLAGS	=	-O0 -g -D__DEBUG__
#DBGFLAGS	=	-O3 -g
#General flags
CFLAGS		+=	-Wall -shared $(DBGFLAGS) -Wextra
LDFLAGS		+=	-ldarnit -Wl,-soname,ai_lib.so

#Makefile tools
RM		=	rm -Rf
MKDIR		=	mkdir -p


ifeq ($(strip $(OS)), Windows_NT)
	#Windows specifics
	PLATFORM=	win32-i386
else 
ifeq ($(BUILDFOR), WIN32)
	#Windows specifics, for cross compiling to windows
	PLATFORM=	win32-i386
	CC	=	i586-mingw32msvc-gcc
	STRIP	=	i586-mingw32msvc-strip
	AR	=	i586-mingw32msvc-ar
else
ifeq ($(BUILDFOR), WIN64)
	#Windows64 specifics, for cross compiling to windows
	PLATFORM=	win32-amd64
	CC	=	x86_64-w64-mingw32-gcc
	STRIP	=	x86_64-w64-mingw32-strip
	AR	=	x86_64-w64-mingw32-ar
else
ifeq ($(strip $(SBOX_UNAME_MACHINE)), arm)
	#Maemo specifics
	PLATFORM=	linux-armv7
	CFLAGS	+=	-fPIC
else
ifneq (,$(findstring -DPANDORA, $(CFLAGS)))
	#Pandora specifics
	PLATFORM=	linux-armv7
	CFLAGS	+=	-fPIC
else
ifneq (,$(findstring -DGCW_ZERO, $(CFLAGS)))
	#GCWZero specifics
	PLATFORM=	gcwzero-mips32
	CFLAGS	+=	-fPIC
else
ifeq ($(BUILDFOR), RPI)
	#RaspberryPi specifics
	PLATFORM=	linux-armv6hf
	CFLAGS	+=	-fPIC
else
	#Linux defaults
	UNAME	=	`uname -m`
ifneq (,$(findstring x86_64, $(UNAME)))
	UNAME	=	i386
endif
	PLATFORM=	linux-$(UNAME)
	CFLAGS	+=	-fPIC
endif
endif
endif
endif
endif
endif
endif

LIB	=	$(TOPDIR)/bin/$(PLATFORM).ai
CFLAGS	+=	-DPLATFORM=\"$(PLATFORM)\"
