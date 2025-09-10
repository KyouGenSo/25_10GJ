#ifndef Dispenser_HPP_
#define Dispenser_HPP_
#include <memory>
#include <vector>

#include "Sprite.h"
#include "../ColorBall/ColorBall.hpp"
#include "Vector4.h"

class CellBasedFiltering;
class Player;

class Dispenser
{
    CellBasedFiltering* cellFilter_ = nullptr;
    std::unique_ptr<InstancedObject3d> model_;
    std::vector<std::unique_ptr<ColorBall>> balls_;

    ColorBall::Colors color_ = ColorBall::Colors::GRAY;
    Player* owner_ = nullptr;

    std::unique_ptr<Sprite> sprite_;
    Vector2 hudPos_ = { 25.f, 620.f };

public:
    static Vector4 ToVector4(ColorBall::Colors _color);

    Dispenser& Initialize();
    void Update();
    void Draw() const;

    void SelectColor(ColorBall::Colors _color);

    void Dispense(Vector3 _direction, bool _diffused = false);
    void Dispense(Vector3 _position, Vector3 _direction, bool _diffused = false);

    Dispenser& SetOwner(Player* _owner);

    void DrawImGui();
    void DrawHUD() const;

private:

}; // class Dispenser

#endif // Dispenser_HPP_
