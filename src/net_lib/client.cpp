/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
   Licensed under GPLv3 */
#include "client.hpp"

#if DC_SERVER
# error Do not include this file in the server
#endif

namespace NetClient
{

NetPeer Server;

AgentID get_agent_id_for_client(ClientID client_id)
{
    IF_ASSERT(!isValid(client_id)) return NULL_AGENT;
    return (AgentID)client_id;
}

}   // NetClient
