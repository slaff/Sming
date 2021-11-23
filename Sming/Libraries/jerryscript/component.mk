# JerryScript 
COMPONENT_NAME := jerryscript
COMPONENT_SUBMODULES := jerryscript

JERRY_CORE := $(COMPONENT_PATH)/jerryscript/jerry-core/
JERRY_INCDIRS := jerryscript $(JERRY_CORE) $(call ListAllSubDirs, $(JERRY_CORE))
COMPONENT_SRCDIRS := $(call ListAllSubDirs, $(JERRY_CORE))
COMPONENT_INCDIRS := $(JERRY_INCDIRS)
INCDIR = $(JERRY_INCDIRS)

COMPONENT_CFLAGS := -std=c99 -D__TARGET_ESP8266
COMPONENT_CFLAGS += -Wall -Os -g -O2 -Wpointer-arith -Wno-implicit-function-declaration -Wl,-EL -fno-inline-functions -nostdlib -D__ets__ -DICACHE_FLASH 

ifeq ($(SMING_ARCH),Esp8266)
	COMPONENT_CFLAGS += -mlongcalls  -mno-text-section-literals
endif

# The size of the JerryScript engine heap. The size is allocated once and defined at compile time.
COMPONENT_VARS := JERRY_GLOBAL_HEAP_SIZE
JERRY_GLOBAL_HEAP_SIZE := 512

# Compact compilation profile makes the JerryScript library smaller
COMPONENT_VARS += JERRY_COMPACT_PROFILE  
JERRY_COMPACT_PROFILE := 1

# COMPONENT_CFLAGS += -fno-builtin -fno-stack-protector -g -gdwarf-4 -Wall -Werror=all -Wextra -Werror=extra -Wformat-nonliteral -Werror=format-nonliteral -Winit-self -Werror=init-self -Wconversion -Werror=conversion -Wsign-conversion -Werror=sign-conversion -Wformat-security -Werror=format-security -Wmissing-declarations -Werror=missing-declarations -Wno-stack-protector -Wno-attributes -Wlogical-op -Werror=logical-op  -D__attr_always_inline___= -Wl,-EL -fno-inline-functions -ffunction-sections -fdata-sections -mlongcalls -mtext-section-literals -mno-serialize-volatile -pedantic -Os
ifeq ($(JERRY_COMPACT_PROFILE), 1)
   COMPONENT_CFLAGS += -DJERRY_BUILTINS=0 \
              		     -DJERRY_ES2015=0 \
              		     -DJERRY_UNICODE_CASE_CONVERSION=0
endif

COMPONENT_CFLAGS += -DJERRY_NUMBER_TYPE_FLOAT64=0 -DJERRY_GLOBAL_HEAP_SIZE=$(JERRY_GLOBAL_HEAP_SIZE) \
              		  -DJERRY_LCACHE=0  \
              		  -DJERRY_PARSER=0 -DJERRY_SNAPSHOT_EXEC=1 -DJERRY_NDEBUG -D_DEFAULT_SOURCE \
              		  -D__TARGET_ESP8266 -D__attr_always_inline___=

MFORCE32 := $(shell $(CC) --help=target | grep mforce-l32)

ifneq ($(MFORCE32),)
    # Your compiler supports the -mforce-l32 flag which means that 
    # constants can be placed in ROM to free additional RAM
    COMPONENT_CFLAGS += -DJERRY_ATTR_CONST_DATA="__attribute__((aligned(4))) __attribute__((section(\".irom.text\")))" -mforce-l32 
endif
