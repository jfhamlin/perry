import math
from math import pi, cos, sin
import pyribbed
from pyribbed import Vector, mag, mag_squared, normalize, dot, cross, normal
from pyribbed import sweep, hermite_ribbed_surface

NUM_SAMPLES = 50
class MySculpture(pyribbed.SculptureProgram):
    def __init__(self):
        pyribbed.SculptureProgram.__init__(self)

        self.param_slider_int("Num. Ribs", 20, 200, 4, 50)
        self.param_slider_float("Rib Radius", 0.05, 2.0, 0.05, 0.05)
        self.param_slider_float("Rail Radius", 0.05, 2.0, 0.05, 0.08)

        self.param_slider_float("Width", 5, 40, 1, 7)
        self.param_slider_float("Height", 5, 40, 1, 10)
        self.param_slider_float("Base Radius", 0.5, 10, 0.5, 2)
        self.param_slider_float("Apex Radius", 0.5, 10, 0.5, 2)

        self.param_slider_float("Rib Bend", 0.0, 5, 0.5, 1.0)
        self.param_slider_int("Twists", 0, 100, 1, 10)
        self.param_slider_float("Twist Offset", 0, 360, 10, 180)

        self.param_color_chooser("Background Color", (1, 1, 1))

        self.instances = set()

    def ellipse_pos(self, width, height, t):
        x = 0.5 * width * cos(t * pi)
        y = height * sin(t * pi)
        return Vector(x, y, 0)

    def ellipse_norm(self, width, height, t):
        x = - width * cos(t * pi)
        y = - height * sin(t * pi)
        return normal(Vector(x, y, 0))

    def update(self, params):
        self.set_background_color(params["Background Color"])

        self.path1 = pyribbed.Polyline()
        self.path2 = pyribbed.Polyline()
        twists = params["Twists"]
        offset = params["Twist Offset"]
        baseRadius = params["Base Radius"]
        apexRadius = params["Apex Radius"]
        segments = 1000
        for i in xrange(segments + 1):
            t = float(i) / segments
            radius = (self.ellipse_pos(1, apexRadius - baseRadius, t)[1] +
                      baseRadius)
            angle = 2 * pi * t * twists + offset
            pos = self.ellipse_pos(params["Width"], params["Height"], t)
            dir = self.ellipse_norm(params["Width"], params["Height"], t)

            p1 = cos(angle) * dir + Vector(0, 0, sin(angle))
            p2 = cos(angle + pi) * dir + Vector(0, 0, sin(angle + pi))
            p1 *= radius
            p2 *= radius
            p1 += pos
            p2 += pos

            self.path1.push_back(p1)
            self.path2.push_back(p2)

        # Sweep radii
        ribRad = params["Rib Radius"]
        self.ribCross = self.circle(Vector(0, 0, 0),
                                    Vector(ribRad, 0, 0),
                                    Vector(0, ribRad, 0),
                                    0.0, 2.0 * math.pi, 8)
        railRad = params["Rail Radius"]
        self.railCross = self.circle(Vector(0, 0, 0),
                                     Vector(railRad, 0, 0),
                                     Vector(0, railRad, 0),
                                     0.0, 2.0 * math.pi, 8)

        self.rail1 = sweep(self.railCross, self.path1)
        self.rail2 = sweep(self.railCross, self.path2)

        bend = params["Rib Bend"]
        def ribParams(end0, frenet0, end1, frenet1, t):
            mag = 2.0 * t - 1.0
            return (end0 + mag * bend * (frenet0.binormal),
                    end1 + mag * bend * (frenet1.binormal))

        numRibs = params["Num. Ribs"]
        self.ribsRails = pyribbed.hermite_ribbed_surface(
            self.path1, (0.02, 0.98),
            self.path2, (0.02, 0.98),
            ribParams, numRibs, self.ribCross)

        for rib in self.ribsRails:
            self.instance(rib)
        self.instance(self.rail1)
        self.instance(self.rail2)

        self.light(Vector(50, 50, 50), Vector(1, 1, 1), 1)
        self.light(Vector(-50, -50, -50), Vector(0.4, 0.1, 1), 0.1)

    def circle(self, center, xVec, yVec, startAngle, endAngle, samples):
        sc = pyribbed.Polyline()
        for i in xrange(samples):
            t = float(i) / float(samples - 1)
            angle = t * (endAngle - startAngle) + startAngle
            sample = math.cos(angle) * xVec + math.sin(angle) * yVec
            sc.push_back(center + sample)
        return sc

s = MySculpture()
s.begin_main_loop()
