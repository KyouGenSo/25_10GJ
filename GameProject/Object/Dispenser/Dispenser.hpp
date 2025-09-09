#ifndef Dispenser_HPP_
#define Dispenser_HPP_
#include <memory>
#include <vector>

#include "../ColorBall/ColorBall.hpp"
#include "Vector4.h"

class Player;

class Dispenser
{
public:
    enum class Colors{
        GRAY,
        BLUE,
        YELLOW,
        RED
    };

private:
    std::unique_ptr<InstancedObject3d> model_;
    std::vector<std::unique_ptr<ColorBall>> balls_;

    Colors color_ = Colors::GRAY;
    Player* owner_ = nullptr;

public:
    static Vector4 ToVector4(Colors _color);

    Dispenser& Initialize();
    void Update();
    void Draw() const;

    void SelectColor(Colors _color);

    void Dispense();

    Dispenser& SetOwner(Player* _owner);

private:

}; // class Dispenser

#endif // Dispenser_HPP_
