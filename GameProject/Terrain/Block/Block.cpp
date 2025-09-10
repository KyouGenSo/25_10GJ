#include "./Block.h"
#include <Features/Color/Color.h>
#include <unordered_map>
#include <imgui.h>

#include "CollisionManager.h"
#include "Collision/CollisionTypeIdDef.h"
#include <Draw2D.h>

const float Block::kScale = 10.0f;

Vector4 Block::ColorToVector4(Block::Colors color)
{
    static const std::unordered_map<Block::Colors, Color> colors =
    {
        {Colors::White,     0xffffffff},    // White
        {Colors::Gray,      0x787878ff},    // Gray
        {Colors::Red,       0xd52a2fff},    // Red
        {Colors::Blue,      0x2234ddff},    // Blue
        {Colors::Yellow,    0xf1f20dff},    // Yellow
        {Colors::DarkBlue,  0x121877ff},    // DarkBlue
        {Colors::DarkRed,   0x871414ff},    // DarkRed
        {Colors::DarkYellow,0x878717ff},    // DarkYellow
        {Colors::Purple,    0x9e16e9ff},    // Purple
        {Colors::Green,     0x33e11eff},    // Green
        {Colors::Orange,    0xe1751eff},    // Orange
    };

    return colors.at(color).Vec4();
}

void Block::Initialize(std::unique_ptr<ModelInstance> modelInstance)
{
    modelInstance_ = std::move(modelInstance);
    modelInstance_->SetScale(Vector3(kScale, kScale, kScale));
    collider_ = std::make_unique<AABBCollider>();
    transform_ = modelInstance_->GetTransform();
    collider_->SetTransform(&transform_);
    collider_->SetSize({kScale, kScale, kScale});
    collider_->SetOwner(this);
    collider_->SetTypeID(static_cast<uint32_t>(CollisionTypeId::kTerrain));
}

void Block::Update()
{

}

void Block::Draw()
{
}

void Block::ImGui()
{
    #ifdef _DEBUG

    if (modelInstance_)
    {
        const char* colorNames[] = {"White", "Gray", "Blue", "Yellow", "Red", "DarkBlue", "DarkYellow", "DarkRed", "Green", "Purple", "Orange"};
        int currentColor = static_cast<int>(color_);
        if (ImGui::Combo("Color", &currentColor, colorNames, IM_ARRAYSIZE(colorNames)))
        {
            this->SetColor(static_cast<Colors>(currentColor));
        }

        ImGui::SeparatorText("Detail");
        const auto& position = modelInstance_->GetTransform().translate;
        ImGui::Text("Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
    }

    #endif // _DEBUG
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
        collider_->SetTransform(&tf);
    }
}

Vector3 Block::GetPosition() const
{
    return modelInstance_->GetTransform().translate;
}

Block::Colors Block::MixingColor(Colors _new) const {
    // 元の色が白の場合は新しい色をそのまま返す
    if (color_ == Colors::White) {
        return _new;
    }
    
    // すでに混合色の場合は新しい色を返す
    if (color_ == Colors::Purple || color_ == Colors::Orange || color_ == Colors::Green) {
        return _new;
    }
    
    // 同じ色同士の場合はそのまま
    if (color_ == _new) {
        return color_;
    }
    
    // 基本色の混合ルール
    if ((color_ == Colors::Red && _new == Colors::Blue) || 
        (color_ == Colors::Blue && _new == Colors::Red)) {
        return Colors::Purple;
    }
    if ((color_ == Colors::Red && _new == Colors::Yellow) || 
        (color_ == Colors::Yellow && _new == Colors::Red)) {
        return Colors::Orange;
    }
    if ((color_ == Colors::Blue && _new == Colors::Yellow) || 
        (color_ == Colors::Yellow && _new == Colors::Blue)) {
        return Colors::Green;
    }
    
    // グレーと原色の混合でダーク色を生成
    if ((color_ == Colors::Gray && _new == Colors::Blue) || 
        (color_ == Colors::Blue && _new == Colors::Gray)) {
        return Colors::DarkBlue;
    }
    if ((color_ == Colors::Gray && _new == Colors::Yellow) || 
        (color_ == Colors::Yellow && _new == Colors::Gray)) {
        return Colors::DarkYellow;
    }
    if ((color_ == Colors::Gray && _new == Colors::Red) || 
        (color_ == Colors::Red && _new == Colors::Gray)) {
        return Colors::DarkRed;
    }
    
    // その他の場合は新しい色を返す
    return _new;
}
