#pragma once

#include "net_lib_common.h"

#include "./sequencer.h"

#include <net_lib/common/packet_buffer.hpp>


#ifdef DC_CLIENT
#include <net_lib/client/pviz.h>
#endif

#include <net_lib/common/sequencer.h>
#include <net_lib/common/net_peer.hpp>

#include <net_lib/common/packet_buffer.hpp>

struct Socket {
    uint32_t ip;
    uint16_t port;
    int socket;
    struct sockaddr_in address;
};

class NetMessageBuffer; //forward declaration

struct packet_sequence {
    int seq;
    int ack;
    int time;

    class NetMessageBuffer* buffer;   //pointer to buffer
    int index;  //starting index in buffer
    int num;    //number of packets
};

struct packet_sequence2 {
    int seq;
    int received;
};

#include <net_lib/common/packet_buffer.hpp>

static const int PACKET_ARRAY_SIZE = 256;

class Packet {
    private:
    public:
    int time_out;    
    int message_n; //number of messages
};

class PacketArray {
    private:
    public:
    int lowest_packet;
    int packet_index;
    int free_index;
    class Net_message* net_message_array[PACKET_ARRAY_SIZE];
    class PacketArray* next;
    
};

static const int NET_MESSAGE_ARRAY_SIZE = 256;

class NetMessageArray {
    private:
    public:
    class Net_message* net_message_array[NET_MESSAGE_ARRAY_SIZE];
    int reference_count;
    class NetMessageArray* next;

    NetMessageArray()
    {
        for(int i=0; i < NET_MESSAGE_ARRAY_SIZE; i++) net_message_array[i] = NULL;
        reference_count = 0;
        next = NULL;
    }

    inline void retire() 
    {
        delete this;
    }

    static NetMessageArray* acquire()
    {
        return new NetMessageArray;
    }
};

class NetMessageBuffer {
    private:
    public:    
    //class NetMessageArray* head;
    //int consume_index;

    class NetMessageArray* current;
    int insert_index;

    inline void insert(class Net_message* nm)
    {
        current->net_message_array[insert_index] = nm;
        current->reference_count++;
        insert_index++;
        if(insert_index == NET_MESSAGE_ARRAY_SIZE)
        {
            insert_index = 0;
            current->next = NetMessageArray::acquire();
            current = current->next;
        }
    }

    //example iterator

    /*
        1> packet acked: decrement reference to packets
        2> packet dropped: put packet back onto que
    */
    void consume(NetMessageArray* nma, int consume_index, int num) 
    {
        int i;
        class Net_message* nm;
        for(i=0; i < num; i++)
        {
            nm = nma->net_message_array[consume_index];
            //do something

            nma->reference_count--;
            /*
            //optimize this; refence either equal zero at end or going into next buffer
            if(head->refence_count == 0)
            {
                head->retire();
            }
            */
            consume_index++;
            if(consume_index == NET_MESSAGE_ARRAY_SIZE)
            {
                if(nma->reference_count == 0) nma->retire(); //check 1
                nma = nma->next;
                consume_index=0;
            }
        }
        if(nma->reference_count == 0) nma->retire(); //check 2

    }
};

class NetPeer
{
    private:
    public:
    int client_id;
    int connected;

    //remote server
    struct Socket* socket;

    uint32_t ip;
    uint16_t port;
    struct sockaddr_in address;

    //buffer
    int buff_n;
    char buff[1500]; //out buffer
    /*
        push packets ont net_message_array and flush from here
    */

    /*
        NetMessageList
    */
    class Net_message* unreliable_net_message_array[256];
    int unreliable_net_message_array_index;
    
    class Net_message* reliable_net_message_array[256];
    int reliable_net_message_array_index;

    int pending_bytes_out;

    void push_unreliable_packet(Net_message* np) ;
    void push_reliable_packet(Net_message* np);

    void flush_to_buffer(char* buff, int* index);
    void flush_to_net();

    /*
        TTL
    */

    unsigned int ttl;
    unsigned int ttl_max;
    unsigned int last_packet_time;

    /*
        packet sequencer
    */

    //x to y: x state
    struct packet_sequence packet_sequence_buffer[256];
    int packet_sequence_number;
    //x to y: y state
    struct packet_sequence2 seqbuff[256];
    int highest_packet_sequence_number;

    /*
        Init
    */

    void init() {}

    NetPeer() {
        for(int i=0; i< 256; i++) unreliable_net_message_array[i] = NULL;
        for(int i=0; i< 256; i++) reliable_net_message_array[i] = NULL;

        unreliable_net_message_array_index = 0;
        reliable_net_message_array_index = 0;

        pending_bytes_out = 0;
    }
};


#define TTL_MAX_DEFAULT 120


//class NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port);
//class NetPeer* create_raw_net_peer(struct sockaddr_in address);

void reset_NetPeer_buffer(class NetPeer* s);

struct Socket* create_socket(uint16_t port);

class NetPeer* create_net_peer_by_remote_IP(int a, int b, int c, int d, unsigned short port);
class NetPeer* create_net_peer_from_address(struct sockaddr_in address);
