
/* ************************************************************************ */

#ifndef ENABLE_RENDER
#error ENABLE_RENDER must be 1 to compile render context.
#endif

/* ************************************************************************ */

// Declaration
#include "render/Context.hpp"

// C++
#include <cmath>

// OpenGL
#include <GL/gl.h>
#include <GL/glu.h>

/* ************************************************************************ */

namespace render {

/* ************************************************************************ */

std::vector<std::uint8_t> Context::getData() const noexcept
{
    std::vector<std::uint8_t> data;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glReadBuffer(GL_FRONT);
    glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3],
                 GL_RGBA, GL_UNSIGNED_BYTE, data.data());

    return data;
}

/* ************************************************************************ */

void Context::init() noexcept
{
    GLfloat sun_direction[] = { 0.0, -1.0, 0.0, 1.0 };
    GLfloat sun_intensity[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat ambient_intensity[] = { 0.3, 0.3, 0.3, 1.0 };

    // Clear color.
    glClearColor(1.f, 1.f, 1.f, 1.f);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    //glEnable(GL_LIGHTING);
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_intensity);

    //glEnable(GL_LIGHT0);
    //glLightfv(GL_LIGHT0, GL_POSITION, sun_direction);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_intensity);

    //glEnable(GL_COLOR_MATERIAL);
    //glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

/* ************************************************************************ */

void Context::setView(int width, int height) noexcept
{
     // Setup viewport (whole window)
    glViewport(0, 0, width, height);

    // Setup projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const GLdouble wh = width / 2.0;
    const GLdouble hh = height / 2.0;

    glOrtho(-wh, wh, -hh, hh, -1000, 1000);
    //glRotatef(90, 1, 0, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Apply zoom matrix
    float scale = 1 / m_camera.getZoom();
    glScalef(scale, scale, scale);

    // Rotate camera
    //glRotatef(m_camera.getRotation(), 0, 1, 0);
}

/* ************************************************************************ */

void Context::frameBegin(int width, int height) noexcept
{
    // Clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setView(width, height);
}

/* ************************************************************************ */

void Context::frameEnd() noexcept
{
    glFlush();
}

/* ************************************************************************ */

void Context::drawLine(const Position& pos, const Vector<float>& dir, const Color& color) noexcept
{
    // Setup transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(pos.x, pos.y, 0);

    // Draw color
    glColor3f(color.red, color.green, color.blue);

    glBegin(GL_LINES);

    glVertex2f(0, 0);
    glVertex2f(dir.x, dir.y);

    glEnd();

    glPopMatrix();
}

/* ************************************************************************ */

void Context::drawCircle(const Position& pos, float radius, const Color& color) noexcept
{
    // Draw color
    glColor4f(color.red, color.green, color.blue, color.alpha);

    // Setup transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(pos.x, pos.y, 0);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);

    constexpr unsigned SEGMENTS = 50;
    constexpr float STEP = 2 * 3.14159265359 / SEGMENTS;

    for (unsigned n = 0; n <= SEGMENTS; ++n)
    {
        float alpha = STEP * n;
        glVertex2f(sin(alpha) * radius, cos(alpha) * radius);
    }

    glEnd();

    glPopMatrix();
}

/* ************************************************************************ */
/*
void Context::drawSphere(const Position& pos, float radius, const Color& color) noexcept
{
    constexpr float DEG2RAD = 3.14159f / 180.f;

    // Setup transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);

    // Sphere color
    glColor4f(color.red, color.green, color.blue, color.alpha);

    constexpr int LATS = 20;
    constexpr int LONGS = 20;

    for (int i = 0; i <= LATS; i++)
    {
        double lat0 = M_PI * (-0.5 + (double) (i - 1) / LATS);
        double z0   = sin(lat0);
        double zr0  = cos(lat0);

        double lat1 = M_PI * (-0.5 + (double) i / LATS);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);

        for (int j = 0; j <= LONGS; j++)
        {
            double lng = 2 * M_PI * (double) (j - 1) / LONGS;
            double x = cos(lng);
            double y = sin(lng);

            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(radius * x * zr0, radius * y * zr0, radius * z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(radius * x * zr1, radius * y * zr1, radius * z1);
        }

        glEnd();
    }

    glPopMatrix();
}
*/
/* ************************************************************************ */

void Context::drawGrid(const Vector<float>& size, const Vector<unsigned>& count,
                       const Color& color) noexcept
{
    const Vector<float> start{
        -size.x / 2.f,
        -size.y / 2.f
    };

    const Vector<float> step{
        size.x / count.x,
        size.y / count.y
    };

    // Setup transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Sphere color
    glColor4f(color.red, color.green, color.blue, color.alpha);

    glBegin(GL_LINES);

    for (unsigned i = 0; i <= count.x; ++i)
    {
        glVertex2f(start.x + i * step.x, start.y);
        glVertex2f(start.x + i * step.x, start.y + size.y);
    }

    for (unsigned i = 0; i <= count.y; ++i)
    {
        glVertex2f(start.x, start.y + i * step.y);
        glVertex2f(start.x + size.x, start.y + i * step.y);
    }

    glEnd();

    glPopMatrix();
}

/* ************************************************************************ */

}

/* ************************************************************************ */
