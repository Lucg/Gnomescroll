#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <agent/constants.hpp>

class NetPeerManager
{
    public:
        int client_id;
        bool inited;
        bool loaded;
        bool waiting_for_auth;
        bool authorized;

        time_t connection_time;

        // auth stuff
        time_t auth_expiration;
        char username[PLAYER_NAME_MAX_LENGTH+1];
        int user_id;
        
        int auth_attempts;

    void was_authorized(int user_id, time_t expiration_time, const char* username);

    bool failed_to_authorize();
    bool authorization_expired();
    void failed_authorization_attempt();
    
    void broadcast_disconnect();

    void init(int client_id);
    void teardown();

    ~NetPeerManager();
    NetPeerManager();
};
