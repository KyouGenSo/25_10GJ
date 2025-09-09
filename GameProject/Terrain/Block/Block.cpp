#include "./Block.h"
#include <Features/Color/Color.h>
#include <unordered_map>
#include <imgui.h>

#include "CollisionManager.h"
#include "Collision/CollisionTypeIdDef.h"

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
    collider_ = std::make_unique<AABBCollider>();
    auto tf = modelInstance_->GetTransform();
    collider_->SetTransform(&tf);
    collider_->SetSize({kScale, kScale, kScale});
    collider_->SetOwner(this);
    collider_->SetTypeID(static_cast<uint32_t>(CollisionTypeId::kTerrain));

    //CollisionManager::GetInstance()->AddCollider(collider_.get());
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
        const char* colorNames[] = { "White", "Gray", "Blue", "Green", "Red", "Yellow", "Purple", "Orange" };
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