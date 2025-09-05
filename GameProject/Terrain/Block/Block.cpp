#include "./Block.h"

#include <unordered_map>

const float Block::kScale = 10.0f;

const Vector4& Block::ColorToVector4(Color color)
{
    static const std::unordered_map<Color, Vector4> colors =
    {
        {Color::White, {1.0f, 1.0f, 1.0f, 1.0f}}, // White
        {Color::Gray, {0.5f, 0.5f, 0.5f, 1.0f}},  // Gray
        {Color::Blue, {0.0f, 0.0f, 1.0f, 1.0f}},  // Blue
        {Color::Green, {0.0f, 1.0f, 0.0f, 1.0f}}, // Green
        {Color::Red, {1.0f, 0.0f, 0.0f, 1.0f}},   // Red
        {Color::Yellow, {1.0f, 1.0f, 0.0f, 1.0f}},// Yellow
        {Color::Purple, {1.0f, 0.0f, 1.0f, 1.0f}},// Purple
        {Color::Orange, {1.0f, 0.5f, 0.0f, 1.0f}},// Orange
    };

    return colors.at(color);
}

void Block::Initialize()
{
    object_ = std::make_unique<Object3d>();
    object_->Initialize();
    object_->SetModel("Cube.obj");
    object_->SetScale(Vector3(kScale, kScale, kScale));
}

void Block::Update()
{
    if (object_)
    {
        object_->Update();
    }
}

void Block::Draw()
{
    if (object_)
    {
        object_->Draw();
    }
}

void Block::SetColor(Color color)
{
    color_ = color;
    if (object_)
    {
        object_->SetMaterialColor(Block::ColorToVector4(color_));
    }
}

void Block::SetPosition(const Vector3& position)
{
    if (object_)
    {
        object_->SetTranslate(position);
    }
}