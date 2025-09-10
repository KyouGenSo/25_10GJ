#include "Dispenser.hpp"

#include <algorithm>
#include <unordered_map>

#include "Features/Color/Color.h"
#include "Object/Player/Player.h"

Vector4 Dispenser::ToVector4(ColorBall::Colors _color)
{
    // Blockよりも色を限定し、濃く見やすくする
    static const std::unordered_map<ColorBall::Colors, Color> colors =
    {
        { ColorBall::Colors::GRAY,      0x787878ff },   // Gray
        { ColorBall::Colors::BLUE,      0x2234ddff },    // Blue
        { ColorBall::Colors::YELLOW,    0xf1f20dff },    // Yellow
        { ColorBall::Colors::RED,       0xd52a2fff },    // Red
    };

    return colors.at(_color).Vec4();
}

Dispenser& Dispenser::Initialize()
{
    model_ = std::make_unique<InstancedObject3d>();
    model_->Initialize("sphere.gltf");

    return *this;
}

void Dispenser::Update() 
{
    std::erase_if(balls_,[](const std::unique_ptr<ColorBall>& _ball){ return _ball->IsDead(); });
    for (const auto& ball : balls_)
    {
        ball->Update();
    }

    model_->Update();
}

void Dispenser::Draw() const
{
    model_->Draw();
}

void Dispenser::SelectColor(const ColorBall::Colors _color)
{
    color_ = _color;
}

void Dispenser::Dispense()
{
    if (!owner_)return;

    auto ball = std::make_unique<ColorBall>();
    ball->Initialize(model_->CreateInstance(owner_->GetTransform(), ToVector4(color_)), color_);
    balls_.push_back(std::move(ball));
}

Dispenser& Dispenser::SetOwner(Player* _owner)
{
    owner_ = _owner;
    return *this;
}
