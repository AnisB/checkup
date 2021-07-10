// Bento includes
#include <bento_base/hash.h>
#include <bento_base/log.h>

// SDK includes
#include "checkup_rest/session.h"
#include "checkup_rest/curl_api.h"

namespace checkup
{
    TSession::TSession(bento::IAllocator& allocator)
    : _allocator(allocator)
    , _curlInstance(nullptr)
    , _curlMessage(nullptr)
    {

    }

    TSession::~TSession()
    {

    }

    void TSession::init()
    {
        // Create a curl instance for our communications
        _curlInstance = curl::create_instance();
        // Create a curl message for our communications
        _curlMessage = curl::create_message(_allocator);
    }

    bool TSession::execute(TRequest& request, bento::ILogger* logger)
    {
        // Build the request we shall be sending
        bento::DynamicString requestText(_allocator);
        requestText += request.api;
        requestText += "?";
        requestText += request.content;

        if (logger != nullptr)
            logger->log(bento::LogLevel::info, "Request", requestText.c_str());

        // Clear the message before using it
        curl::clear_message(_curlMessage);

        const char* res = curl::request(_curlInstance, requestText.c_str(), nullptr, 0, nullptr, nullptr, _curlMessage);
        
        bool success = false;
        if (res == nullptr)
        {
            // Grab the raw message data
            auto rawMessage = curl::message_raw_data(_curlMessage);
            
            // If message has no error code
            request.validity = true;
            request.result = rawMessage;
            success = true;
        }
        
        if (!success)
        {
            request.validity = false;
            request.result = "INVALID";
        }

        if (logger != nullptr)
            logger->log(bento::LogLevel::info, "Result", request.result.c_str());


        return success;
    }

    void TSession::terminate()
    {
        curl::destroy_instance(_curlInstance);
        curl::destroy_message(_curlMessage);
    }
}
