COMPONENT_SUBMODULES += lv_drivers
COMPONENT_DEPENDS := LVGL

LVGL_DRIVERS := $(COMPONENT_PATH)/lv_drivers

COMPONENT_SRCDIRS := $(LVGL_DRIVERS)/ \
					$(LVGL_DRIVERS)/wayland \
					$(LVGL_DRIVERS)/indev \
					$(LVGL_DRIVERS)/gtkdrv \
					$(LVGL_DRIVERS)/display
					
COMPONENT_INCDIRS := $(COMPONENT_PATH) $(LVGL_DRIVERS) 

# TODO: Make it easier to enable SDL2 on Linux...

ifeq ($(SMING_ARCH),Host)
# Make sure to have 32bit version of SDL2
# On Ubuntu this can be installed via: sudo apt-get install libsdl2-dev:i386
	EXTRA_LIBS += SDL2 					  
endif