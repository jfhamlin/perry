import math
import pyribbed
from pyribbed import Vector, mag, mag_squared, normalize, dot, cross
from pyribbed import sweep, hermite_ribbed_surface

NUM_SAMPLES = 100
class MySculpture(pyribbed.SculptureProgram):
    def __init__(self):
        pyribbed.SculptureProgram.__init__(self)

        self.param_slider_int("Num. Ribs", 20, 100, 4, 50)
        self.param_slider_float("Rib Radius", 0.05, 2.0, 0.05, 0.25)
        self.param_slider_float("Rail Radius", 0.05, 2.0, 0.05, 0.3)
        self.param_color_chooser("Background Color", (.7, .85, 1))
        self.param_color_chooser("Sculpture Color", (1, 1, 1))
        self.param_slider_float("Distribution", 0.0, 1.0, 0.1, 0.7)
        self.param_slider_float("Rib Bend", -15.0, 15.0, 0.1, 0)

        self.instances = set()

    def update(self, params):
        

        self.set_background_color(params["Background Color"])

        self.circleYZ = self.circle(Vector(0, 20, 0), Vector(0, 0, 20),
                                    0.0, math.pi, NUM_SAMPLES)
        self.circleXZ = self.circle(Vector(20, 0, 0), Vector(0, 0, -20),
                                    0.0, math.pi, NUM_SAMPLES)

        # Sweep radii
        ribRad = params["Rib Radius"]
        self.ribCross = self.circle(Vector(ribRad, 0, 0),
                                    Vector(0, ribRad, 0),
                                    0.0, 2.0 * math.pi, 8)
        railRad = params["Rail Radius"]
        self.railCross = self.circle(Vector(railRad, 0, 0),
                                     Vector(0, railRad, 0),
                                     0.0, 2.0 * math.pi, 8)

        self.yzSweep = sweep(self.railCross, self.circleYZ)
        self.xzSweep = sweep(self.railCross, self.circleXZ)

        bend = params["Rib Bend"]
        def ribParams(end0, frenet0, end1, frenet1):
            return (end0 - bend * frenet0.normal,
                    end1 - bend * frenet1.normal)

#            return (end0, end1)

        dist = params["Distribution"]
        def ribMap(t):
            return dist * (4.0 * (t - 0.5) ** 3 + 0.5) + (1.0 - dist) * t

        numRibs = params["Num. Ribs"]
        offset = 0.25 / (numRibs + 2)

        self.ribsRails = pyribbed.hermite_ribbed_surface(
            self.circleYZ, (offset, 1 - offset),
            self.circleXZ, (offset, 1 - offset),
            ribParams, ribMap, numRibs, self.ribCross)

        for rib in self.ribsRails:
            self.instance(rib, params["Sculpture Color"])
        self.instance(self.yzSweep, params["Sculpture Color"])
        self.instance(self.xzSweep, params["Sculpture Color"])

        self.light(Vector(50, 50, 50), Vector(1, 1, 1), 1)
        self.light(Vector(-50, -50, -50), Vector(0.4, 0.4, 0.2), 0.1)

    def circle(self, xVec, yVec, startAngle, endAngle, samples):
        sc = pyribbed.Polyline()
        for i in xrange(samples):
            t = float(i) / float(samples - 1)
            angle = t * (endAngle - startAngle) + startAngle
            sample = math.cos(angle) * xVec + math.sin(angle) * yVec
            sc.push_back(sample)
        return sc

s = MySculpture()
s.begin_main_loop()
