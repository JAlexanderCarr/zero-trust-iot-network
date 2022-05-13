# Standard Makefile for a C++ project that compiles each .cc file into a .o
# file, and then links .o files together to produce an executable.

# Specify the name of the folder where all the source files will go
SDIR := ./src

# Names of .cc files that have a main() function
TARGETS = server mqtt_client coap_client

# Names of other .cc files
CXXFILES = common iot

# Specify the name of the folder where all output will go
ODIR := ./build
EDIR := ./bin
output_folder := $(shell mkdir -p $(ODIR))
exe_folder := $(shell mkdir -p $(EDIR))

# Generate the names of the .o files and .exe files that we will be creating.
# Note that we name all .o files explicitly, so that we can add them to the
# .PRECIOUS target, which prevents them from being auto-removed.
# NB: These get linked into every executable
COMMONOFILES = $(patsubst %, $(ODIR)/%.o, $(CXXFILES))
ALLOFILES    = $(patsubst %, $(ODIR)/%.o, $(CXXFILES) $(TARGETS))
EXEFILES     = $(patsubst %, $(EDIR)/%.exe, $(TARGETS))

# Generate the names of the dependency files that G++ will generate, so that we
# can include them later in this makefile
DFILES = $(patsubst %.o, %.d, $(ALLOFILES))

# Basic tool configuration for GCC/G++.  We will create debug symbols, enable
# optimizations, and generate dependency information on-the-fly
pkgconfig = $(shell pkg-config $(1) $(2))
CXX       = g++
LD        = g++
CXXFLAGS  = -MMD -O3 -ggdb -std=c++17 -Wall $(call pkgconfig,--cflags,libcoap-3) -Werror
LDFLAGS   = $(call pkgconfig,--libs,libcoap-3) -l paho-mqttpp3 -l paho-mqtt3cs -l mraa

# Build 'all' by default, and don't clobber .o files after each build
.DEFAULT_GOAL = all
.PRECIOUS: $(ALLOFILES)
.PHONY: all clean

# Goal is to build all executables
all: $(EXEFILES)

# Rules for building object files
$(ODIR)/%.o: $(SDIR)/%.cc
	@echo "[CXX] $< --> $@"
	@$(CXX) $< -o $@ -c $(CXXFLAGS)

# Rules for building executables... we assume an executable uses *all* of the 
# common OFILES
$(EDIR)/%.exe: $(ODIR)/%.o $(COMMONOFILES)
	@echo "[LD] $^ --> $@"
	@$(CXX) $^ -o $@ $(LDFLAGS)

# clean by clobbering the build folder
clean:
	@echo Cleaning up...
	@rm -rf $(ODIR)
	@rm -rf $(EDIR)

# Include any dependencies we generated previously
-include $(DFILES)
