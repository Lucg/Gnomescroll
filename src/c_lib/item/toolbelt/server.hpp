/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

namespace Toolbelt
{

void turn_fire_on(AgentID agent_id);
void turn_fire_off(AgentID agent_id);

void broadcast_agent_set_active_item_packet(AgentID agent_id, ItemType item_type);
void broadcast_agent_toolbelt_begin_alpha_action_packet(AgentID agent_id, ItemType item_type);
void broadcast_agent_toolbelt_end_alpha_action_packet(AgentID agent_id);
void broadcast_agent_toolbelt_alpha_action_packet(AgentID agent_id, ItemType item_type);
void broadcast_agent_toolbelt_beta_action_packet(AgentID agent_id, ItemType item_type);
void send_agent_set_active_item_packet(ClientID client_id, AgentID agent_id, ItemType item_type);

} // Toolbelt
