
//#include "net_agent.hpp"

#include <net_lib/client/client.hpp>

#include <net_lib/common/message_handler.h>
#include <c_lib/agent/agent.hpp>

//helper class
class ClientToServer {
    public:
        void send();
        void serialize(unsigned char* buff, int* buff_n, int* size);
};

void serialize(unsigned char* buff, int* buff_n, int* size) {
    printf("ClientToServer: this should never happen\n");
    return;
}

void send::ClientToServer() {
    unsigned char* buff= NetClient::get_client_out_buffer();
    int* buff_n = NetClient::get_client_out_buffer_n();
    if(*buff_n > 800) { printf("Cannot send message: output buffer is full! %i bytes\n", *buff_n); return; }

    int bytes_written;
    serialize(buff_n,  ,&bytes_written)
}

class ServerToClient {
    public:
        void sendToClient(int client_id);  
        void broadcast();
        void serialize(unsigned char* buff, int* buff_n, int* size);
};

void sendToClient(int client_id) {
    return;
}

void broadcast() {
    return;
}

void serialize(unsigned char* buff, int* buff_n, int* size) {
    printf("ServerToClient: this should never happen\n");
    return;
}

/*
    agent control state message
*/

void handle_agent_control_state_message(unsigned char* buff, int buff_n, int* read_bytes) {
    Agent_control_state_message cs;
    cs.deserialize(buff, buff_n, read_bytes);
}

class Agent_control_state_message: public ClientToServer 
{
    public:
        static int message_id = 3;
        static int size = 2*sizeof(uint8_t) + 2*sizeof(uint16_t) + 3*sizeof(uint32_t);

        int id;
        int seq;
        uint32_t cs;
        unsigned int tick;
        unsigned int ctick;
        float theta;
        float phi;

        void serialize(unsigned char* buff, int* buff_n);
        void deserialize(unsigned char* buff, int buff_n, int* read_bytes, int* size);
};

void Agent_control_state_message::serialize(unsigned char* buff, int* buff_n, int* size) {
/*
    unsigned char* buff= NetClient::get_client_out_buffer();
    int* buff_n = NetClient::get_client_out_buffer_n();

    if(*buff_n > 800) {
        printf("Cannot send message: output buffer is full! %i bytes\n", *buff_n);
        return;
    }
*/
    int _buff_n = *buff_n;
    PACK_uint8_t(Agent_control_state_message_id, buff, buff_n);  //push message id on stack
    PACK_uint16_t(id, buff, buff_n); //agent id
    PACK_uint8_t(seq, buff, buff_n);
    PACK_uint16_t(tick%65536, buff, buff_n);
    PACK_uint32_t(cs, buff, buff_n);
    PACK_float(theta, buff, buff_n);
    PACK_float(phi, buff, buff_n);
    *size= buff_n-_buff_n;
}

void Agent_control_state_message::deserialize(unsigned char* buff, int buff_n, int* read_bytes) {
    int _buff_n = buff_n;
    int msg_id = UPACK_uint8_t(buff, &buff_n); //msg id, not used
    id = UPACK_uint16_t(buff, &buff_n); //agent id
    seq = UPACK_uint8_t(buff, &buff_n);
    tick =UPACK_uint16_t(buff, &buff_n);
    cs = UPACK_uint32_t(buff, &buff_n);
    theta = UPACK_float(buff, &buff_n);
    phi = UPACK_float(buff, &buff_n);
    *read_bytes = buff_n - _buff_n;
}

/*
    agent state message
*/

class Agent_state_message: public ServerToClient
{
    public:
        static int message_id = 4;
        static int size = 2*sizeof(uint8_t)+2*sizeof(uint16_t)+6*sizeof(uint32_t)

        int id;
        int seq;
        int tick;

        float x;
        float y;
        float z;
        float vx,vy,vz;

        void serialize(unsigned char* buff, int* buff_n);
        void deserialize(unsigned char* buff, int buff_n, int* read_bytes);
};

/*
#define Agent_state_message_id 4
#define Agent_state_message_size 2*sizeof(uint8_t)+2*sizeof(uint16_t)+6*sizeof(uint32_t)

void handle_agent_state_message(unsigned char* buff, int buff_n, int* bytes_read) {
    Agent_state_message cs;
    cs.deserialize(buff, buff_n, bytes_read); //should pass in pointer for number of ints read

    printf("Agent state received: id=%i, seq=%i \n", cs.id, cs.seq);

    //check id, if id does not exist, then create agent on receipt
}
*/
typedef void (*pt2handler)(unsigned char*, int, int*);
void Agent_state_message_register_message() {
    register_message_handler(Agent_state_message_id, Agent_state_message_size, (pt2handler) &handle_agent_control_state_message);
    printf("Registering handler for agent control state input: message_id= %i, message_size= %i\n", Agent_state_message_id, Agent_state_message_size);
}

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

Agent_state_message::Agent_state_message() {
    id = 0;
    seq = 0;
    tick = 0;

    x=0;y=0;z=0;
    vx=0;vy=0;vz=0;
}


/*
!!! Need to be able to broadcast messages


*/

void Agent_state_message::send_message(unsigned char* buff, int* buff_n) {

/*
    unsigned char* buff= NetClient::get_client_out_buffer();
    int* buff_n = NetClient::get_client_out_buffer_n();

    int bcount = *buff_n;
    if(*buff_n > 800) {
        printf("Cannot send message: output buffer is full! %i bytes\n", *buff_n);
        return;
    }
*/
    PACK_uint8_t(Agent_state_message_id, buff, buff_n);  //push message id on stack
    PACK_uint16_t(id, buff, buff_n); //agent id
    PACK_uint8_t(seq, buff, buff_n);
    PACK_uint16_t(tick%65536, buff, buff_n);

    PACK_float(x, buff, buff_n);
    PACK_float(y, buff, buff_n);
    PACK_float(z, buff, buff_n);
    PACK_float(vx, buff, buff_n);
    PACK_float(vy, buff, buff_n);
    PACK_float(vz, buff, buff_n);

    seq++;
    //printf("Agent_control_state_message::send_message: message size= %i bytes\n", *buff_n - bcount);
}

void Agent_state_message::deserialize(unsigned char* buff, int buff_n, int* read_bytes) {
    //PACK_uint8_t(3, buff, buff_n);  //push message id on stack
    int _buff_n = buff_n;
    int msg_id = UPACK_uint8_t(buff, &buff_n); //msg id, not used
    id = UPACK_uint16_t(buff, &buff_n); //agent id
    seq = UPACK_uint8_t(buff, &buff_n);
    tick =UPACK_uint16_t(buff, &buff_n);

    x = UPACK_float(buff, &buff_n);
    y = UPACK_float(buff, &buff_n);
    z = UPACK_float(buff, &buff_n);
    vx = UPACK_float(buff, &buff_n);
    vy = UPACK_float(buff, &buff_n);
    vz = UPACK_float(buff, &buff_n);

    *read_bytes = buff_n - _buff_n;
}
