#include "MainWindow.h"
#include "Math/BSplineCurve.h"
#include "Math/BezierCurve.h"
#include "Math/Polyline.h"
#include "Math/Transformations.h"
#include "Math/Vector.h"
#include "Render/Color.h"
#include "Render/PointLight.h"
#include "Resource/MutableGeometryChunk.h"
#include "RibbedScene.h"
#include "Utility/SceneToRIB.h"
#include "Utility/SceneToSTL.h"
#include "glutMaster.h"
#include "glutWindow.h"
#include <GL/glui.h>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <iostream>
#include <string>

using namespace romulus;
using namespace romulus::math;
using namespace romulus::render;

using namespace std;
using namespace boost;
using namespace boost::python;

GlutMaster* g_GlutMaster;
MainWindow* g_MainWindow;
class SculptureProgram* g_Sculpture = 0;

#define CHECK_3TUPLE(tup) \
    /** do nothing **/

object VecGet(object self, object key) {
    int i = extract<int>(key);
    Vector3& s = extract<Vector3&>(self);
    return object(s[i]);
}

void VecSet(object self, object key, object val) {
    Vector3& s = extract<Vector3&>(self);
    int i = extract<int>(key);
    double v = extract<double>(val);
    s[i] = v;
}

object BezGet(object self, object key) {
    int i = extract<int>(key);
    BezierCurve& b = extract<BezierCurve&>(self);
    return object(b[i]);
}

void BezSet(object self, object key, object val) {
    BezierCurve& b = extract<BezierCurve&>(self);
    int i = extract<int>(key);
    Vector3 v = extract<Vector3>(val);
    b[i] = v;
}

//! GUI Control classes
class Control
{
public:
    virtual ~Control() { }

    virtual void Create(GLUI* parent) = 0;

    virtual void AddValue(dict d) = 0;
};

void CreateSlider(GLUI_Node* p,
                  real_t min, real_t max, real_t step,
                  const std::string& title,
                  real_t* destValPtr)
{
    GLUI_Panel* pan = new GLUI_Panel(p, 0);
    GLUI_EditText* et = new GLUI_EditText(pan, title.c_str(), destValPtr,
                                          -1); //, &UpdateCallback);
    new GLUI_Column(pan, false);
    GLUI_Scrollbar* sb = new GLUI_Scrollbar(pan, title.c_str(),
                                            GLUI_SCROLL_HORIZONTAL,
                                            destValPtr, -1); //, &UpdateCallback);

    sb->set_speed(step);
    sb->velocity_limit = 5. * step;
    sb->set_float_limits(min, max);
    et->set_float_limits(min, max);
}

void CreateSlider(GLUI_Node* p,
                  int min, int max, int step,
                  const std::string& title, int* destValPtr)
{
    GLUI_Panel* pan = new GLUI_Panel(p, 0);
    GLUI_EditText* et = new GLUI_EditText(pan, title.c_str(),
                                          reinterpret_cast<int*>(destValPtr),
                                          -1); //, &UpdateCallback);
    new GLUI_Column(pan, false);
    GLUI_Scrollbar* sb = new GLUI_Scrollbar(pan, title.c_str(),
                                            GLUI_SCROLL_HORIZONTAL,
                                            reinterpret_cast<int*>(destValPtr),
                                            -1); //, &UpdateCallback);
    sb->set_speed(step);
    sb->velocity_limit = 5 * step;
    sb->set_int_limits(min, max);
    et->set_int_limits(min, max);
}

class ColorChooser : public Control
{
public:
    virtual ~ColorChooser() { }

    ColorChooser(const std::string& name, const Color& color):
        m_name(name), m_color(color) { }

    virtual void Create(GLUI* parent)
    {
        GLUI_Panel* colorPanel = new GLUI_Panel(parent, m_name.c_str());
        const char* names[] = {"Red", "Green", "Blue"};

        for (int i = 0; i < 3; ++i)
        {
            CreateSlider(colorPanel, 0.0, 1.0, 0.05, names[i], &m_color[i]);
        }
    }

