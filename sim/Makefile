# Possible Targets:	all clean Debug cleanDebug Release cleanRelease

##############################################################################################
# Settings
#

# General settings
	# See $(GFXLIB)/tools/gmake_scripts/readme.txt for the list of variables
	OPT_OS					= linux
	OPT_LINK_OPTIMIZE		= yes
	# Change this next setting (or add the explicit compiler flags) if you are not compiling for x86 linux
	OPT_CPU					= x64

# uGFX settings
	# See $(GFXLIB)/tools/gmake_scripts/library_ugfx.mk for the list of variables
	GFXLIB					= ../ugfx
	GFXBOARD				= Linux-SDL
	# GFXDEMO					= modules/gwin/widgets

# Linux settings
	# See $(GFXLIB)/tools/gmake_scripts/os_linux.mk for the list of variables

##############################################################################################
# Set these for your project
#

ARCH     =
SRCFLAGS = -g -Og
CFLAGS   = `sdl2-config --libs --cflags`
CXXFLAGS =
ASFLAGS  =
# LDFLAGS  = -L/usr/lib32
LDFLAGS  = 

SRC      =
SRC     += ../ui.c ../app_disp.c ../app_thread.c ../app_settings.c
SRC 	+=	main.c
OBJS     =
DEFS     =
LIBS     = -lm
INCPATH  = 
LIBPATH  =

##############################################################################################
# These should be at the end
#

include $(GFXLIB)/tools/gmake_scripts/library_ugfx.mk
include $(GFXLIB)/tools/gmake_scripts/os_$(OPT_OS).mk
include $(GFXLIB)/tools/gmake_scripts/compiler_gcc.mk

# *** EOF ***
