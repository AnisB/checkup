// Bento includes
#include <bento_base/log.h>
#include <bento_math/vector3.h>

// SDK includes
#include "checkup_transport/ratp.h"

// External includes
#include <json.hpp>

namespace checkup
{
    TLineState slug_to_line_state(const char* slug)
    {
        if (strcmp(slug, "normal") == 0)
            return TLineState::Normal;
        if (strcmp(slug, "normal_trav") == 0)
            return TLineState::NormalTravaux;
        return TLineState::Other;
    }

    const char* state_to_string(TLineState state)
    {
        switch (state)
        {
        case TLineState::Normal:
            return "normal";
        case TLineState::NormalTravaux:
            return "normal_trav";
        case TLineState::Other:
            return "other";
        }
        return "INVALID";
    }

    bento::Vector3 state_to_color(TLineState state)
    {
        switch (state)
        {
        case TLineState::Normal:
            return bento::vector3(0.0, 1.0, 0.0);
        case TLineState::NormalTravaux:
            return bento::vector3(1.0, 1.0, 0.0);
        case TLineState::Other:
            return bento::vector3(1.0, 0.0, 0.0);
        }
        return bento::vector3(1.0, 0.0, 0.0);
    }

    void build_ratp_request(TRequest& request)
    {
        request.validity = false;
        request.api = "https://api-ratp.pierre-grimaud.fr/v4/traffic/metros";
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
                const std::string& slug = lineJson["slug"].get<std::string>();
                TLineInfo lineInfo;
                lineInfo.state = slug_to_line_state(slug.c_str());
                lineInfo.message = message;
                ratpInfo.linesInfo[lineName] = lineInfo;
            }

            if (logger != nullptr)
            {
	            for (auto iteratorLM = ratpInfo.linesInfo.begin(); iteratorLM != ratpInfo.linesInfo.end(); ++iteratorLM)
	            {
                	logger->log(bento::LogLevel::info, "Line", iteratorLM->first.c_str());
                    logger->log(bento::LogLevel::info, "State", state_to_string(iteratorLM->second.state));
                    logger->log(bento::LogLevel::info, "Message", iteratorLM->second.message.c_str());
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
