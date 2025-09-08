#include "Dispenser.hpp"

#include <unordered_map>

#include "Features/Color/Color.h"
#include "Object/Player/Player.h"

const Vector4& Dispenser::ToVector4(Dispenser::Colors _color)
{
    // Blockよりも色を限定し、濃く見やすくする
    static const std::unordered_map<Dispenser::Colors, Color> colors =
    {
        { Colors::GRAY,      0x787878ff },   // Gray
        { Colors::BLUE,      0x2234ddff },    // Blue
        { Colors::YELLOW,    0xf1f20dff },    // Yellow
        { Colors::RED,       0xd52a2fff },    // Red
    };

    return colors.at(_color).Vec4();
}

Dispenser& Dispenser::Initialize()
{
    model_ = std::make_unique<InstancedObject3d>();
    model_->Initialize("sphere.gltf");

    return *this;
}

void Dispenser::Update() const
{
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

void Dispenser::SelectColor(const Colors _color)
{
    color_ = _color;
}

void Dispenser::Dispense()
{
    if (!owner_)return;

    auto ball = std::make_unique<ColorBall>();
    ball->Initialize(model_->CreateInstance(owner_->GetTransform(), ToVector4(color_)));
    balls_.push_back(std::move(ball));
}

Dispenser& Dispenser::SetOwner(Player* _owner)
{
    owner_ = _owner;
    return *this;
}
