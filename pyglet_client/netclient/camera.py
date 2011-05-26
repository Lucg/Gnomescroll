from math import sin, cos, pi

from pyglet.gl import *
from pyglet import clock, font, image, window

base_dir = "./"

class Camera(object):

    def __init__(self, win, x=0.0, y=0.0, z=0.0, rot=0.0, zoom=1.0):
        self.win = win
        self.x = x
        self.y = y
        self.z = z
        self.x_angle = 0.0
        self.y_angle = 0.0
        self.rot = rot
        self.zoom = zoom
        self.rts = True

    def worldProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        aspect = self.win.width / float(self.win.height)
        #gluPerspective( 45.0 / self.scale, aspect, 0.1, 100.0);
        gluPerspective(65, aspect, .1, 1000)


        glMatrixMode( GL_MODELVIEW )
        glLoadIdentity()

        #camera_focus_x = self.x + cos( self.x_angle * pi)
        #camera_focus_y = self.y + sin( self.x_angle * pi)
        #camera_focus_z = self.z + sin( self.y_angle)

        camera_focus_x = self.x + cos( self.x_angle * pi) * cos( self.y_angle * pi)
        camera_focus_y = self.y + sin( self.x_angle * pi) * cos( self.y_angle * pi)
        camera_focus_z = self.z + sin( self.y_angle)

        gluLookAt( self.x, self.y, self.z,
                camera_focus_x, camera_focus_y, camera_focus_z,
                0., 0., 1.0)

        glEnable (GL_DEPTH_TEST)
        #glEnable(GL_CULL_FACE);

    def hudProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluOrtho2D(0, self.win.width, 0, self.win.height)

        glMatrixMode( GL_MODELVIEW )
        glLoadIdentity()

        glDisable(GL_DEPTH_TEST);
        #glDisable(GL_CULL_FACE);
        glEnable(gl.GL_TEXTURE_2D) 

    def move_camera(self, dx, dy, dz):

        if self.rts == True:
            #dx delta
            self.x += dx*cos(self.x_angle * pi)
            self.y += dx*sin(self.x_angle * pi)
            #dy delta
            self.x += dy*cos(self.x_angle * pi + pi/2)
            self.y += dy*sin(self.x_angle * pi + pi/2)
        else:
            self.x += dx
            self.y += dy
        #dz delta
        self.z += dz

    def pan(self, dx_angle, dy_angle):
        #print "dx_angle= " + str(dx_angle)
        #print "dy_angle= " + str(dy_angle)
        self.x_angle += dx_angle
        self.y_angle += dy_angle
        if self.y_angle < -0.499:
            self.y_angle = -0.499
        if self.y_angle > 0.499:
            self.y_angle = 0.499

    def on_resize(self, width, height):
        print "Resize Window"
        glViewport(0, 0, width, height)
        self.worldProjection()
        return pyglet.event.EVENT_HANDLED

    def agent_view(self, agent):
        self.x = agent.x
        self.y = agent.y
        self.z = agent.z
        self.x_angle = agent.x_angle
        self.y_angle = agent.y_angle
