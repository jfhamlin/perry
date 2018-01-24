import math
import pyribbed
from pyribbed import Vector, mag, mag_squared, normalize, dot, cross
from pyribbed import sweep, hermite_ribbed_surface

NUM_SAMPLES = 100
class MySculpture(pyribbed.SculptureProgram):
    def __init__(self):
        pyribbed.SculptureProgram.__init__(self)

        self.param_slider_int("Num. Ribs", 20, 200, 4, 50)

        self.param_slider_float("Rib Radius", 0.05, 2.0, 0.05, 0.1)
        self.param_slider_float("Rail Radius", 0.05, 2.0, 0.05, 0.25)

        self.param_slider_float("Param Gb0", -5.0, 5.0, 0.05, 0.0)
#        self.param_slider_float("Param Gb1", -5.0, 5.0, 0.05, 1.0)
        self.param_slider_float("Param Ge0", -5.0, 5.0, 0.05, 0.5)
#        self.param_slider_float("Param Ge1", -5.0, 5.0, 0.05, 1.5)

        self.param_slider_float("Rib Bend", -15.0, 15.0, 0.1, 0)

        self.param_color_chooser("Background Color", (1, 1, 1))
        self.param_color_chooser("Sculpture Color", (.8, .8, .2))

        self.instances = set()

    def update(self, params):
        

        self.set_background_color(params["Background Color"])

        self.rail = pyribbed.BSplineCurve()

        h = 0.5
        z = 0.2
        s = 0.5
        b = 0.6
        a = 0.6
        f = 1.0
        t = 0.0

        p1a = 10 * Vector(f * (h - s),  f * (h + s),  5 * z + t * s)
        p1b = 10 * Vector(f * (h + s),  f * (h - s),  5 * z)
        p2  = 10 * Vector(3 * a * h,    -2 * b * h,   0)
        p3a = 10 * Vector((-h - s) / f, (h - s) / f,  -5 * z - t * z)
        p3b = 10 * Vector((-h + s) / f, (h + s) / f,  -5 * z)
        p4  = 10 * Vector(2 * b * h,    3 * a * h,    0)
        p5a = 10 * Vector(f * (-h + s), f * (-h - s), 5 * z + t * s)
        p5b = 10 * Vector(f * (-h - s), f * (-h + s), 5 * z)
        p6  = 10 * Vector(-3 * a * h,   2 * b * h,    0)
        p7a = 10 * Vector((h + s) / f,  (-h + s) / f, -5 * z - t * s)
        p7b = 10 * Vector((h - s) / f,  (-h - s) / f, -5 * z)
        p8  = 10 * Vector(-2 * b * h,   -3 * a * h,    0)

        self.rail.push_back(p1a)
        self.rail.push_back(p1b)
        self.rail.push_back(p2)
        self.rail.push_back(p3a)
        self.rail.push_back(p3b)
        self.rail.push_back(p4)
        self.rail.push_back(p5a)
        self.rail.push_back(p5b)
        self.rail.push_back(p6)
        self.rail.push_back(p7a)
        self.rail.push_back(p7b)
        self.rail.push_back(p8)

        self.rail.set_closed(True)

        # Sweep radii
        ribRad = params["Rib Radius"]
        self.ribCross = self.circle(Vector(ribRad, 0, 0),
                                    Vector(0, ribRad, 0),
                                    0.0, 2.0 * math.pi, 8)
        railRad = params["Rail Radius"]
        self.railCross = self.circle(Vector(railRad, 0, 0),
                                     Vector(0, railRad, 0),
                                     0.0, 2.0 * math.pi, 8)

        railSweep = sweep(self.railCross, self.rail)
        bend = params["Rib Bend"]
        def ribParams(end0, frenet0, end1, frenet1):
            return (end0 - bend * frenet0.normal,
                    end1 - bend * frenet1.normal)

        gb0 = params["Param Gb0"]
        ge0 = params["Param Ge0"]
        gb1 = gb0 + 1.0
        ge1 = ge0 + 1.0

        self.ribs = pyribbed.hermite_ribbed_surface(
            self.rail, (gb0, gb1),
            self.rail, (ge0, ge1),
            ribParams, params["Num. Ribs"], self.ribCross)

        for rib in self.ribs:
            self.instance(rib, params["Sculpture Color"])
        self.instance(railSweep, params["Sculpture Color"])

        self.light(Vector(50, 50, 50), Vector(1, 1, 1), 1)
        self.light(Vector(-50, 50, -50), Vector(0.4, 0.1, 1), 0.1)

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
