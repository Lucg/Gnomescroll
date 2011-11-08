#!/usr/bin/python

'''
Client input
'''
import opts
opts = opts.opts

import c_lib.c_lib_input as cInput
import c_lib.c_lib_hud as cHUD
import c_lib.c_lib_agents as cAgents

from math import sin, cos, pi
from c_lib.terrain_map import toggle_t_viz_vbo_indicator_style, toggle_terrain_map_blend_mode, refresh_map_vbo, toggle_z_buffer
from init_c_lib import _toggle_latency_unit

#handles special characters
Keystring = {}
def setup_keystring():
    global Keystring
    global special_keys
    special_keys = {
    'BACKSPACE' : 8,
    'TAB': 9,
    'ENTER': 13,
    'ESC': 27,
    'SPACE': 32,
    'DELETE': 127,
    'LEFT': 276,
    'RIGHT': 275,
    'UP' : 273,
    'DOWN' : 274,
    'LSHIFT' : 304,
    'RSHIT' : 303,
    'LCTRL' : 306,
    'RCTRL' : 305,
    'QUIT'  : 9999, # custom key
    }
    for i in range(1,255):
        Keystring[i] = chr(i)
    for key,value in special_keys.items():
        Keystring[value] = key
setup_keystring()

event_names = {
    0   :   'SDL_KEYUP',
    1   :   'SDL_KEYDOWN',
}

keystate = {}

class InputEventGlobal:
    mouse = None
    keyboard = None

    def keyboard_state(self, pressed_keys):
        keyboard = []
        for keycode in pressed_keys:
            temp = Keystring.get(keycode, None)
            if temp != None:
                keyboard.append(temp)
        self.keyboard.stateHandler(keyboard)

    #add support for key pressed/key released
    def keyboard_text_event(self, keycode, key, event_type):
        event_name = event_names[event_type]
        #print "%s, keycode=%d key=%s" % (event_name, keycode, key,)

        # set keystate map
        if event_name == 'SDL_KEYDOWN':
            keystate[keycode] = 1
            self.keyboard.on_key_press(key)

        elif event_name == 'SDL_KEYUP':
            keystate[keycode] = 0
            self.keyboard.on_key_release(key)
        
    def mouse_event(self, button,state,x,y,):
        self.mouse.on_mouse_press(x,y,button, state)

    def mouse_motion(self, x,y,dx,dy,button):
        if button != 0:
            self.mouse.on_mouse_drag(x,y,dx,dy,button)
        else:
            self.mouse.on_mouse_motion(x,y,dx,dy)

class InputGlobal:
    keyboard = None
    mouse = None
    agentInput = None
    voxel_aligner = None
    use_voxel_aligner = False
    inventory = False

    input = 'camera'
    _inputs = ('camera', 'agent')
    camera = 'camera'
    _cameras = ('camera', 'agent')

    scoreboard = False
    block_selector = None

    @classmethod
    def init_0(cls, main):
        #InputEventGlobal.inputGlobal = cls

        InputGlobal.mouse = Mouse()
        InputGlobal.keyboard = Keyboard(main)
        InputGlobal.agentInput = AgentInput()
        cls.block_selector = BlockSelector(8,8,range(8*8))
        cls.voxel_aligner = VoxelAligner()

        InputEventGlobal.mouse = cls.mouse
        InputEventGlobal.keyboard = cls.keyboard

    @classmethod
    def init_1(cls, main):
        pass
        
    @classmethod
    def _toggle_mode(cls, change, current_mode, type):
        modes = getattr(InputGlobal, '_'+type+'s')
        current_mode = (current_mode + change) % len(modes)
        new_mode_name = modes[current_mode]
        if new_mode_name == 'agent' and GameStateGlobal.agent is None:
            return
        setattr(InputGlobal, type, new_mode_name)
        #print "%s mode= %s" % (type, str(getattr(InputGlobal, type)),)
        return current_mode

    # toggles through modes.
    @classmethod
    def toggle_input_mode(cls, change=1, current_mode=[0]):
        curr = InputGlobal._toggle_mode(change, current_mode[0], 'input')
        if curr is not None:
            current_mode[0] = curr
        print 'input mode is %s' % curr
        #if cls._inputs[curr] == 'camera':
            #cls.mouse.clear_mouse_deltas()

    @classmethod
    def toggle_camera_mode(cls, change=1, current_mode=[0]):
        curr = InputGlobal._toggle_mode(change, current_mode[0], 'camera')
        if curr is not None:
            current_mode[0] = curr
        print 'camera mode is %s' % curr

    @classmethod
    def enable_chat(cls):
        InputGlobal.input = 'chat'

    @classmethod
    def agent_button_state(cls):
        return

    @classmethod
    def agent_mouse_angles(cls):
        return