    virtual void AddValue(dict d)
    {
        d[m_name] = Vector3(m_color[0], m_color[1], m_color[2]);
    }

private:

    std::string m_name;
    Color m_color;
};

class IntSlider : public Control
{
public:
    virtual ~IntSlider() { }

    IntSlider(const std::string& name,
              int min, int max, int step, int def):
        m_name(name), m_min(min), m_max(max), m_step(step), m_value(def) { }

    virtual void Create(GLUI* parent)
    {
        CreateSlider(parent, m_min, m_max, m_step, m_name, &m_value);
    }

    virtual void AddValue(dict d)
    {
        d[m_name] = m_value;
    }

private:

    std::string m_name;
    int m_min, m_max, m_step, m_value;
};

class FloatSlider : public Control
{
public:
    FloatSlider(const std::string& name,
                float min, float max, float step, float def):
        m_name(name), m_min(min), m_max(max), m_step(step), m_value(def) { }

    virtual ~FloatSlider() { }

    virtual void Create(GLUI* parent)
    {
        CreateSlider(parent, m_min, m_max, m_step, m_name, &m_value);
    }

    virtual void AddValue(dict d)
    {
        d[m_name] = m_value;
    }

private:

    std::string m_name;
    real_t m_min, m_max, m_step, m_value;
};

namespace
{
//! Callbacks for RIB/STL output.
void OpenFileSelectWindow(int);
void OpenSTLFileSelectWindow(int);
} // unnamed namespace

//! SculptureProgram class. Glues GUI/Rendering backend to the python code.
class SculptureProgram : public RibbedScene
{
public:

    SculptureProgram()
    {
        m_defaultMat.reset(new Material);
        m_defaultMat->SetColor(render::Color(0.5, 0.5, 0.5, 0.5));
    }
    virtual ~SculptureProgram() { }

    void ParamColorChooser(const std::string& name, const python::tuple& tup)
    {
      boost::shared_ptr<ColorChooser> p(
                new ColorChooser(name, Color(extract<real_t>(tup[0]),
                                             extract<real_t>(tup[1]),
                                             extract<real_t>(tup[2]),
                                             1.0)));
        m_controlList.push_back(p);
    }

    void ParamSliderInt(const std::string& name,
                        int min, int max, int step, int def)
    {
        boost::shared_ptr<IntSlider> p(
                new IntSlider(name, min, max, step, def));
        m_controlList.push_back(p);
    }

    void ParamSliderFloat(const std::string& name,
                          float min, float max, float step, float def)
    {
        boost::shared_ptr<FloatSlider> p(
                new FloatSlider(name, min, max, step, def));
        m_controlList.push_back(p);
    }

    virtual void PotentiallyVisibleGeometry(
            GeometryCollection& geometry,
            const romulus::math::Frustum& viewFrustum) const
    { Geometry(geometry); }

    virtual void PotentiallyRelevantLights(
            LightCollection& lights,
            const romulus::math::Frustum& viewFrustum) const
    { Lights(lights); }

    virtual void Geometry(GeometryCollection& geometry) const
    {
        for (size_t i = 0; i < m_instances.size(); ++i)
            geometry.insert(m_instances[i].get());
    }

    virtual void Lights(LightCollection& lights) const
    {
        for (size_t i = 0; i < m_lights.size(); ++i)
            lights.insert(m_lights[i].get());
    }

    void Instance(const boost::shared_ptr<MutableGeometryChunk>& gc)
    {
        boost::shared_ptr<GeometryChunkInstance> inst(
                new GeometryChunkInstance);
        gc->ComputeBoundingVolume();

        inst->SetGeometryChunk(gc);
        inst->SetSurfaceDescription(m_defaultMat);
        Matrix44 xform;
        SetIdentity(xform);
        inst->SetTransform(xform);

        m_instances.push_back(inst);
    }

