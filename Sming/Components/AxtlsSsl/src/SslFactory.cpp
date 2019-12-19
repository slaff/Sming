/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * SslFactory.cpp
 *
 * @author: 2019 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/

#include <Network/Ssl/SslFactory.h>
#include "SslContextImpl.h"

class SslFactoryImpl : public SslFactory
{
public:
	SslContext* sslCreateContext() override
	{
		return new SslContextImpl();
	}
};
