/**
 * @file    MessageAgent.h
 * @brief
 * @author  MyungHyun Lee (kent), orignally by Quang and SeungEun
 * @date    Feb 02, 2009
 */
#ifndef _DOCUMENT_MESSAGE_AGENT_H_
#define _DOCUMENT_MESSAGE_AGENT_H_

#include <net/message-framework/MessageClientLight.h>

namespace messageframework
{
    /**
     * @brief For profiling, Wei Cao
     */
    extern long permission_time;
    extern long send_time;
    extern long recv_time;
    
    typedef MessageClientLight MessageClient;

    /**
     * @brief Get the service permission info of a given service name
     * from cache if hit, or get it from controller.
     */
	bool getServicePermissionInfo( const std::string& serviceName,
		ServicePermissionInfo& servicePermissionInfo,
		MessageClient& client) throw (std::runtime_error);

    /**
     * @brief Process one request at a time.
     */
    // Service with no results
    // added @by MyungHyun Lee - Feb 11, 2009
    bool requestService(const std::string &                     name,
            ServiceRequestInfoPtr                              parameter,
            MessageClient &                                 client) throw (std::runtime_error);


    bool requestService(const std::string &                     name,
            ServiceRequestInfoPtr                             parameter,
            ServiceResultPtr &                                 result,
            MessageClient &                                 client) throw (std::runtime_error);

    void runMessageClient(ServiceRequestInfoPtr&               serviceRequestInfo,
            ServiceResultPtr&                                  serviceResult,
            MessageClient&                                  client) throw (std::runtime_error);

    /**
     * @brief This set of functions act in a way of "batch processing".
     * Send all requests out at the same time, then wait for their replies
     * Added by Wei Cao, 2009-02-19
     */
    bool requestService(const std::string& serviceName,
		std::vector<ServiceRequestInfoPtr>& serviceRequestInfos,
		MessageClient& client ) throw (std::runtime_error);

    bool requestService(const std::string& serviceName,
		std::vector<ServiceRequestInfoPtr>& serviceRequestInfos,
		std::vector<ServiceResultPtr>& serviceResults,
		MessageClient& client ) throw (std::runtime_error);
}

#endif