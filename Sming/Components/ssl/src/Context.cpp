#include <Network/Ssl/Context.h>
#include <Network/Ssl/Session.h>

namespace Ssl
{
Extension& Context::getExtension()
{
	return session.extension;
}

bool Context::handshakeComplete()
{
	return session.handshakeComplete();
}

} // namespace Ssl
