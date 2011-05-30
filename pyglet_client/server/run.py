from time import sleep

#import cython
import pyximport #; pyximport.install()

#from pudb import set_trace; set_trace()

from net_server import NetServer
from net_out import NetOut
from net_event import NetEvent
from game_state import GameStateGlobal
from chat_server import ChatServer

from cube_dat import CubeGlobals

import random
def load_map():
    m = GameStateGlobal.terrainMap
    print "Start map generation"
    x_min = -1
    y_min = -1
    z_min = 0

    x_max = 8
    y_max = 8
    z_max = 1

    #plane for testing z collision
    for xa in range(x_min, x_max):
        for ya in range(y_min, y_max):
            for za in range(z_min, z_max):
                rnd = random.randint(0,64)
                if True or rnd < 16:
                    rnd2 = random.randint(1,4)
                    rnd2 = 3
                    m.set(xa,ya,za, rnd2)
    #protrusions for testing collision detection
    for xa in range(x_min, x_max):
        for ya in range(y_min, y_max):
            for za in range(z_min, z_max):
                rnd = random.randint(0,64)
                if rnd < 16:
                    m.set(xa,ya,za+1, 4)
                    m.set(xa,ya,za+2, 4)
                    #m.set(xa,ya,za+3, 4)
    print "Finished map generation"

class Main:

    def __init__(self):
        #setup
        NetServer().init()
        NetOut().init()
        NetEvent().init()
        GameStateGlobal().init()
        ChatServer().init()
        CubeGlobals.init()
        #phase 2 inits
        CubeGlobals.init_1()
    def run(self):
        print "Server Started"
        load_map()
        #print "Create Agent"
        #GameStateGlobal.agentList.create(-10, 0, -3, 0, 0)

        while True:
            NetServer.serverListener.accept() #accept incoming connections
            NetServer.connectionPool.process_events() #check for new data
            GameStateGlobal.gameState.tick()
            NetOut.event.process_events()
            sleep(GameStateGlobal.TICK)

if __name__ == "__main__":
    print "starting server"
    main = Main()
    main.run()
