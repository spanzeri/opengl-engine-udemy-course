#pragma once

#include "base.h"

class Application
{
public:
    virtual ~Application() = default;

    virtual bool Init() = 0;
    virtual void Shutdown() = 0;
    virtual void Update(f32 delta_time_sec) = 0;

    void SetNeedsToBeClosed()       { m_should_close = true; }
    bool NeedsToBeClosed() const    { return m_should_close; }

protected:
    bool m_should_close;
};

