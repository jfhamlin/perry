#ifndef _RENDEROPENGLGLINTERFACE_H_
#define _RENDEROPENGLGLINTERFACE_H_

namespace romulus
{
namespace render
{
namespace opengl
{

class RenderDevice;
class ShaderProgramManager;
class TextureManager;
class IGeometryCache;

struct GLInterface
{
    GLInterface(RenderDevice* rd, ShaderProgramManager* spm, TextureManager* tm,
                IGeometryCache* gc):
        Device(rd), ShaderProgramMgr(spm), TextureMgr(tm),
        GeometryCache(gc)
    {
    }

    RenderDevice* const Device;
    ShaderProgramManager* const ShaderProgramMgr;
    TextureManager* const TextureMgr;
    IGeometryCache* const GeometryCache;
};

}
}
}

#endif // _RENDEROPENGLGLINTERFACE_H_
