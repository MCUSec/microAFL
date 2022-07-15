# 
# Makefile for building the JLink FlashDownload sample utility. 
#
# Invoke 'make' like this:
# 
#     make -f Samples_FlashDownload release   # builds only the release version
#     make -f Samples_FlashDownload debug     # builds only the debug version
#     make -f Samples_FlashDownload all       # builds release and debug versions
#     make -f Samples_FlashDownload clean     # removes all the generated binaries
#
TARGET_NAME = x86_64
#
# Name of the generated executable
#
PROG_DIR = ETMFuzz
#
# List of object files to link
#
OBJECTS  = afl-fuzz.o bridge.o decoder.o etm_decoder.o Init.o trace.o 
#
# List of libraries to link
#
LIBS     = pthread rt dl m  

#
# Global variables
#
PROG_NAME    = ETMFuzz
RELEASE_DIR  = Output/Release/$(PROG_DIR)_int
DEBUG_DIR    = Output/Debug/$(PROG_DIR)_int
RELEASE_PROG = ./$(PROG_NAME)
DEBUG_PROG   = ./$(PROG_NAME)_Debug

#
# Preprocessor, compiler and linker flags
#
CPPFLAGS += -I./Samples_$(PROG_DIR)_Src -I./Samples_Inc \
            -D_GNU_SOURCE

#
# Setup compiler flags (C & C++)
#
CPPFLAGS += -fPIC -c -fvisibility=hidden -Wall \
            -fno-strict-aliasing \
            -Werror=strict-aliasing
ifeq ($(findstring release,$(MAKECMDGOALS)),release)
	CPPFLAGS += -O3
else
	CPPFLAGS += -O0
endif

CFLAGS   += -fPIC -c -fvisibility=hidden -Wall \
            -fsigned-char \
            -Wno-unused-parameter \
            -Wno-unused-variable \
            -Wno-pointer-sign \
            -Wno-unknown-pragmas \
            -Wno-unused-value \
            -Wno-unused-function \
            -Wno-switch \
            -Wno-switch-enum \
            -Wno-missing-braces \
			-Wno-unused-but-set-variable \
			-Wno-format-security \
            -Wpointer-to-int-cast \
            -fno-strict-aliasing \
            -Werror=strict-aliasing \
            #-Wno-int-to-pointer-cast -Wextra
ifeq ($(findstring release,$(MAKECMDGOALS)),release)
	CFLAGS   += -O3
else
	CFLAGS   += -O0
endif

#
# Handy names for utilities used in this makefile
#
RM    = rm -fr
MKDIR = mkdir -p
ECHO  = echo
#
# Make sure that unresolved symbols inside our DLL are reported as error
# while unresolved symbols in other shared libs, referenced by our DLL are not
#
LDFLAGS += -Wl,--unresolved-symbols=ignore-in-shared-libs
#
# Configure flags and tools for ARM targets (DigiEL).
#
ifeq ($(TARGET_NAME),arm_uclibc)
	DIGIEL_ROOT  = /usr/local/DigiEL-5.2
  CPPFLAGS    += -DNO_SETSCHEDPRIO_SUPPORT -I$(DIGIEL_ROOT)/usr/include
	LDFLAGS     += -L$(DIGIEL_ROOT)/usr/lib  
  CC           = $(DIGIEL_ROOT)/usr/bin/arm-linux-gcc
endif
#                                                       
# Configure flags and tools for ARM targets (Raspberry Pi).
#
ifeq ($(TARGET_NAME),arm_glibc)
	RPITOOLS_ROOT  = $(HOME)/Tool/raspberrypi/rpitools
  CPPFLAGS      += -I$(RPITOOLS_ROOT)/arm-linux-gnueabihf/include
	LDFLAGS       += -L$(RPITOOLS_ROOT)/arm-linux-gnueabihf/lib  
  CC             = $(RPITOOLS_ROOT)/bin/arm-linux-gnueabihf-gcc