    void Instance(const boost::shared_ptr<MutableGeometryChunk>& gc,
                  const Vector3& c)
    {
        boost::shared_ptr<GeometryChunkInstance> inst(
                new GeometryChunkInstance);
        gc->ComputeBoundingVolume();

        inst->SetGeometryChunk(gc);
        boost::shared_ptr<Material> mat(new Material);
        mat->SetColor(render::Color(c[0], c[1], c[2], 1.0));
        inst->SetSurfaceDescription(mat);
        Matrix44 xform;
        SetIdentity(xform);
        inst->SetTransform(xform);

        m_instances.push_back(inst);
    }

    //! Instance with a rotation and a translation.
    void Instance(const boost::shared_ptr<MutableGeometryChunk>& gc,
                  const Transformation& xform,
                  const Vector3& color)
    {
        boost::shared_ptr<GeometryChunkInstance> inst(
                new GeometryChunkInstance);
        gc->ComputeBoundingVolume();

        inst->SetGeometryChunk(gc);
        boost::shared_ptr<Material> mat(new Material);
        mat->SetColor(render::Color(color[0], color[1], color[2], 1.0));
        inst->SetSurfaceDescription(mat);

        inst->SetTransform(xform.Matrix());

        m_instances.push_back(inst);
    }


    void Light(const Vector3& pos, const Vector3& col, real_t intensity)
    {
        m_lights.push_back(
                boost::shared_ptr<render::Light>(
                        new render::PointLight(
                                render::Color(col[0], col[1], col[2], 1.0),
                                intensity, false, pos, 1000.0)));
    }

    virtual void Update(boost::python::object params) = 0;

    void BeginMainLoop();

    virtual void ApplyUpdate()
    {
        GLUI_Master.sync_live_all();
        boost::shared_ptr<dict> p = GUIParameters();
        if (!m_params || (*p != *m_params))
        {
            m_params = p;
            m_instances.clear();
            m_lights.clear();
            Update(*m_params);
        }
    }

    //! File output methods.
    void OpenFileSelectWindow()
    {
        SaveSceneToRIB();
    }
    void SaveSceneToRIB()
    {
        std::ofstream out(m_outputText->get_text());
        if (!out.good())
        {
            std::cerr << "Could not open file '" << m_outputText->get_text() <<
                    "' for writing." << std::endl;
            return;
        }
        SceneFrameToRIB(*g_MainWindow->Scene(), g_MainWindow->Camera(),
                        g_MainWindow->Width(), g_MainWindow->Height(),
                        "a.tiff", out);
        out.close();
    }
    void OpenSTLFileSelectWindow()
    {
        SaveSceneToSTL();
    }
    void SaveSceneToSTL()
    {
        ASSERT(g_MainWindow);
        std::ofstream out(m_outputText->get_text());
        if (!out.good())
        {
            std::cerr << "Could not open file '" << m_outputText->get_text() <<
                    "' for writing." << std::endl;
            return;
        }
        SceneFrameToSTL(*g_MainWindow->Scene(), out);
        out.close();
    }

private:

    typedef std::map<std::string, boost::shared_ptr<Control> > ControlMap;
    typedef std::vector<boost::shared_ptr<Control> > ControlList;

    GLUI_Button* m_outputButton;
    GLUI_Button* m_stlOutputButton;
    GLUI_EditText* m_outputText;

    void CreateGUI()
    {
        m_glui = GLUI_Master.create_glui("pyribbed controls", 0);
        for (ControlList::iterator it = m_controlList.begin();
             it != m_controlList.end(); ++it)
            (*it)->Create(m_glui);

        // Add controls for RIB/STL output.
        // Output rollout.
        GLUI_Rollout* op = new GLUI_Rollout(
                m_glui, "RIB/STL Output", false);

        m_outputText = new GLUI_EditText(op, "Filename:");
        m_outputText->set_text("./solstice.rib");
        m_outputButton = new GLUI_Button(op, "Save to RIB...",
                                     0, &::OpenFileSelectWindow);
        m_stlOutputButton = new GLUI_Button(op, "Save to STL...",
                                            0, &::OpenSTLFileSelectWindow);
    }

