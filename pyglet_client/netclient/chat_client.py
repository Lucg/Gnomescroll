#!/usr/bin/python

from time import time as now
from collections import deque

class ChatServerGlobal:
    chatServer = None

    def __init__(self): #first pass is declaring
        ChatServerGlobal.chat = ChatServer()
    @classmethod
    def init(self): #calls import methods if needed
        pass

from net_client import NetClientGlobal
from net_out import NetOut
from net_event import NetEventGlobal

'''
Chat client
'''

class Chat:

    CURRENT_CHANNEL = 'global'
    ignored = []
    subscriptions = {}

    def __init__(self, channel=None):
        #NetClientGlobal.chat = self
        channel is not None or self.set_CURRENT_CHANNEL(channel)
        self.subscribe('system')

    def set_current_channel(self, channel):
        channel = str(channel)
        if self.CURRENT_CHANNEL != channel:
            self.unsubscribe(self.CURRENT_CHANNEL)
        self.CURRENT_CHANNEL = channel
        self.subscribe(self.CURRENT_CHANNEL)

    def subscribe(self, channel):
        if channel == 'system':
            self.subscriptions.setdefault(channel, SystemChannel(channel))
        else:
            self.subscriptions.setdefault(channel, Channel(channel))

    def unsubscribe(self, channel):
        if channel == 'system':
            return
        del self.subscriptions[channel]

    def ignore(self, client_id):
        client_id in self.ignored or self.ignored.append(client_id)

    def unignore(self, client_id):
        client_id in self.ignored and self.ignored.remove(client_id)

    def send(self, text):
        if not NetClientGlobal.client_id:
            print 'Client_id is not set; cannot send chat msg'
            return

        if text[0] == '/':
            msg = ChatCommand(text).send()
        else:
            msg = ChatMessageOut(text).send()

    def receive(self, msg):
        if channel in subscriptions:
            msg = ChatMessageIn(msg)
            if msg.payload.client_id in self.ignored:
                return
            subscriptions[channel].receive(msg)

# channel wrapper
class Channel:

    def __init__(self, name):
        self.name = name
        self.history = deque([], 200)

    def receive(self, msg):
        if msg.valid:
            self.history.appendleft(msg)

# special channel for system msgs (has extra processing specific to /commands)
class SystemChannel(Channel):

    def receive(self, msg):
        log = None
        if msg.content == 'ping':
            log = Payload({
                'content'  : 'Chat ping round-trip time: ' + (int(now()) - int(msg.time)),
                'channel'  : 'system',
                'client_id': 'system',
                'cmd'      : 'chat',
            })

        if log is not None:
            self.history.appendleft(log)

# command message (e.g. /channel highlands i am messaging the highlands channel)
class ChatCommand():

    def __init__(self, text):
        text_pts = text.split(' ')
        command = text_pts[0][1:]
        args = text_pts[1:]
        self.route(command, args)
        self._send = None
        self.payload = None

    # create a special payload and/or a special _send command
    def route(self, command, args):
        _send = None
        payload = None
        if command == 'channel':
            payload = Payload({
                'content'  : str(' '.join(args[1:])),
                'channel'  : args[0]
            })

        elif command == 'version':
            def _send():
                NetClientGlobal.chat.receive(Payload({
                    'content' : 'DCMMO Client version: ' + NetClientGlobal.VERSION,
                    'channel' : 'system'
                }).serialize())

        elif command == 'ping':
            payload = Payload({
                'channel' : 'system',
                'content' : 'ping'
            })

        self.payload = payload
        self._send = _send

    def send(self):
        if self._send is not None:
            self._send()
        else:
            if self.payload is not None:
                NetClientGlobal.sendMessage.send_chat(self.payload.serialize())

# msg to be sent
class ChatMessageOut():

    def __init__(self, text):
        self.payload = Payload({
            'content' : str(text),
            'channel' : Chat.CURRENT_CHANNEL
        })
        self.payload.clean()
        self.valid = self.payload.valid()

    def send(self):
        NetClientGlobal.sendMessage.send_chat(self.payload.serialize()) # fix this reference
        print 'Sent chat message'

# msg received
class ChatMessageIn():

    def __init__(self, msg):
        self.payload = Payload(msg)
        self.payload.clean()
        self.valid = self.payload.valid()
        self.timestamp = int(now())

# msg payload, attached to a ChatMessageIn/Out or (optionally) ChatCommand
class Payload:

    properties = [
        'cmd',
        'content',
        'time',
        'channel'
    ]

    def __init__(self, **kwargs):
        self.cmd = kwargs.get('cmd', 'chat')
        self.content = kwargs.get('content', '')
        self.time = kwargs.get('time', int(now()))
        self.channel = kwargs.get('channel', '')
        self.valid()

    # checks if all properties are in payload
    def valid(self, properties=None):
        properties = properties or self.properties
        valid = True
        for prop in properties:
            if getattr(self, p, None) is None:
                valid = False
        self.valid = valid
        return self.valid

    def serialize(self):
        d = {}
        for p in self.properties:
            d[p] = getattr(self, p, None)
        return d