endif

#
# Configure flags and tools for 32-bit targets.
#
ifeq ($(TARGET_NAME),i386)
	CFLAGS  += -m32
	LDFLAGS += -m32
endif

LDFLAGS += -L./Samples_Inc
#
# Switch for verbose outputs. Invoke make with "V=" on the command line 
# to see more info about the progress of the build process
#
V ?= @  

#
# Tells make where to look for source files
#
VPATH += Samples_$(PROG_DIR)_Src
VPATH += Samples_Inc

#
# These are the build goals
#
all: release debug

clean:
	$(V)$(ECHO) "CLEAN"
	$(V)$(RM) $(RELEASE_DIR)/* $(DEBUG_DIR)/*
	$(V)$(RM) $(RELEASE_PROG) $(DEBUG_PROG)
	$(V)$(RM) Output

release: $(RELEASE_PROG) 

debug: $(DEBUG_PROG) 

#
# Figure out what kind of goal we should build
#
DEBUG_GOAL     = 0
RELEASE_GOAL   = 0
ifeq ($(strip $(MAKECMDGOALS)),)
	DEBUG_GOAL   = 1
	RELEASE_GOAL = 1
endif
ifeq ($(findstring all,$(MAKECMDGOALS)),all)
	DEBUG_GOAL   = 1
	RELEASE_GOAL = 1
endif
ifeq ($(findstring release,$(MAKECMDGOALS)),release)
	RELEASE_GOAL = 1
endif
ifeq ($(findstring debug,$(MAKECMDGOALS)),debug)
	DEBUG_GOAL   = 1
endif

#
# Include file dependencies
#
ifeq ($(RELEASE_GOAL),1) 
	-include $(addprefix $(RELEASE_DIR)/,$(subst .o,.d,$(OBJECTS)))
endif
ifeq ($(DEBUG_GOAL),1) 
	-include $(addprefix $(DEBUG_DIR)/,$(subst .o,.d,$(OBJECTS)))
endif

#
# Rules to generate dependencies
#
$(RELEASE_DIR)/%.d: %.c	
	$(V)$(ECHO) "DEP $@"
	$(V)$(MKDIR) $(RELEASE_DIR)
	$(V)$(COMPILE.c) -MM -MF $@ $<

$(DEBUG_DIR)/%.d: %.c	
	$(V)$(ECHO) "DEP $@"
	$(V)$(MKDIR) $(DEBUG_DIR)
	$(V)$(COMPILE.c) -D_DEBUG -MM -MT $(subst .d,.o,$@) -MF $@ $<

#
# Rules to generate the object files
#
$(RELEASE_DIR)/%.o: %.c	
	$(V)$(ECHO) "CC $@"
	$(V)$(COMPILE.c) $< -o $@

$(DEBUG_DIR)/%.o: %.c
	$(V)$(ECHO) "CC $@"
	$(V)$(COMPILE.c) -D_DEBUG $< -g -o $@

#
# Rules to generate the executables
# --strip-all: Remove all symbol information from output file.
#
$(RELEASE_PROG): $(addprefix $(RELEASE_DIR)/, $(OBJECTS))
	$(V)$(ECHO) "EXE $@"
	$(V)$(LINK.o) $^ -o $@ -Wl,--strip-all $(addprefix -l,$(LIBS)) -L${HOME}/microAFL/ETMFuzz_Src/ -ljlinkarm -Wl,-rpath=${HOME}/microAFL/ETMFuzz_Src/

$(DEBUG_PROG): $(addprefix $(DEBUG_DIR)/, $(OBJECTS))
	$(V)$(ECHO) "EXE $@"
	$(V)$(LINK.o) $^ -o $@ $(addprefix -l,$(LIBS)) -L${HOME}/microAFL/ETMFuzz_Src/ -ljlinkarm -Wl,-rpath=${HOME}/microAFL/ETMFuzz_Src/

.PHONY: all clean release debug 

