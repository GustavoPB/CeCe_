/* ************************************************************************ */
/* Georgiev Lab (c) 2015                                                    */
/* ************************************************************************ */
/* Department of Cybernetics                                                */
/* Faculty of Applied Sciences                                              */
/* University of West Bohemia in Pilsen                                     */
/* ************************************************************************ */
/*                                                                          */
/* This file is part of CeCe.                                               */
/*                                                                          */
/* CeCe is free software: you can redistribute it and/or modify             */
/* it under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* CeCe is distributed in the hope that it will be useful,                  */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with CeCe.  If not, see <http://www.gnu.org/licenses/>.            */
/*                                                                          */
/* ************************************************************************ */

// Declaration
#include "cece/plugins/obstacle/Polygon.hpp"

#ifdef CECE_ENABLE_BOX2D_PHYSICS
#  include <Box2D/Box2D.h>
#endif

// CeCe
#include "cece/core/VectorUnits.hpp"
#include "cece/core/StringStream.hpp"
#include "cece/config/Configuration.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace obstacle {

/* ************************************************************************ */

void Polygon::configure(const config::Configuration& config, simulator::Simulation& simulation)
{
    object::Object::configure(config, simulation);

    DynamicArray<PositionVector> edges;
    InStringStream iss(config.get("vertices"));
    PositionVector vector;

    while (iss >> vector)
        edges.push_back(vector);

    // Init shape
    getMutableShapes().push_back(simulator::Shape::makeEdges(std::move(edges)));

#ifdef CECE_ENABLE_BOX2D_PHYSICS
    DynamicArray<b2Vec2> vertices;
    const auto& shape = getShapes().back();

    for (const auto& v : shape.getEdges().edges)
        vertices.push_back(getConverter().convertPosition(v));

    // Create edges loop
    if (vertices.size() < 3)
        m_bodyShape.CreateChain(vertices.data(), vertices.size());
    else
        m_bodyShape.CreateLoop(vertices.data(), vertices.size());

    getBody()->CreateFixture(&m_bodyShape, 1);
#endif
}

/* ************************************************************************ */

#ifdef CECE_ENABLE_RENDER
void Polygon::draw(render::Context& context)
{
    if (!m_drawPolygon)
    {
        const auto& shape = getShapes().back();
        const auto& edges = shape.getEdges().edges;
        DynamicArray<render::Lines::LineType> lines;
        lines.reserve(edges.size());

        // Create lines from shape
        for (std::size_t i = 1u; i < edges.size(); ++i)
        {
            lines.push_back(makePair(
                render::Lines::PointType(edges[i - 1].getX().value(), edges[i - 1].getY().value()),
                render::Lines::PointType(edges[i].getX().value(), edges[i].getY().value())
            ));
        }

        if (edges.size() > 2)
        {
            lines.push_back(makePair(
                render::Lines::PointType(edges.back().getX().value(), edges.back().getY().value()),
                render::Lines::PointType(edges.front().getX().value(), edges.front().getY().value())
            ));
        }

        m_drawPolygon.create(context, std::move(lines));
    }

    context.matrixPush();
    context.matrixTranslate(getPosition());
    m_drawPolygon->draw(context);
    context.matrixPop();
}
#endif

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
