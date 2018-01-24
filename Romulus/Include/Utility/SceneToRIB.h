#ifndef _SCENET0RIB_H_
#define _SCENET0RIB_H_

#include <iostream>
#include <string>

namespace romulus
{

namespace render
{
class IScene;
class Camera;
} // namespace render

//! Convert a Romulus scene, given a particular view, into a RIB file,
//! converting surfaces as well as possible.
void SceneFrameToRIB(const render::IScene& scene, const render::Camera& cam,
                     int imageWidth, int imageHeight,
                     const std::string& imageFile, std::ostream& out);

} // namespace romulus

#endif // _SCENET0RIB_H_
