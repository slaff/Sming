COMPONENT_SOC := esp* host

COMPONENT_SRCDIRS := \
	src \
	$(call ListAllSubDirs,$(COMPONENT_PATH)/src) \
	$(call ListAllSubDirs,$(COMPONENT_PATH)/Arch/$(SMING_ARCH))

COMPONENT_INCDIRS := \
	src \
	Arch/$(SMING_ARCH)/include

COMPONENT_DOXYGEN_INPUT := \
	src \
	Arch/Esp32/include

COMPONENT_DEPENDS := \
	ssl \
	http-parser \
	libb64 \
	ws_parser \
	mqtt-codec \
	libyuarel

# WiFi settings may be provide via Environment variables
CONFIG_VARS				+= WIFI_SSID WIFI_PWD
ifdef WIFI_SSID
	APP_CFLAGS			+= -DWIFI_SSID=\"$(WIFI_SSID)\"
	APP_CFLAGS			+= -DWIFI_PWD=\"$(WIFI_PWD)\"
endif

# => WPS
COMPONENT_VARS			+= ENABLE_WPS
ifeq ($(ENABLE_WPS), 1)
	GLOBAL_CFLAGS		+= -DENABLE_WPS=1
endif

# => Smart Config
COMPONENT_VARS			+= ENABLE_SMART_CONFIG
ifeq ($(ENABLE_SMART_CONFIG),1)
	GLOBAL_CFLAGS		+= -DENABLE_SMART_CONFIG=1
endif

# => HTTP server
COMPONENT_VARS			+= HTTP_SERVER_EXPOSE_NAME
HTTP_SERVER_EXPOSE_NAME ?= 1
GLOBAL_CFLAGS			+= -DHTTP_SERVER_EXPOSE_NAME=$(HTTP_SERVER_EXPOSE_NAME)

COMPONENT_VARS			+= HTTP_SERVER_EXPOSE_VERSION
HTTP_SERVER_EXPOSE_VERSION ?= 0
GLOBAL_CFLAGS			+= -DHTTP_SERVER_EXPOSE_VERSION=$(HTTP_SERVER_EXPOSE_VERSION)

# => LWIP
COMPONENT_VARS			+= ENABLE_CUSTOM_LWIP
ifeq ($(SMING_ARCH),Esp8266)

ENABLE_CUSTOM_LWIP		?= 1
ifeq ($(ENABLE_CUSTOM_LWIP), 0)
	COMPONENT_DEPENDS	+= esp-lwip
else ifeq ($(ENABLE_CUSTOM_LWIP), 1)
	COMPONENT_DEPENDS	+= esp-open-lwip
else ifeq ($(ENABLE_CUSTOM_LWIP), 2)
	COMPONENT_DEPENDS	+= lwip2
endif

else ifeq ($(SMING_ARCH),Host)

COMPONENT_DEPENDS += \
	esp_wifi \
	lwip

endif
