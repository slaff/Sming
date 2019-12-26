# => SSL
COMPONENT_SRCDIRS	:= src
COMPONENT_INCDIRS	:= include

COMPONENT_RELINK_VARS	:= ENABLE_SSL
ENABLE_SSL				?= 0
ifeq ($(ENABLE_SSL),$(filter $(ENABLE_SSL),1 AXTLS axTLS))
	override ENABLE_SSL	:= AXTLS
	COMPONENT_DEPENDS	:= axtls-8266
	COMPONENT_SRCDIRS	+= Axtls
else ifeq ($(ENABLE_SSL),$(filter $(ENABLE_SSL),2 BRSSL BearSSL BEARSSL))
	override ENABLE_SSL	:= BEARSSL
	COMPONENT_DEPENDS	:= bearssl-esp8266
	COMPONENT_SRCDIRS	+= BearSsl
else ifeq ($(ENABLE_SSL),MBEDTLS)
$(error MBED TLS not yet implemented)
else ifeq ($(ENABLE_SSL),0)
	COMPONENT_SRCDIRS	+= NoSsl
else
$(error Unsupported ENABLE_SSL value "$(ENABLE_SSL)")
endif

COMPONENT_RELINK_VARS	+= SSL_DEBUG
SSL_DEBUG				?= 0
ifeq ($(SSL_DEBUG),1)
	COMPONENT_CXXFLAGS	+= -DSSL_DEBUG=1
endif

# Prints SSL status when App gets built
CUSTOM_TARGETS			+= check-ssl
.PHONY:check-ssl
check-ssl:
ifeq ($(ENABLE_SSL),0)
	$(info SSL support disabled)
else
	$(info Using $(ENABLE_SSL) SSL implementation)
endif

COMPONENT_DOXYGEN_PREDEFINED := \
	ENABLE_SSL=1