class Mouse(object):

    def __init__(self):
        pass
        
    def on_mouse_drag(self, x, y, dx, dy, buttons, modifiers=None):
        pass

    def on_mouse_motion(self, x, y, dx, dy):
        pass
        #if InputGlobal.input == 'agent':
            #self._pan_agent(dx, dy)

    #def _pan_agent(self, dx, dy):
        #GameStateGlobal.agent.pan(dx,dy)
        
    #buttonss:
    #1 left, 2 right, 4 scroll up, 5 scroll down
    #state is 0 or 1, 1 if mouse was click, 0 if it was released
    def on_mouse_press(self, x, y, button, state):
        if InputGlobal.input == 'agent':
            if state == 1: #pressed down
                if button == 1:
                    #playSound.build()
                    ##print "fire"
                    GameStateGlobal.agent.fire()
                elif button == 3: #right click
                    GameStateGlobal.agent.set_active_block()
                elif button == 4: #scroll up
                    direction = 'up'
                    GameStateGlobal.agent.weapons.switch(direction)
                elif button == 5: #scroll down
                    direction = 'down'
                    GameStateGlobal.agent.weapons.switch(direction)
            elif state == 0: #mouse button released
                pass

    def clear_mouse_deltas(self):
        print 'clearing mouse deltas'
        cInput.get_mouse_deltas() # discard

class Keyboard(object):

    def __init__(self, main):
        self.main = main
        self.camera = main.camera
        self.agent_camera = main.agent_camera
        self.key_handlers = {}

        self._init_key_handlers()

    def _input_callback(self, callback):
        if callable(callback):
            callback(self)

    #deprecate
    def on_text(self, text):
        if InputGlobal.input == 'chat':
            callback = ChatClientGlobal.chatClient.input.on_text(text)
            self._input_callback(callback)
        else:
            if text == 'y':
                self.toggle_chat()

    # continuous non-character key detection
    #e.g. back space, cursor movement
    def on_text_motion(self, motion):
        if InputGlobal.input == 'chat':
            callback = ChatClientGlobal.chatClient.input.on_text_motion(motion)
            self._input_callback(callback)

    # one-time non character key detection
    # e.g. enter
    def on_key_press(self, symbol):

        if symbol == 'QUIT':
            GameStateGlobal.exit = True
        #elif symbol == 'f1':
            #terrain_map.save_to_disk()

        if InputGlobal.use_voxel_aligner:
            InputGlobal.voxel_aligner.keys(symbol)
            return
            
        if InputGlobal.input == 'chat':
            callback = ChatClientGlobal.chatClient.input.on_key_press(symbol)
            self._input_callback(callback)
        else:
            if symbol == 'y':
                self.toggle_chat()
            if InputGlobal.input == 'agent':
                InputGlobal.agentInput.on_key_press(symbol)
            if symbol == 'tab':
                InputGlobal.scoreboard = not InputGlobal.scoreboard
            if symbol == 'escape':
                GameStateGlobal.exit = True

            self.key_handlers.get(symbol, lambda : None)()

    def on_key_release(self, symbol):
        if InputGlobal.input == 'agent':
            InputGlobal.agentInput.on_key_release(symbol)

    def _init_key_handlers(self):
        self.bind_key_handlers({
            "G" : self.main.world.toggle_mipmap,
            "T" : self.main.world.toggle_gl_smooth,
            "e" : self.toggle_inventory,
            "h" : InputGlobal.toggle_input_mode,
            "g" : InputGlobal.toggle_camera_mode,
            "n" : toggle_t_viz_vbo_indicator_style,
            "m" : toggle_terrain_map_blend_mode,
            "l" : refresh_map_vbo,
            "v" : toggle_z_buffer,
            "p" : _toggle_latency_unit,
            ',' : self.toggle_agent_gravity,
            'u' : cInput.toggle_mouse_bind,
            '/' : self.toggle_hud,
            ';' : self.voxel_aligner_mode_toggle,
            ']' : self.toggle_agent_tick_mode,
        })
        
    # accept key,handler or a dict of key,handlers
    def bind_key_handlers(self, key, handler=None):
        if handler is None:
            assert type(key) == dict
            for k, h in key.items():
                self.key_handlers[k] = h
        else:
            self.key_handlers[key] = handler

    def toggle_hud(self):
        opts.hud = not opts.hud

    def toggle_inventory(self):
        InputGlobal.inventory = not InputGlobal.inventory

    def toggle_chat(self, empty=None):
        if InputGlobal.input == 'chat':
            InputGlobal.toggle_input_mode(0)
        else:
            InputGlobal.input = 'chat'

    def stateHandler(self, keyboard):
        if InputGlobal.use_voxel_aligner:
            return
        if InputGlobal.input == 'chat':
            return
        if InputGlobal.input == 'camera':
            self.camera_input_mode(keyboard)
        elif InputGlobal.input == 'agent':
            self.agent_input_mode(keyboard)

    def voxel_aligner_mode_toggle(self):
        InputGlobal.use_voxel_aligner = not InputGlobal.use_voxel_aligner

    def toggle_agent_gravity(self):
        print 'toggle agent g'
        GameStateGlobal.apply_gravity = not GameStateGlobal.apply_gravity

    def toggle_agent_tick_mode(self):
        print 'toggle agent tick mode'
        if GameStateGlobal.agent_tick_mode == 'jetpack':
            GameStateGlobal.agent_tick_mode = 'jump'
            cAgents.jump_physics()
        else:
            GameStateGlobal.agent_tick_mode = 'jetpack'
            cAgents.jetpack_physics()
        NetOut.miscMessage.agent_tick_mode(GameStateGlobal.agent_tick_mode)

    def agent_input_mode(self, keyboard):
        if GameStateGlobal.agent.dead:
            return
        v = 1
        d_x, d_y, v_x, v_y, jetpack, brake = [0 for i in range(6)]

        u,d,l,r, jetpack, brake = [0 for i in range(6)]
        old_buttons = GameStateGlobal.agent.button_state

        if 'w' in keyboard:
                u=1
        if 's' in keyboard:
                d=1
        if 'a' in keyboard:
                l = 1
        if 'd' in keyboard:
                r=1
        if 'c' in keyboard:
            brake = 1
        if 'SPACE' in keyboard:
            jetpack = 1

        button_state = [u,d,l,r, jetpack, brake]
        GameStateGlobal.agent.button_state = button_state



    def camera_input_mode(self, keyboard):
        v = opts.camera_speed

        if 'w' in keyboard:
            self.camera.move_camera(v,0,0)
        if 's' in keyboard:
            self.camera.move_camera(-v,0,0)
        if 'a' in keyboard:
            self.camera.move_camera(0,v,0)
        if 'd' in keyboard:
            self.camera.move_camera(0,-v,0)
        if 'r' in keyboard:
            self.camera.move_camera(0,0,v)
        if 'f' in keyboard:
            self.camera.move_camera(0,0,-v)


