COMPONENT_DEPENDS := jsvm

SPIFF_FILES := files/build

HWCONFIG := spiffs

# Our sample JerryScript code needs very little heap size
# and it uses only standard JavaScript.   
JERRY_GLOBAL_HEAP_SIZE := 1
JERRY_COMPACT_PROFILE := 1