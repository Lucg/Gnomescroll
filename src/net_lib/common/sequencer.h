/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
   Licensed under GPLv3 */
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//#include <net_lib/common/net_peer.hpp>

#if DC_CLIENT
#include <net_lib/client/pviz.h>
#endif

class NetPeer;

//outgoing
/*
struct PacketSequence {
    int seq;
    int ack;
    //int active;
    //int tick; //better than sequence for determining dropped packets
};

struct PacketSequence2 {
    int seq;
    int received;
};
*/

void init_sequencer(class NetPeer* np);

void init_sequence_numbers(class NetPeer* np);
void process_acks(class NetPeer* np, unsigned short seq, unsigned int flag);
uint16_t get_next_sequence_number(class NetPeer* np);

int dropped_packet_count();

void check_for_dropped_packets(class NetPeer* np);

void init_sequence_numbers_out(class NetPeer* np);
void set_ack_for_received_packet(class NetPeer* np, int seq);
uint16_t get_sequence_number(class NetPeer* np);
uint32_t generate_outgoing_ack_flag(class NetPeer* np);


//incoming/response to outgoing
/*
 *
struct Pseq {
    struct PacketSequence packet_sequence_buffer[64];
    int packet_sequence_number;
};
*
struct PacketSequence2 {
    int seq;
    int received;
};

struct Pseq2 {
    struct PacketSequence2 seqbuff[64];
    int highest_packet_sequence_number;
};

void init_sequence_numbers_out(struct Pseq2* pq2);
uint16_t get_sequence_number(struct Pseq2* pq2);
void set_ack_for_received_packet(struct Pseq2* pq2, int seq);
uint32_t generate_outgoing_ack_flag(struct Pseq2* pq2);
*/