    boost::shared_ptr<dict> GUIParameters()
    {
        boost::shared_ptr<dict> params(new dict);
        for (ControlList::iterator it = m_controlList.begin();
             it != m_controlList.end(); ++it)
            (*it)->AddValue(*params);
        return params;
    }

    boost::shared_ptr<Material> m_defaultMat;

    std::vector<boost::shared_ptr<GeometryChunkInstance> > m_instances;
    std::vector<boost::shared_ptr<render::Light> > m_lights;
    boost::shared_ptr<dict> m_params;
    GLUI* m_glui;
    std::vector<boost::shared_ptr<Control> > m_controlList;
};

namespace
{
void OpenFileSelectWindow(int)
{
    g_Sculpture->OpenFileSelectWindow();
}
void OpenSTLFileSelectWindow(int)
{
    g_Sculpture->OpenSTLFileSelectWindow();
}
} // unnamed namespace

void (SculptureProgram::*InstanceG)
(const boost::shared_ptr<MutableGeometryChunk>&) = &SculptureProgram::Instance;

void (SculptureProgram::*InstanceGC)
(const boost::shared_ptr<MutableGeometryChunk>&, const Vector3&) =
        &SculptureProgram::Instance;

void (SculptureProgram::*InstanceGTC)
(const boost::shared_ptr<MutableGeometryChunk>&,
 const Transformation&, const Vector3&) =
        &SculptureProgram::Instance;

class SculptureProgramWrap : public SculptureProgram
{
public:
    SculptureProgramWrap(PyObject* p): m_self(p) { }
    virtual ~SculptureProgramWrap() { }

    virtual void Update(boost::python::object params)
    {
        call_method<void>(m_self, "update", params);
    }

private:
    PyObject* m_self;
};

class CurveWrap : public Curve
{
public:
    CurveWrap(PyObject* p): m_self(p) { }
    virtual ~CurveWrap() { }

    virtual Vector3 Sample(real_t t) const
    {
        call_method<void>(m_self, "sample", t);
    }

private:
    PyObject* m_self;
};

boost::shared_ptr<MutableGeometryChunk> MakeSweep(
        const Curve& profile, const Curve& path)
{
    Sweep sweep;
    Polyline pl;
    try
    {
        pl = dynamic_cast<const Polyline&>(path);
    }
    catch (...)
    {
        const int numSlices = 1000;
        for (int i = 0; i < numSlices; ++i)
        {
            real_t t = real_t(i) / real_t(numSlices);
            Vector3 sample = path.Sample(t);
            pl.PushBack(sample);
        }
    }

    if (path.Begin() == path.End())
    {
        sweep.SetClosed(true);
    }

    sweep.SetPath(pl);
    sweep.SetCrosssection(dynamic_cast<const Polyline&>(profile));
    sweep.SetMinimizeTorsion(true);
    sweep.ConstructSweep();

    boost::shared_ptr<MutableGeometryChunk> mgc =
            const_pointer_cast<MutableGeometryChunk>(sweep.GeometryChunk());
    return mgc;
}

struct FrenetFrame
{
    Vector3 tangent;
    Vector3 normal;
    Vector3 binormal;
};

