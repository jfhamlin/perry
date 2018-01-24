from pyribbed import Vector, Transformation, concatenate

class Group:
    def __init__(self):
        self.xform = Transformation()
        self.elements = []

    def add_elements(self, el):
        if type(el) == list:
            self.elements += el
        else:
            self.elements.append(el)

    def set_transform(self, xform):
        self.xform = Transformation(xform)

    def instantiate(self, instanceFunc, parentXform = Transformation()):
        xform = concatenate(parentXform, self.xform)
        for e in self.elements:
            if isinstance(e, Group):
                e.instantiate(instanceFunc, xform)
            else:
                instanceFunc(e, xform)

    def copy(self):
        g = Group()
        g.xform = Transformation(self.xform)
        for el in self.elements:
            if type(el) == Group:
                g.elements.append(el.copy())
            else:
                g.elements.append(el)
        return g
