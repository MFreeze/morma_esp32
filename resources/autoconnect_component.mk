#include $(IDF_PATH)/make/component_common.mk
COMPONENT_ADD_INCLUDEDIRS := src
COMPONENT_SRCDIRS := src

CXXFLAGS += -Wno-maybe-uninitialized