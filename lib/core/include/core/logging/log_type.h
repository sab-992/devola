#pragma once

#include <core/logging/detail/rang.h>
#include <memory>


// Rework class
class DisplayColor_i {
public:
	DisplayColor_i() = default;
	virtual ~DisplayColor_i() = default;

    virtual rang::fg color() = 0;
};

// Rework class or maybe remove class
class LogType : public DisplayColor_i {
public:
    LogType(rang::fg color);
    ~LogType() = default;

    static std::unique_ptr<DisplayColor_i> debug();
    static std::unique_ptr<DisplayColor_i> error();
    static std::unique_ptr<DisplayColor_i> info();
    static std::unique_ptr<DisplayColor_i> special();
    static std::unique_ptr<DisplayColor_i> warning();

    rang::fg color() override;
private:
    static rang::fg m_color;
};