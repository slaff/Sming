#include <Network/Ssl/Context.h>
#include <Network/Ssl/Session.h>

namespace Ssl
{
Extension& Context::getExtension()
{
	return session.extension;
}

bool Context::validateCertificate()
{
	return session.validateCertificate();
}

void Context::handshakeComplete(bool success)
{
	session.handshakeComplete(success);
}

} // namespace Ssl
