#pragma once

namespace Entities
{

/* Robot Bomb */

/* Common/Defaults */

// health
const int MONSTER_BOMB_HEALTH_MIN = 65;
const int MONSTER_BOMB_HEALTH_MAX = 85;

// vox
const bool MONSTER_BOMB_INIT_WITH_HITSCAN = true;
const bool MONSTER_BOMB_INIT_WITH_DRAW = true;

// dims
const float MONSTER_BOMB_HEIGHT = 1.0f;

// explosion
const float MONSTER_BOMB_EXPLOSION_RADIUS = 4.0f;
const int MONSTER_BOMB_EXPLOSION_DAMAGE = 40;
const float MONSTER_BOMB_EXPLOSION_PROXIMITY_RADIUS = 0.8f;
const float MONSTER_BOMB_BLOCK_DESTRUCTION_RADIUS = 3.0f;
const int MONSTER_BOMB_BLOCK_DAMAGE = 16;

// targeting
const float MONSTER_BOMB_MOTION_PROXIMITY_RADIUS = 15.0f;
const int MONSTER_BOMB_MAX_TARGET_LOCK_TICKS = ONE_SECOND * 10; // will lose target after 10 seconds

// motion
const float MONSTER_BOMB_WALK_SPEED = 0.125f;
//const float MONSTER_BOMB_WALK_SPEED = 1.0f;
const float MONSTER_BOMB_CHASE_SPEED = 0.25f;
const int MONSTER_BOMB_MOTION_MAX_Z_DIFF = 5;
//const int MONSTER_BOMB_WALK_RANGE = 10;
const int MONSTER_BOMB_WALK_RANGE = 20;

// waiting
//const unsigned int MONSTER_BOMB_IDLE_TIME = ONE_SECOND * 6;
const unsigned int MONSTER_BOMB_IDLE_TIME = ONE_SECOND * 3;

// animations
const Color MONSTER_BOMB_ANIMATION_COLOR = Color(31,223,223);
const int MONSTER_BOMB_ANIMATION_COUNT = 50;
const int MONSTER_BOMB_ANIMATION_COUNT_MAX = 100;
const float MONSTER_BOMB_ANIMATION_SIZE = 0.7f;
const float MONSTER_BOMB_ANIMATION_FORCE = 20.0f;

/* Slime */

/* Common/Defaults */

// health
//const int MONSTER_SLIME_HEALTH_MIN = 65;
//const int MONSTER_SLIME_HEALTH_MAX = 85;
const int MONSTER_SLIME_HEALTH_MIN = 1000;
const int MONSTER_SLIME_HEALTH_MAX = 1000;

// vox
const bool MONSTER_SLIME_INIT_WITH_HITSCAN = true;
const bool MONSTER_SLIME_INIT_WITH_DRAW = true;

// dims
const float MONSTER_SLIME_HEIGHT = 1.0f;

// targeting
const float MONSTER_SLIME_MOTION_PROXIMITY_RADIUS = 15.0f;
const int MONSTER_SLIME_MAX_TARGET_LOCK_TICKS = ONE_SECOND * 10; // will lose target after 10 seconds
const float MONSTER_SLIME_JUMP_FORCE = 0.38f;
const float MONSTER_SLIME_AGENT_STOP_PROXIMITY_RADIUS = 2.1f;
const int MONSTER_SLIME_JUMP_COOLDOWN_EN_ROUTE = (3 * ONE_SECOND) / 2;
const int MONSTER_SLIME_JUMP_COOLDOWN_NEARBY = ONE_SECOND * 2;

// motion
const float MONSTER_SLIME_WALK_SPEED = 0.1f;
//const float MONSTER_SLIME_WALK_SPEED = 1.0f;
const float MONSTER_SLIME_CHASE_SPEED = 0.12f;
const int MONSTER_SLIME_MOTION_MAX_Z_DIFF = 3;
//const int MONSTER_SLIME_WALK_RANGE = 10;
const int MONSTER_SLIME_WALK_RANGE = 20;

// waiting
//const unsigned int MONSTER_SLIME_IDLE_TIME = ONE_SECOND * 6;
const unsigned int MONSTER_SLIME_IDLE_TIME = ONE_SECOND * 3;

// animations
const Color MONSTER_SLIME_ANIMATION_COLOR = Color(31,223,31);
const int MONSTER_SLIME_ANIMATION_COUNT = 25;
const int MONSTER_SLIME_ANIMATION_COUNT_MAX = 40;
const float MONSTER_SLIME_ANIMATION_SIZE = 0.2f;
const float MONSTER_SLIME_ANIMATION_FORCE = 1.0f;

/* MONSTER_BOX */

// health
const int MONSTER_BOX_HEALTH_MIN = 20;
const int MONSTER_BOX_HEALTH_MAX = 30;

//vox
const bool MONSTER_BOX_INIT_WITH_HITSCAN = true;
const bool MONSTER_BOX_INIT_WITH_DRAW = true;

//dims
const float MONSTER_BOX_HEIGHT = 3.0f;
const float MONSTER_BOX_CAMERA_HEIGHT = 2.5f;

// targeting
const float MONSTER_BOX_TARGET_ACQUISITION_FAILURE_RATE = 0.1f;
const int MONSTER_BOX_FIRE_RATE_LIMIT = 45;
const bool MONSTER_BOX_USES_BIAS = false;
const float MONSTER_BOX_ACCURACY_BIAS = 0.0f;
const float MONSTER_BOX_FIRING_SIGHT_RANGE = 15.0f;
const bool MONSTER_BOX_ATTACK_AT_RANDOM = false;
const int MONSTER_BOX_TERRAIN_DAMAGE = 5;
const int MONSTER_BOX_AGENT_DAMAGE_MIN = 1;
const int MONSTER_BOX_AGENT_DAMAGE_MAX = 5;
const int MONSTER_BOX_AGENT_IMMUNITY_DURATION = 30 * 3;
//const int MONSTER_BOX_VOXEL_DAMAGE_RADIUS = 1;
const int MONSTER_BOX_FIRE_DELAY_MAX = 1;

// animations
const Color MONSTER_BOX_ANIMATION_COLOR = Color(223,31,31);
const int MONSTER_BOX_ANIMATION_COUNT = 5*5*5;
//const int MONSTER_BOX_ANIMATION_COUNT_MAX = 5*5*5;
const float MONSTER_BOX_ANIMATION_SIZE = 0.2f;
const float MONSTER_BOX_ANIMATION_FORCE = 10.0f;

// motion
const float MONSTER_BOX_MOTION_SIGHT_RANGE = 50.0f;
const float MONSTER_BOX_SPEED = 0.2f;
const int MONSTER_BOX_WALK_RANGE = 30;
const int MONSTER_BOX_MOTION_MAX_Z_DIFF = 3;

/* Monster spawners */

// health
const int MONSTER_SPAWNER_MAX_HEALTH = 150;

//vox
const bool MONSTER_SPAWNER_INIT_WITH_HITSCAN = true;
const bool MONSTER_SPAWNER_INIT_WITH_DRAW = true;

// dim
const float MONSTER_SPAWNER_HEIGHT = 1.9f;

// spawner
const int MONSTER_SPAWNER_MAX_CHILDREN = 10;
const float MONSTER_SPAWNER_SPAWN_RADIUS = 2.5f;

// animations
const Color MONSTER_SPAWNER_ANIMATION_COLOR = Color(223,31,31);
const int MONSTER_SPAWNER_ANIMATION_COUNT = 5*5*5;
//const int MONSTER_SPAWNER_ANIMATION_COUNT_MAX = 5*5*5;
const float MONSTER_SPAWNER_ANIMATION_SIZE = 0.4f;

} // Entities
