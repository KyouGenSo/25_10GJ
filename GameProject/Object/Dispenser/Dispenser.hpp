#ifndef Dispenser_HPP_
#define Dispenser_HPP_
#include <memory>
#include <vector>

#include "../ColorBall/ColorBall.hpp"
#include "Vector4.h"

class Player;

class Dispenser
{
    std::unique_ptr<InstancedObject3d> model_;
    std::vector<std::unique_ptr<ColorBall>> balls_;

    ColorBall::Colors color_ = ColorBall::Colors::GRAY;
    Player* owner_ = nullptr;

public:
    static Vector4 ToVector4(ColorBall::Colors _color);

    Dispenser& Initialize();
    void Update();
    void Draw() const;

    void SelectColor(ColorBall::Colors _color);

    void Dispense();

    Dispenser& SetOwner(Player* _owner);

private:

}; // class Dispenser

#endif // Dispenser_HPP_
