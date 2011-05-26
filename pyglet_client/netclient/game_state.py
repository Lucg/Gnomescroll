import math

class GameStateGlobal:

    TICK = 0.01 # seconds
    
    gameState = None
    terrainMap = None
    agentList = None
    playerList = None
    player = None
    agent = None

    def __init__(self):
        pass
        
    @classmethod
    def init_0(self):
        pass
        
    @classmethod
    def init_1(self):
        GameStateGlobal.player = Player()
        GameStateGlobal.terrainMap = TerrainMap()
        GameStateGlobal.gameState = GameState()
        GameStateGlobal.agentList = AgentList()
        GameStateGlobal.playerList = PlayerList()
        
# main game state wrapper
class GameState:
        
    def __init__(self):
        self.time = 0

    # tick all agents
    #def tick(self):
        #for agent in GameStateGlobal.agentList.values():
            #agent.tick()
        #self.time += 1
        #if self.time % 100 == 0:
            #print "time= %i" % (self.time,)

from terrain_map import TerrainMap
from object_lists import AgentList
from agents import Agent
from object_lists import PlayerList
from players import Player
