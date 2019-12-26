COMPONENT_SUBMODULES	:= axtls-8266

COMPONENT_SRCDIRS := \
	axtls-8266/crypto \
	axtls-8266/ssl

ifneq ($(SMING_ARCH),Host)
COMPONENT_SRCDIRS += \
	axtls-8266/replacements
endif

COMPONENT_INCDIRS := .

EXTRA_INCDIR := \
	axtls-8266 \
	axtls-8266/ssl \
	axtls-8266/crypto

GLOBAL_CFLAGS			+= -DLWIP_RAW=1
COMPONENT_CFLAGS		:= -DWITH_PGM_READ_HELPER=1 -DAXTLS_BUILD

# Application
CUSTOM_TARGETS			+= include/ssl/private_key.h

AXTLS_PATH				:= $(COMPONENT_PATH)/axtls-8266

include/ssl/private_key.h:
	$(info Generating unique certificate and key. This may take some time...)
	$(Q) mkdir -p $(PROJECT_DIR)/include/ssl/
	$(Q) chmod a+x $(AXTLS_PATH)/tools/make_certs.sh
	AXDIR=$(PROJECT_DIR)/include/ssl/ $(AXTLS_PATH)/tools/make_certs.sh
