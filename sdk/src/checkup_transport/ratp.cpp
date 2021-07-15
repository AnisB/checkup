// Bento includes
#include <bento_base/log.h>

// SDK includes
#include "checkup_transport/ratp.h"

// External includes
#include <json.hpp>

namespace checkup
{
    void build_ratp_request(TRequest& request)
    {
        request.validity = false;
        request.api = "https://api-ratp.pierre-grimaud.fr/v4/traffic";
        request.content = "";
        request.result = "INVALID";
    }

    bool build_ratp_data(const bento::DynamicString& ratpData, TRATPInfo& ratpInfo, bento::ILogger* logger)
    {
        bool success = true;
        try
        {
            // Parse the json answer
            auto ratpReponseJson = nlohmann::json::parse(ratpData.c_str());
            
            auto resultsJson = ratpReponseJson["result"];
            auto metrosJson = resultsJson["metros"];

            uint32_t numLines = (uint32_t)metrosJson.size();
            for (uint32_t lineIdx = 0; lineIdx < numLines; ++lineIdx)
            {
            	auto lineJson = metrosJson[lineIdx];
            	const std::string& lineName = lineJson["line"].get<std::string>();
                const std::string& message = lineJson["message"].get<std::string>();
                ratpInfo.lines_message[lineName] = message;
            }

            if (logger != nullptr)
            {
	            for (auto iteratorLM = ratpInfo.lines_message.begin(); iteratorLM != ratpInfo.lines_message.end(); ++iteratorLM)
	            {
                	logger->log(bento::LogLevel::info, "Line", iteratorLM->first.c_str());
                	logger->log(bento::LogLevel::info, "Message", iteratorLM->second.c_str());
	            }
            }
            // Every data was fetch successfully
            success = true;
        }
        catch (nlohmann::json::exception&)
        {
            if (logger != nullptr)
                logger->log(bento::LogLevel::error, "RATP", "RATP parsing failed.");
            success = false;
        }
        return success;
    }
}
