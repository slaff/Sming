#pragma once

#include "../Resource/HttpConditionalResource.h"

class HttpAuthChain
{
public:
	using HttpCheckers=Vector<HttpResourceDelegate>;

	/**
	 * @brief Creates a chain of checkers
	 * @param  checkAll if true then all checkers must succeed to the request to be allowed
	 * 					otherwise at least one checker must succeed
	 */
	HttpAuthChain(bool checkAll = true): checkAll(checkAll)
	{
	}

	bool add(const HttpResourceDelegate& delegate)
	{
		return checkers.add(delegate);
	}

	/**
	 * @brief Runs all checkers to see if the request can proceed.
	 * @note The checkers are run in the same order in which they were declared
	 * 		 If checkAll is set then the request will proceed when all checkers are true (AND condition)
	 * 		 If checkAll is set to false then at least one checker needs to be true (OR condition)
	 */
	int operator()(HttpServerConnection& connection, HttpRequest& request, HttpResponse& response)
	{
		for(int i=0; i<checkers.count(); i++) {
			auto checker = checkers[i];
			int error = checker(connection, request, response);
			if(error && checkAll) {
				return error;
			}

			if(!error && !checkAll) {
				return 0;
			}
		}

		return 0;
	}

private:
	HttpCheckers checkers;
	bool checkAll;
};
