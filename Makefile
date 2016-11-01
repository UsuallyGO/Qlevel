#Qlevel manually written makefile...

#makefile absolute path
MAKEFILE_PATH=$(shell dirname $(MAKEFILE_LIST))

ifeq ($(BUILD_STATIC), yes)
BUILD_STATIC_SHARED=--build_static
else
endif

ifeq ($(INSTALL_PREFIX), )
PATH_INSTALL_PREFIX=$(PWD)
else
PATH_INSTALL_PREFIX=$(INSTALL_PREFIX)
endif

$(shell CC="$(CC)" CXX="$(CXX)" TARGET_OS="$(TARGET_OS)" \
	$(MAKEFILE_PATH)/build_config $(BUILD_STATIC_SHARED) --source $(MAKEFILE_PATH))

include $(PWD)/qlevel_make_config

default:all

CFLAGS +=-I$(MAKEFILE_PATH) -I$(MAKEFILE_PATH)/include
CXXFLAGS +=-I$(MAKEFILE_PATH) -I$(MAKEFILE_PATH)/include
AR=ar
ARFLAGS=rc

STATIC_OBJS=$(SOURCE_OBJS:.cpp=.o)
SHARED=libqlevel.$(PLATFORM_SHARED_EXT)
STATIC=libqlevel.a

$(SHARED):
	mkdir -p $(PATH_INSTALL_PREFIX)/qlevel
	#echo $(BUILD_TARGET)
	$(CXX) $(LDFLAGS) $(PLATFORM_SHARED_LDFLAGS) $(CXXFLAGS) $(PLATFORM_SHARED_CFLAGS) $(SOURCES) -o $(SHARED)
	mv $(SHARED) $(PATH_INSTALL_PREFIX)/qlevel
	mv qlevel_make_config $(PATH_INSTALL_PREFIX)/qlevel

$(STATIC):
	mkdir -p $(PATH_INSTALL_PREFIX)/qlevel
	#echo $(BUILD_TARGET)
	$(CXX) $(CXXFLAGS) $(SOURCES)
	$(AR) $(ARFLAGS) $(STATIC) $(STATIC_OBJS)
	mv $(STATIC) $(PATH_INSTALL_PREFIX)/qlevel
	mv qlevel_make_config $(PATH_INSTALL_PREFIX)/qlevel
	rm -rf $(STATIC_OBJS)

ifeq ($(STATIC_OR_SHARED), shared)
BUILD_TARGET=$(SHARED)
else
BUILD_TARGET=$(STATIC)
CFLAGS += -c
CXXFLAGS += -c
endif


all: $(BUILD_TARGET)

.PHONY:install

install:
	mkdir -p $(PATH_INSTALL_PREFIX)/lib 
	cp -f `ls ./qlevel/libqlevel*` $(PATH_INSTALL_PREFIX)/lib
	cp -rfp $(MAKEFILE_PATH)/include/ $(PATH_INSTALL_PREFIX)/include/
	rm -rf qlevel_make_config

.PHONY:clean

clean:
	@rm -rf $(STATIC_OBJS)
	@rm -rf $(STATIC)
	@rm -rf $(SHARED)
	@rm -rf qlevel_make_config