boost::python::list  HermiteRibbedSurfaceWithMapper(
        const Curve& c0, python::tuple i0, const Curve& c1, python::tuple i1,
        python::object tangentFunc, python::object mapFunc,
        int numRibs, const Curve& ribCross)
{
    boost::python::list objs;

    real_t i00 = extract<real_t>(i0[0]);
    real_t i01 = extract<real_t>(i0[1]);
    real_t i10 = extract<real_t>(i1[0]);
    real_t i11 = extract<real_t>(i1[1]);

    for (int i = 0; i < numRibs; ++i)
    {
        real_t s = real_t(i) / real_t(numRibs - 1);
        s = extract<real_t>(mapFunc(s));
        real_t t0 = s * (i01 - i00) + i00;
        real_t t1 = s * (i11 - i10) + i10;

        Vector3 end0 = c0.Sample(t0);
        Vector3 end1 = c1.Sample(t1);

        FrenetFrame frenet0;
        frenet0.tangent = c0.Tangent(t0);
        frenet0.normal = c0.Normal(t0);
        frenet0.binormal = c0.Binormal(t0);
        FrenetFrame frenet1;
        frenet1.tangent = c1.Tangent(t1);
        frenet1.normal = c1.Normal(t1);
        frenet1.binormal = c1.Binormal(t1);

        python::tuple cps;
        int expectedArgs = 5;

        // Handle function objects vs. functions
        //  * Free python functions have a 'func_code' attributes
        //  * For python function objects, their __call__ method has func_code
        object func_code;

        if (0 == PyObject_HasAttrString(tangentFunc.ptr(), "func_code"))
        {
            // Function object, need to increment count for 'self'
            expectedArgs += 1;
            func_code = tangentFunc.attr("__call__").attr("func_code");
        }
        else
        {
            func_code = tangentFunc.attr("func_code");

            // Handle instancemethod type callable
            if (0 != PyObject_HasAttrString(tangentFunc.ptr(), "im_self"))
                expectedArgs += 1;
        }

        int argCount = extract<int>(func_code.attr("co_argcount"));
        if (argCount != expectedArgs)
            cps = extract<python::tuple>(
                    tangentFunc(end0, frenet0, end1, frenet1));
        else
            cps = extract<python::tuple>(
                    tangentFunc(end0, frenet0, end1, frenet1, s));

        Vector3 cp1 = extract<Vector3>(cps[0]);
        Vector3 cp2 = extract<Vector3>(cps[1]);

        BezierCurve ribCurve;
        ribCurve[0] = end0;
        ribCurve[1] = cp1;
        ribCurve[2] = cp2;
        ribCurve[3] = end1;

        Polyline ribPoly(ribCurve, 50);

        objs.append(MakeSweep(ribCross, ribPoly));
    }

    return objs;
}

real_t FixedMap(real_t x) { return x; }

boost::python::list HermiteRibbedSurface(
        const Curve& c0, python::tuple i0, const Curve& c1, python::tuple i1,
        python::object tangentFunc, int numRibs, const Curve& ribCross)
{
    return HermiteRibbedSurfaceWithMapper(
            c0, i0, c1, i1, tangentFunc,
            python::object(FixedMap), numRibs, ribCross);
}

Transformation ConcatenateXX(const Transformation& t0,
                             const Transformation& t1)
{
    return Concatenate(t0, t1);
}

