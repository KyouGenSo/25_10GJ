#include "./Block.h"
#include <Features/Color/Color.h>
#include <unordered_map>

const float Block::kScale = 10.0f;

const Vector4& Block::ColorToVector4(Block::Colors color)
{
    static const std::unordered_map<Block::Colors, Color> colors =
    {
        {Colors::White,     0xffffffff},    // White
        {Colors::Gray,      0x787878ff},    // Gray
        {Colors::Blue,      0x2234ddff},    // Blue
        {Colors::Green,     0x33e11eff},    // Green
        {Colors::Red,       0xd52a2fff},    // Red
        {Colors::Yellow,    0xf1f20dff},    // Yellow
        {Colors::Purple,    0x9e16e9ff},    // Purple
        {Colors::Orange,    0xe1751eff},    // Orange
    };

    return colors.at(color).Vec4();
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

void Block::SetColor(Colors color)
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