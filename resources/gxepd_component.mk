#include $(IDF_PATH)/make/component_common.mk
GX_EPD_DIRS := src $(patsubst $(COMPONENT_PATH)/src/GxGDE%,src/GxGDE%,$(wildcard $(COMPONENT_PATH)/src/*)) src/GxIO src/imglib src/GxIO/GxIO_SPI

# CFLAGS += -DESP32
# CPPFLAGS += -DESP32
# CXXFLAGS += -DESP32

COMPONENT_ADD_INCLUDEDIRS := $(GX_EPD_DIRS)
COMPONENT_SRCDIRS := src #$(GX_EPD_DIRS)