BOOST_PYTHON_MODULE(pyribbed)
{
    //! Vector class
    class_<Vector3>("Vector")
            .def(init<real_t, real_t, real_t>())
            .def("__getitem__", &VecGet)
            .def("__setitem__", &VecSet)
            .def(-self)
            .def(self + self)
            .def(self += self)
            .def(self - self)
            .def(self -= self)
            .def(float() * self)
            .def(self * float())
            .def(self *= float())
            .def(self / float())
            .def(self /= float())
            .def(self == self)
            .def(self != self)
            ;

    def("mag_squared", &MagnitudeSquared<3, real_t>);
    def("mag", &Magnitude<3, real_t>);
    def("normal", &Normal<3, real_t>, default_call_policies());
    def("normalize", &Normalize<3, real_t>, return_internal_reference<1>());
    def("dot", &Dot<3, real_t, real_t>, default_call_policies());
    def("cross", &Cross<real_t>, default_call_policies());


    //! Curve
    class_<Curve, boost::noncopyable, CurveWrap>("Curve")
            .def("begin", &Curve::Begin)
            .def("end", &Curve::End)
            .def("sample", pure_virtual(&Curve::Sample))
            ;

    //! BezierCurve
    class_<BezierCurve, bases<Curve> >("BezierCurve", init<unsigned int>())
            .def("__getitem__", &BezGet)
            .def("__setitem__", &BezSet)
            .def("push_back", &BezierCurve::PushBack)
            .def("resize", &BezierCurve::Resize)
            .def("size", &BezierCurve::Size)
            .def("degree", &BezierCurve::Degree)
            .def("begin", &BezierCurve::Begin)
            .def("end", &BezierCurve::End)
            .def("sample", &BezierCurve::Sample)
            ;

    //! BSplineCurve
    class_<BSplineCurve, bases<Curve> >("BSplineCurve")
            .def("push_back", &BSplineCurve::PushBack)
            .def("size", &BSplineCurve::Size)
            .def("set_degree", &BSplineCurve::SetDegree)
            .def("degree", &BSplineCurve::Degree)
            .def("is_closed", &BSplineCurve::IsClosed)
            .def("set_closed", &BSplineCurve::SetClosed)
            .def("begin", &BSplineCurve::Begin)
            .def("end", &BSplineCurve::End)
            .def("sample", &BSplineCurve::Sample)
            ;

    //! Polyline
    class_<Polyline, bases<Curve> >("Polyline")
            .def("push_back", &Polyline::PushBack)
            .def("size", &Polyline::Size)
            .def("begin", &Polyline::Begin)
            .def("end", &Polyline::End)
            .def("sample", &Polyline::Sample)
            ;

    //! Sculpture class
    class_<SculptureProgram, boost::noncopyable,
            SculptureProgramWrap>("SculptureProgram")
            .def("param_color_chooser", &SculptureProgram::ParamColorChooser)
            .def("param_slider_int", &SculptureProgram::ParamSliderInt)
            .def("param_slider_float", &SculptureProgram::ParamSliderFloat)
            .def("begin_main_loop", &SculptureProgram::BeginMainLoop)
            .def("update", pure_virtual(&SculptureProgram::Update))
            .def("set_background_color",
                 &SculptureProgram::SetBackgroundColor)
            .def("instance", InstanceG)
            .def("instance", InstanceGC)
            .def("instance", InstanceGTC)
            .def("light", &SculptureProgram::Light)
            ;

    //! Geometry generation classes and functions
    class_<MutableGeometryChunk, boost::noncopyable>("GeometryChunk")
            ;
    register_ptr_to_python<boost::shared_ptr<MutableGeometryChunk> >();

    class_<FrenetFrame>("FrenetFrame")
            .def_readwrite("tangent", &FrenetFrame::tangent)
            .def_readwrite("normal", &FrenetFrame::normal)
            .def_readwrite("binormal", &FrenetFrame::binormal)
            ;

    //! Transformations.
    class_<Rotation>("Rotation")
            .def(init<const Vector3&, real_t>())
            .def("set_axis_angle", &Rotation::SetAxisAngle)
            ;
    class_<Translation>("Translation")
            .def(init<real_t, real_t, real_t>())
            .def("set_vector", &Translation::SetVector)
            ;
    class_<Transformation>("Transformation")
            .def(init<const Transformation&>())
            .def(init<const Translation&>())
            .def(init<const Rotation&>())
            .def("set_rotation", &Transformation::SetRotation)
            .def("set_translation", &Transformation::SetTranslation)
            ;
    def("concatenate", &ConcatenateXX, default_call_policies());
    def("hermite_ribbed_surface", &HermiteRibbedSurface,
        default_call_policies());
    def("hermite_ribbed_surface", &HermiteRibbedSurfaceWithMapper,
        default_call_policies());
    def("sweep", &MakeSweep, default_call_policies());
}

void SculptureProgram::BeginMainLoop()
{
    g_Sculpture = this;

    g_GlutMaster = new GlutMaster();

    std::string winName = "pyribbed";
    g_MainWindow = new MainWindow(g_GlutMaster,
                                  800, 600,
                                  610, 50,
                                  const_cast<char*>(winName.c_str()));

    g_MainWindow->SetScene(this);

    if (m_controlList.size())
    {
        CreateGUI();
        m_glui->set_main_gfx_window(g_MainWindow->GetWindowID());
    }

    ApplyUpdate();

    g_GlutMaster->CallGlutMainLoop();
}
