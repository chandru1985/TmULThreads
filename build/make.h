#!/bin/bash
.SILENT:

#(LINUX,WINDOWS,NONE) /*define OS your are Running */  /*Windows not supported now*/
TARGET_OS = LINUX
#(SFS, NORMAL)
SCHED = NORMAL

ifeq ($(TARGET_OS), LINUX)
COMPILATION_SWITCH += -DLINUX_WANTED
endif

ifeq ($(TARGET_OS), WINDOWS)
COMPILATION_SWITCH += -DWIN32_WANTED
endif

ifeq ($(SCHED), SFS)
COMPILATION_SWITCH += -DSFS_WANTED
endif

INC_DIR = $(CODE_DIR)/inc
SRC_DIR = $(CODE_DIR)/src
OBJ_DIR = $(CODE_DIR)/obj
LIB_DIR = $(CODE_DIR)/lib
CC   = echo $(notdir $<); gcc 
CXXFLAGS = $(CXXINCS)  
RM = rm -f
DEBUG_FLAGS = -g -DDEBUG
LD = ld
LD_FLAGS = -r
VERSION=1.0
CODE_COV = -fprofile-arcs -ftest-converage
CFLAGS = $(DEBUG_FLAGS) \
	-Wall \
	-rdynamic 
