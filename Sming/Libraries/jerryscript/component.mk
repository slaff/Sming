# JerryScript 
COMPONENT_SUBMODULES := jerryscript

JERRYSCRIPT_ROOT := $(COMPONENT_PATH)/jerryscript
COMPONENT_SRCDIRS := src $(call ListAllSubDirs,$(JERRYSCRIPT_ROOT)/jerry-core)
COMPONENT_INCDIRS := src jerryscript $(JERRYSCRIPT_ROOT)/jerry-core $(COMPONENT_SRCDIRS)

# The size of the JerryScript engine heap. The size in kilobytes is allocated once and defined at compile time.
COMPONENT_VARS := JERRY_GLOBAL_HEAP_SIZE
JERRY_GLOBAL_HEAP_SIZE ?= 1

# Compact compilation profile makes the JerryScript library smaller
COMPONENT_VARS += JERRY_COMPACT_PROFILE  
JERRY_COMPACT_PROFILE ?= 1

ifeq ($(JERRY_COMPACT_PROFILE),1)
	COMPONENT_CFLAGS += \
		-DJERRY_BUILTINS=0 \
		-DJERRY_ES2015=0 \
		-DJERRY_UNICODE_CASE_CONVERSION=0
endif

COMPONENT_CFLAGS += \
	-DJERRY_NUMBER_TYPE_FLOAT64=0 \
	-DJERRY_GLOBAL_HEAP_SIZE=$(JERRY_GLOBAL_HEAP_SIZE) \
	-DJERRY_LCACHE=0  \
	-DJERRY_PARSER=0 \
	-DJERRY_SNAPSHOT_EXEC=1 \
	-DJERRY_NDEBUG

DEBUG_VARS += JERRY_SNAPSHOT_TOOL
JERRY_BUILD_DIR := $(COMPONENT_PATH)/jerryscript/out
ifeq ($(UNAME),Windows)
JERRY_SNAPSHOT_TOOL := $(JERRY_BUILD_DIR)/bin/MinSizeRel/jerry-snapshot.exe
else
JERRY_SNAPSHOT_TOOL := $(JERRY_BUILD_DIR)/bin/jerry-snapshot
endif

$(JERRY_SNAPSHOT_TOOL):
	$(Q) $(PYTHON) $(JERRYSCRIPT_ROOT)/tools/build.py \
		--lto OFF \
		--jerry-cmdline-snapshot ON \
		--builddir "$(JERRY_BUILD_DIR)"

jerryscript-clean: jerry-tools-clean

.PHONY: jerry-tools-clean
jerry-tools-clean:
	$(Q) rm -rf $(JERRY_BUILD_DIR)

# Support for building .snap files from .js source

DEBUG_VARS += APP_JS_SOURCE_DIR APP_JS_SNAP_DIR

ifdef APP_JS_SOURCE_DIR
APP_JS_SNAP_DIR ?= out/jerryscript

APP_JS_SOURCE_FILES := $(notdir $(wildcard $(APP_JS_SOURCE_DIR)/*.js))
APP_JS_SNAP_FILES := $(patsubst %,$(APP_JS_SNAP_DIR)/%.snap,$(APP_JS_SOURCE_FILES))

COMPONENT_PREREQUISITES := $(APP_JS_SNAP_FILES)

$(APP_JS_SNAP_FILES): $(JERRY_SNAPSHOT_TOOL) | $(APP_JS_SNAP_DIR)
	$(Q) $(JERRY_SNAPSHOT_TOOL) generate $(APP_JS_SOURCE_DIR)/$(@F:.snap=) -o $@

$(APP_JS_SNAP_DIR):
	mkdir -p $@

clean: snap-clean
snap-clean:
	$(Q) rm -rf $(APP_JS_SNAP_DIR)
endif
