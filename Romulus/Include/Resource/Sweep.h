#ifndef _SWEEP_H_
#define _SWEEP_H_

#include "Math/Matrix.h"
#include "Math/Polyline.h"
#include "Resource/MutableGeometryChunk.h"
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>

namespace romulus
{
class Sweep
{
public:

    Sweep();

    boost::shared_ptr<const romulus::MutableGeometryChunk>
    GeometryChunk() const { return m_mgc; }

    inline void SetPath(const romulus::math::Polyline& path)
    { m_path = path; InitializeSliceAzimuths(); }
    inline void SetCrosssection(const romulus::math::Polyline& cross)
    { m_cross = cross; }

    void SetClosed(bool isClosed) { m_isClosed = isClosed; }

    real_t GlobalTwist() const { return m_globalTwist; }
    void SetGlobalTwist(real_t twist) { m_globalTwist = twist; }

    real_t GlobalAzimuth() const { return m_globalAzimuth; }
    void SetGlobalAzimuth(real_t az) { m_globalAzimuth = az; }

    real_t SliceAzimuth(int i) { return m_sliceAzimuths[i]; }
    void SetSliceAzimuth(int i, real_t az) { m_sliceAzimuths[i] = az; }

    void SetMinimizeTorsion(bool minimizeTorsion)
    { m_minimizeTorsion = minimizeTorsion; }

    void ConstructSweep();

private:

    void InitializeSliceAzimuths()
    {
        m_sliceAzimuths.resize(m_path.Size());
        for (uint_t i = 0; i < m_sliceAzimuths.size(); ++i)
            m_sliceAzimuths[i] = 0.;
    }

    void DestroySweep();

    romulus::math::Matrix33 CalculateFrenetFrame(
            romulus::math::Vector3 p0, romulus::math::Vector3 p1,
            romulus::math::Vector3 p2, int prevIndex);
    void CalculateFrenetFrames();
    void MinimizeTorsion();
    romulus::math::Polyline TransformCrosssection(uint_t index);

    romulus::math::Polyline m_path;
    romulus::math::Polyline m_cross;
    bool m_isClosed;
    bool m_crossIsClosed;
    bool m_minimizeTorsion;
    std::vector<romulus::math::Matrix33> m_frenetFrames;
    real_t m_globalTwist;
    real_t m_globalAzimuth;
    std::vector<real_t> m_sliceAzimuths;

    boost::shared_ptr<romulus::MutableGeometryChunk> m_mgc;
    boost::scoped_array<boost::scoped_array<ushort_t> > m_meshVertices;
};

} // namespace romulus

#endif // _SWEEP_H_
