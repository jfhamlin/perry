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
        self.param_color_chooser("Background Color", (1, 1, 1))
        self.param_color_chooser("Sculpture Color", (1, 1, 1))

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

        # Set up the intervals of the rail curves for the four
        # ribbed surface sweeps.
        ribsPerSurf = params["Num. Ribs"] / 4
        numRibs = 4 * ribsPerSurf
        endOffset = 0.01
        ribSep = (1.0 - 2.0 * endOffset) / (numRibs - 1)
        ivals = [0, 0, 0, 0]
        surfParamWidth = ribSep * (ribsPerSurf - 1)
        for i in xrange(4):
            start = endOffset + float(i) * (surfParamWidth + ribSep)
            ivals[i] = (start, start + surfParamWidth)

        reverse = lambda ival: (ival[1], ival[0])
        def ribParams(end0, frenet0, end1, frenet1):
            return (end0 + -10.0 * frenet0.normal,
                    end1 + -10.0 * frenet1.normal)
        self.ribsRails = pyribbed.hermite_ribbed_surface(
            self.circleYZ, ivals[0],
            self.circleXZ, ivals[2],
            ribParams, ribsPerSurf, self.ribCross)

        self.ribsRails += pyribbed.hermite_ribbed_surface(
            self.circleYZ, ivals[1],
            self.circleXZ, reverse(ivals[0]),
            ribParams, ribsPerSurf, self.ribCross)

        self.ribsRails += pyribbed.hermite_ribbed_surface(
            self.circleYZ, ivals[2],
            self.circleXZ, reverse(ivals[3]),
            ribParams, ribsPerSurf, self.ribCross)

        self.ribsRails += pyribbed.hermite_ribbed_surface(
            self.circleYZ, ivals[3],
            self.circleXZ, ivals[1],
            ribParams, ribsPerSurf, self.ribCross)

        for rib in self.ribsRails:
            self.instance(rib, params["Sculpture Color"])
        self.instance(self.yzSweep, params["Sculpture Color"])
        self.instance(self.xzSweep, params["Sculpture Color"])

        self.light(Vector(50, 50, 50), Vector(1, 1, 1), 1)
#        self.light(Vector(-50, -50, -50), Vector(0.4, 0.1, 1), 0.1)

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
