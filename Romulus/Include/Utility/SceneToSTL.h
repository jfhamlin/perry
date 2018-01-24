#ifndef _SCENET0STL_H_
#define _SCENET0STL_H_

#include <iostream>
#include <string>

namespace romulus
{

namespace render
{
class IScene;
} // namespace render

//! Convert a Romulus scene, given a particular view, into a STL file.
void SceneFrameToSTL(const render::IScene& scene, std::ostream& out);

} // namespace romulus

#endif // _SCENET0STL_H_