# only calls method if GameStateGlobal.agent is not None
def requireAgent(f):
    def requireAgent_wrap(*args, **kwargs):
        if GameStateGlobal.agent is not None:
            f(*args, **kwargs)
    return requireAgent_wrap

class AgentInput:

    def __init__(self):
        self.key_press_handlers = {}
        self.key_release_handlers = {}
        self._init_key_handlers()

    def _init_key_handlers(self):
        self.key_press_handlers = {
            "r" : self.reload,
            "1": self.switch_weapon,
            "2": self.switch_weapon,
            "3": self.switch_weapon,
            "4": self.switch_weapon,
            "5": self.switch_weapon,
            "6": self.switch_weapon,
            "7": self.switch_weapon,
            "8": self.switch_weapon,
            "9": self.switch_weapon,
            "0": self.switch_weapon,
            'left':self.adjust_block,
            'right':self.adjust_block,
            'up':self.adjust_block,
            'down':self.adjust_block,
            'b'   : self.bleed,
            'left ctrl': self.crouch,
        }

        self.key_release_handlers = {
            'left ctrl' :   self.crouch,
        }

    def on_key_press(self, symbol, modifiers=None):
        self.key_press_handlers.get(symbol, lambda s: None)(symbol)

    def on_key_release(self, symbol):
        self.key_release_handlers.get(symbol, lambda s: None)(symbol)

    @classmethod
    @requireAgent
    def reload(cls, symbol=None, modifiers=None):
        GameStateGlobal.agent.reload()

    @classmethod
    @requireAgent
    def bleed(cls, *args, **kwargs):
        GameStateGlobal.agent.bleed()

    @classmethod
    @requireAgent
    def crouch(cls, *args, **kwargs):
        GameStateGlobal.agent.crouch()

    @classmethod
    @requireAgent
    def switch_weapon(cls, symbol=None, modifiers=None):
        try:
            weapon_index = int(symbol)
        except (ValueError, TypeError):
            pass
        else:
            GameStateGlobal.agent.weapons.switch(weapon_index)

    @classmethod
    @requireAgent
    def adjust_block(cls, symbol=None, modifiers=None):
        aw = GameStateGlobal.agent.weapons.active()

        if not aw or aw.type != 3:  # block applier
            return
        if symbol == 'left':
            InputGlobal.block_selector.left()
        elif symbol == 'right':
            InputGlobal.block_selector.right()
        elif symbol == 'up':
            InputGlobal.block_selector.up()
        elif symbol == 'down':
            InputGlobal.block_selector.down()
        GameStateGlobal.agent.set_active_block(InputGlobal.block_selector.get_texture_id())   # +1 because used 0-index when created mapping, but cube_list stores them 1-indexed (0 is reserved for block absence)


