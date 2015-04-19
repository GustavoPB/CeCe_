
/* ************************************************************************ */

// Declaration
#include "Cell.hpp"

/* ************************************************************************ */

namespace module {
namespace cell {

/* ************************************************************************ */

void Cell::update(units::Duration dt)
{
    // Just update position
    DynamicObject::update(dt);
}

/* ************************************************************************ */

#ifdef ENABLE_RENDER
void Cell::drawInit(render::Context& context)
{
    m_renderObject.init();
}
#endif

/* ************************************************************************ */

#ifdef ENABLE_RENDER
void Cell::draw(render::Context& context)
{
    auto gfp = getGfp();
    auto rfp = getRfp();
    auto yfp = getYfp();

    // TODO: Better calculation
    float red = 0.5f + rfp / 1000.f + yfp / 1000.f;
    float green = 0.5f + gfp / 1000.f + yfp / 1000.f;
    float blue = 0.5f + 0;

    const auto pos = getPosition();
    const auto radius = calcSphereRadius(getVolume());

    m_renderObject.draw(pos, radius, {red, green, blue, 0.8f});
}
#endif

/* ************************************************************************ */

}
}

/* ************************************************************************ */