#pragma once

namespace Entities
{

class Entity;  // forward decl

void load_turret_data();
Entity* create_turret();
void ready_turret(Entity* object);
void die_turret(Entity* object);
void tick_turret(Entity* object);
void update_turret(Entity* object);

} // Entities
