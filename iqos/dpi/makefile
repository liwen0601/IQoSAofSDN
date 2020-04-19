#############################################################################
# Author: Wen Li
# Date  : 3/25/2020
# Description: makefile for entry of module of project
##############################################################################
export CC = gcc 
export LD = gcc

export ROOT_PATH = $(PWD)
export LIB_PATH = $(ROOT_PATH)/lib

export CFLAGS     = -std=c++11 -g -O3
export LD_CFLAGES = -pthread -lpthread -lstdc++

SUBDIRS = lib dpiClient dpiDeamon

$(SUBDIRS):init
	for subdir in $(SUBDIRS);\
	do $(MAKE) -C $$subdir;\
	done

init:
	if [ ! -d $(LIB_PATH) ]; then	mkdir $(LIB_PATH); fi
	echo begin compile $(PWD)

.PHONY : clean
clean:
	for dir in $(SUBDIRS);\
	do $(MAKE) -C $$dir clean;\
	done
