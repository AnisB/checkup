#pragma once

// Bento includes
#include <bento_memory/common.h>

// SDK includes
#include "checkup_rest/curl_api.h"
#include "checkup_rest/request.h"

namespace checkup
{
	class TSession
	{
		ALLOCATOR_BASED;
	public:
		// Cst & Dst
		TSession(bento::IAllocator& allocator);
		~TSession();

		void init();
		void terminate();

		// Interactions
		bool execute(TRequest& request, bento::ILogger* logger = nullptr);

	private:
		curl::CurlInstance* _curlInstance;
		curl::CurlMessage* _curlMessage;
	public:
		bento::IAllocator& _allocator;
	};
}