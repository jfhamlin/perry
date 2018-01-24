import math
import pyribbed
from pyribbed import Vector, mag, mag_squared, normalize, dot, cross
from pyribbed import sweep, hermite_ribbed_surface

NUM_SAMPLES = 50
class MySculpture(pyribbed.SculptureProgram):
    def __init__(self):
        pyribbed.SculptureProgram.__init__(self)

        self.param_slider_int("Num. Ribs", 20, 200, 4, 50)
        self.param_slider_float("Rib Radius", 0.05, 2.0, 0.05, 0.1)
        self.param_slider_float("Rail Radius", 0.05, 2.0, 0.05, 0.2)
        self.param_color_chooser("Background Color", (1, 1, 1))

        self.instances = set()

    def update(self, params):
        self.set_background_color(params["Background Color"])

        self.circle1 = self.circle(Vector(0, 0, 0),
                                   Vector(18, 0, 0), Vector(0, 0, 18),
                                    0.0, math.pi, NUM_SAMPLES)
        self.circle2 = self.circle(Vector(0, -5, 0),
                                   Vector(20, 0, 0), Vector(0, 0, 20),
                                   0.0, math.pi, NUM_SAMPLES)

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

        self.rail1 = sweep(self.railCross, self.circle1)
        self.rail2 = sweep(self.railCross, self.circle2)

        def ribParams1(end0, frenet0, end1, frenet1):
            return (end0 + 2.0 * (frenet0.normal),
                    end1 + 2.0 * (frenet1.normal))
        def ribParams2(end0, frenet0, end1, frenet1):
            return (end0 + 2.0 * (-frenet0.normal),
                    end1 + 2.0 * (-frenet1.normal))

        halfNumRibs = params["Num. Ribs"] / 2
        ribSpacing = (0.98 - 0.02) / (halfNumRibs * 2)
        self.ribsRails = pyribbed.hermite_ribbed_surface(
            self.circle1, (0.02, 0.98 - ribSpacing),
            self.circle2, (0.02 + ribSpacing, 0.98),
            ribParams1, halfNumRibs, self.ribCross)
        self.ribsRails += pyribbed.hermite_ribbed_surface(
            self.circle1, (0.98, 0.02 + ribSpacing),
            self.circle2, (0.98 - ribSpacing, 0.02),
            ribParams2, halfNumRibs, self.ribCross)

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
