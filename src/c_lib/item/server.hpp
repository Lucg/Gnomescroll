/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

namespace Item
{

// subscriptions
void subscribe_agent_to_item(AgentID agent_id, ItemID item_id);
void unsubscribe_agent_from_item(AgentID agent_id, ItemID item_id);
void unsubscribe_all_from_item(ItemID item_id);

// packets
// sends to all subscribers
void send_item_create(ItemID item_id);
void send_item_state(ItemID item_id);
void send_item_destroy(ItemID item_id);

void send_item_charges(ItemID item_id);

}   // ItemContainer