class BlockSelector:

    def __init__(self, x, y, block_ids):
        self.x = x
        self.y = y
        self.n = x*y
        assert len(block_ids) == self.n
        self.active = 0

    def __setattr__(self, k, v):
        self.__dict__[k] = v
        if k == 'active':
            cHUD.hud_control_input(self.active)

    def vertical(self, up=True):
        shift = -1 if up else 1
        row = self.active // self.x
        col = self.active % self.x

        row = (row + shift) % self.y
        new = (row * self.x) + col

        if new < 0 or new > self.n - 1:
            #print 'warning, block selector attempted to select block out of range'
            return
        self.active = new

    def up(self):
        self.vertical(up=True)

    def down(self):
        self.vertical(up=False)

    def horizontal(self, left=True):
        shift = -1 if left else 1
        row = self.active // self.x
        col = self.active % self.x

        new = (col + shift) % self.x
        new += row * self.x

        if new < 0 or new > self.n - 1:
            #print 'warning, block selector attempted to select block out of range'
            return
        self.active = new

    def left(self):
        self.horizontal(left=True)

    def right(self):
        self.horizontal(left=False)

    def get_texture_id(self):
        return cHUD.get_selected_cube_id()

from math import pi
class VoxelAligner:

    def __init__(self):
        self.num_parts = 6
        self.current_part = 0
        self.increment = 0.1
        self.rot_increment = 0.1 * 360 * 4
        import dat.agent_dim as dat
        self.dat = dat

    def keys(self, symbol):
        print 'aligner:: ', symbol
        if symbol == 'l':
            self.switch_part()
        elif symbol == 'w':
            self.move_part_forward()
        elif symbol == 'a':
            self.move_part_left()
        elif symbol == 's':
            self.move_part_backward()
        elif symbol == 'd':
            self.move_part_right()
        elif symbol == 'r':
            self.move_part_up()
        elif symbol == 'f':
            self.move_part_down()

        elif symbol == 'u':
            self.rot1()
        elif symbol == 'i':
            self.rot2()
        elif symbol == 'o':
            self.rot3()

        elif symbol == 'm':
            self.save()

        elif symbol == ';':
            InputGlobal.use_voxel_aligner = False

        elif symbol == 'h':
            self.change_speed()

    def switch_part(self):
        self.current_part = (self.current_part + 1) % self.num_parts

    def move_part_forward(self):
        print 'moving part forward'
        self.dat.lu2[self.current_part][1] += self.increment

    def move_part_backward(self):
        self.dat.lu2[self.current_part][1] -= self.increment

    def move_part_left(self):
        self.dat.lu2[self.current_part][2] += self.increment

    def move_part_right(self):
        self.dat.lu2[self.current_part][2] -= self.increment

    def move_part_up(self):
        self.dat.lu2[self.current_part][3] += self.increment
        
    def move_part_down(self):
        self.dat.lu2[self.current_part][3] -= self.increment
        
    def rot1(self):
        self.dat.lu3[self.current_part][0] += self.rot_increment
    def rot2(self):
        self.dat.lu3[self.current_part][1] += self.rot_increment
    def rot3(self):
        self.dat.lu3[self.current_part][2] += self.rot_increment

    def change_speed(self):
        self.increment = 0.01 if self.increment == 0.1 else 0.1

    def save(self):
        self.dat.save()
    

from game_state import GameStateGlobal
from chat_client import ChatClientGlobal
from net_out import NetOut

inputEventGlobal = InputEventGlobal()
cInput.set_input_callback(inputEventGlobal)
