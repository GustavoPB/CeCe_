
#pragma once

/* ************************************************************************ */

// wxWidgets
#include <wx/panel.h>
#include <wx/sizer.h>

// Simulator
#include "core/Vector.hpp"

/* ************************************************************************ */

/**
 * @brief Base control for Vector values.
 */
template<typename Ctrl, typename Value>
class VectorCtrlBase : public wxPanel
{

// Public Ctors & Dtors
public:


    /**
     * @brief Constructor.
     *
     * @param parent
     * @param id
     */
    VectorCtrlBase(wxWindow* parent, wxWindowID id)
        : wxPanel(parent, id)
    {
        SetSizer(new wxBoxSizer(wxHORIZONTAL));

        GetSizer()->Add(m_valueX = new Ctrl(this), wxSizerFlags(1));
        GetSizer()->Add(m_valueY = new Ctrl(this), wxSizerFlags(1));
    }


// Public Accessors
public:


    /**
     * @brief Returns current value.
     *
     * @return
     */
    Vector<Value> GetValue() const noexcept
    {
        return Vector<Value>(m_valueX->GetValue(), m_valueY->GetValue());
    }


// Public Mutators
public:


    /**
     * @brief Change current value.
     *
     * @param value
     */
    void SetValue(const Vector<Value>& value) noexcept
    {
        m_valueX->SetValue(value.x);
        m_valueY->SetValue(value.y);
    }


// Private Data Members
private:

    /// X value.
    Ctrl* m_valueX;

    /// Y value.
    Ctrl* m_valueY;

};

/* ************************************************************************ */