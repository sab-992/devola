#include <memory>
#include <Rang.h>


class DisplayColor_i {
public:
	DisplayColor_i() = default;
	virtual ~DisplayColor_i() = default;

    virtual rang::fg Color() = 0;
};

class LogType : public DisplayColor_i {
public:
    LogType(rang::fg Color);
    ~LogType() = default;

    static std::unique_ptr<DisplayColor_i> Debug();
    static std::unique_ptr<DisplayColor_i> Error();
    static std::unique_ptr<DisplayColor_i> Info();
    static std::unique_ptr<DisplayColor_i> Special();
    static std::unique_ptr<DisplayColor_i> Warning();

    rang::fg Color() override;
private:
    static rang::fg m_Color;
};