
BUILD_PATH=$(PWD)
MAKEFILE_PATH=$(shell dirname $(MAKEFILE_LIST))
SOURCE_FILES=$(MAKEFILE_PATH)/db_test.cpp
DEPEND_LIB=-lqlevel -lpthread
#use LD_FLAGS to specify runtime library load path
LD_FLAGS=-Wl,-rpath,$(BUILD_PATH)/lib
GXX=g++

default:all

all:
	mkdir -p $(BUILD_PATH)/db_test
	$(GXX) $(SOURCE_FILES) -o dbtest \
	-I$(BUILD_PATH)/include \
	-L$(BUILD_PATH)/lib $(DEPEND_LIB) $(LD_FLAGS)
	mv $(BUILD_PATH)/dbtest $(BUILD_PATH)/db_test

