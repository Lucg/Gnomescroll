#include "teams.hpp"

#include <c_lib/game/packets.hpp>

void Team::init(int id)
{
    viewers = false;
    this->id = id;
    n = 0;
    score = 0;
    r = 255;
    g = 10;
    b = 10;
    
    int i;
    for(i=0; i<TEAM_MAX_PLAYERS; i++) {
        members[i] = -1;    // load default agent values
    }
}

void Team::set_name(char* name) {
    int i;
    for (i=0; i<TEAM_NAME_MAX_LENGTH && name[i] != '\0'; i++) {
        this->name[i] = name[i];
    }
    name[i] = '\0';
}

void Team::set_color(unsigned char r, unsigned char g, unsigned char b) {
    this->r = r;
    this->g = g;
    this->b = b;
    #ifdef DC_SERVER
    TeamColor_StoC msg;
    msg.team = id;
    msg.r = r;
    msg.g = g;
    msg.b = b;
    msg.broadcast();
    #endif
}

bool Team::full() {
    if (n == TEAM_MAX_PLAYERS) {
        return true;
    }
    return false;
}

bool Team::has_agent(int id) {
    int i;
    for (i=0; i<TEAM_MAX_PLAYERS; i++) {
        if (members[i] == id) {
            return true;
        }
    }
    return false;
}

// returns false if team full
bool Team::add_agent(int id) {
    bool added = false;
    int i;
    for (i=0; i<TEAM_MAX_PLAYERS; i++) {
        if (members[i] != -1) continue; // spot taken
        members[i] = id;
        added = true;
        n++;
        break;
    }
    return added;
}

// returns false if agent wasnt found in team
bool Team::remove_agent(int id) {
    bool removed = false;
    int i;
    for (i=0; i<TEAM_MAX_PLAYERS; i++) {
        if (members[i] != id) continue;
        members[i] = -1;
        removed = true;
        n--;
        break;
    }
    return removed;
}

#ifdef DC_SERVER

void Team::update_client(int client_id) {
    // send color
    TeamColor_StoC msg;
    msg.team = id;
    msg.r = r;
    msg.g = g;
    msg.b = b;
    msg.sendToClient(client_id);
    // team roster is sent via agent objects
}

#endif

/* NoTeam */

void NoTeam::init(int id)
{
    viewers = true;
    n = 0;
    this->id = id;
    int i;
    for(i=0; i<GAME_MAX_PLAYERS; i++) {
        members[i] = -1;    // load default agent values
    }
    char name[] = "Viewers";
    strcpy(this->name, name);
}

// return true if agent in team
bool NoTeam::has_agent(int id) {
    int i;
    for (i=0; i<GAME_MAX_PLAYERS; i++) {
        if (members[i] == id) {
            return true;
        }
    }
    return false;
}

// returns false if team full
bool NoTeam::add_agent(int id) {
    bool added = false;
    int i;
    for (i=0; i<GAME_MAX_PLAYERS; i++) {
        if (members[i] != -1) continue;
        members[i] = id;
        added = true;
        n++;
        break;
    }
    return added;
}

// returns true if removed
bool NoTeam::remove_agent(int id) {
    bool removed = false;
    int i;
    for (i=0; i<GAME_MAX_PLAYERS; i++) {
        if (members[i] != id) continue;
        members[i] = -1;
        removed = true;
        n--;
        break;
    }
    return removed;
}

/* CTF Team */

CTFTeam::CTFTeam()
:
flag(NULL), base(NULL),
base_score(0),
flag_captures(0)
{}

void CTFTeam::init(int id)
{
    Team::init(id);
    this->flag = new Flag();
    this->flag->team = id;

    this->base = new Base();
    this->base->team = id;
}

int CTFTeam::score()
{
    #ifdef DC_SERVER
    return this->flag_captures;
    #endif

    #ifdef DC_CLIENT
    return this->base_score;
    #endif
}

void CTFTeam::captured_flag()
{
    int s = this->score();
    this->flag_captures++;
    if (s != this->score())
    {
        TeamScore_StoC msg;
        msg.team = this->id;
        msg.score = this->score();
        msg.broadcast();
    }
}

CTFTeam::~CTFTeam()
{
    if (this->flag != NULL) delete this->flag;
    if (this->base != NULL) delete this->base;
}
