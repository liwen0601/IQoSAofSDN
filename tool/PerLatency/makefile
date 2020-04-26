#############################################################################
# Author: Wen Li
# Date  : 1/31/2020
# Description: makefile for entry of module of project
##############################################################################
export CC = gcc 
export LD = gcc

CUR_PATH = $(PWD)
HEADER   = $(CUR_PATH)/common

export BIN_PATH = $(CUR_PATH)/bin

export CFLAGS     = -I$(HEADER) -O3
export LD_CFLAGES =

SUBDIRS = client server

$(SUBDIRS):init
	for dir in $(SUBDIRS);\
		do $(MAKE) -C $$dir;\
	done

init:
	@if [ ! -d "bin" ]; then	mkdir bin; fi
	@echo begin compile $(PWD)

.PHONY : clean
clean:
	for dir in $(SUBDIRS);\
		do $(MAKE) -C $$dir clean;\
	done
	@rm -rf lib