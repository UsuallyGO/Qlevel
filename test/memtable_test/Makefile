
BUILD_PATH=$(PWD)
MAKEFILE_PATH=$(shell dirname $(MAKEFILE_LIST))
SOURCE_FILES=$(MAKEFILE_PATH)/memtable_test.cpp
DEPEND_LIB=-lqlevel
GXX=g++

default:all

all:
	mkdir -p $(BUILD_PATH)/memtable_test
	$(GXX) 	$(SOURCE_FILES) -o mmtabletest \
	-I$(BUILD_PATH)/include -I$(MAKEFILE_PATH)/../../ \
	-L$(BUILD_PATH)/lib $(DEPEND_LIB)
	mv mmtabletest $(BUILD_PATH)/memtable_test

