import math
from math import cos, sin
import pyribbed
from pyribbed import *
from group import Group

NUM_SAMPLES = 100
class MySculpture(pyribbed.SculptureProgram):
    def __init__(self):
        pyribbed.SculptureProgram.__init__(self)

        self.param_slider_int("Num. Ribs", 3, 20, 1, 4)

        self.param_slider_float("Rib Radius", 0.05, 2.0, 0.05, 0.1)
        self.param_slider_float("Rail Radius", 0.05, 2.0, 0.05, 0.25)

        self.param_slider_float("radius", 3.0, 15.0, 0.1, 5.0)
        self.param_slider_float("Radius", 3.0, 15.0, 0.1, 5.0)
        self.param_slider_float("separation", 0.0, 20.0, 0.1, 13.0)

        self.param_slider_float("Rib Bend", -1.0, 1.0, 0.1, 0.0)
        self.param_slider_float("Cap Scale", 0.0, 3.0, 0.1, 1.0)
        self.param_slider_float("Cap Rotation", 0.0, 360.0, 1.0, 0.0)

        self.param_color_chooser("Background Color", (0, 0, 0))
        self.param_color_chooser("Sculpture Color", (1, 1, 1))

        self.instances = set()

    def update(self, params):
        
        self.set_background_color(params["Background Color"])

        radius = params["Radius"]
        capScale = params["Cap Scale"]
        capRadius = radius * capScale
        radDiff = capRadius - radius
        scaleFunc = lambda x: radius + x * radDiff

        capRot = math.pi / 180.0 * params["Cap Rotation"]
        capRotDelta = 0.33333333 * capRot
        deltaAngle = 2.0 * math.pi / 5.0
        self.rails = [pyribbed.BezierCurve(3) for x in xrange(5)]
        for i in xrange(5):
            angle = i * deltaAngle
            self.rails[i][0] = (radius *
                                Vector(cos(angle),
                                       sin(angle), 0.0))
            angle += capRotDelta
            self.rails[i][1] = Vector(scaleFunc(0.25) * cos(angle),
                                      scaleFunc(0.25) *sin(angle),
                                      radius * 0.25)
            angle += capRotDelta
            self.rails[i][2] = Vector(scaleFunc(0.75) * cos(angle),
                                      scaleFunc(0.75) *sin(angle),
                                      radius * 0.75)
            angle += capRotDelta
            self.rails[i][3] = Vector(scaleFunc(1.0) * cos(angle),
                                      scaleFunc(1.0) * sin(angle), radius)

        # Sweep radii
        ribRad = params["Rib Radius"]
        self.ribCross = self.circle(Vector(ribRad, 0, 0),
                                    Vector(0, ribRad, 0),
                                    0.0, 2.0 * math.pi, 8)
        railRad = params["Rail Radius"]
        self.railCross = self.circle(Vector(railRad, 0, 0),
                                     Vector(0, railRad, 0),
                                     0.0, 2.0 * math.pi, 8)

    #     bend = params["Rib Bend"]
        def ribParams(end0, frenet0, end1, frenet1):
            return (end0, end1)

        center = Group()
        for i in xrange(5):
            rail0 = self.rails[i]
            rail1 = self.rails[(i + 1) % 5]
            center.add_elements(pyribbed.hermite_ribbed_surface(
                    rail0, (0, 1), rail1, (0, 1),
                    ribParams, params["Num. Ribs"], self.ribCross))

        for rail in self.rails:
            center.add_elements(sweep(self.railCross, rail))

        ## Make the first satellite.
        rot = Rotation(Vector(0, 0, 1), math.pi / 180.0 * 108.0)
        xform0 = Transformation(rot);
        apothem = radius * 0.809017 # params["apothem"]
        xform0.set_translation(Translation(-apothem, 0, 0))
        rot = Rotation(Vector(0, -1, 0), 1.10715)
        xform1 = Transformation(rot)
        xform1.set_translation(Translation(-apothem, 0, 0))
        sat0 = center.copy()
        sat0.set_transform(concatenate(xform1, xform0))

        center.add_elements(sat0)

        ## Make the other 4 satellites.
        for i in xrange(4):
            rot = Rotation(Vector(0, 0, 1),
                           float(i + 1) * math.pi / 180.0 * 72.0)
            sat = Group()
            sat.add_elements(sat0.copy())
            sat.set_transform(rot)
            center.add_elements(sat)

        ## Make the other hemisphere.
        c2 = center.copy()
        rot = Rotation(Vector(0, 1, 0), math.pi)
        separation = params["separation"]
        c2.set_transform(Transformation(rot))
        c2.xform.set_translation(Translation(0, 0, -separation))

        ## Instantiate!
        def instanceFunc(swp, xform):
            self.instance(swp, xform, params["Sculpture Color"])
        dodec = Group()
        dodec.add_elements([center, c2])
        dodec.instantiate(instanceFunc)

        #dodec.instantiate(instanceFunc,
        #Transformation(Translation(0, 0, 2*separation)))

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
