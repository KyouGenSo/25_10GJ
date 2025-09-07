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

void Block::Initialize(std::unique_ptr<ModelInstance> modelInstance)
{
    modelInstance_ = std::move(modelInstance);
    modelInstance_->SetScale(Vector3(kScale, kScale, kScale));
}

void Block::Update()
{

}

void Block::Draw()
{

}

void Block::SetColor(Color color)
{
    color_ = color;
    if (modelInstance_)
    {
        modelInstance_->SetColor(Block::ColorToVector4(color_));
    }
}

void Block::SetPosition(const Vector3& position)
{
    if (modelInstance_)
    {
        auto tf = modelInstance_->GetTransform();
        tf.translate = position;
        modelInstance_->SetTransform(tf);
    }
}