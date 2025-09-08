#pragma once

#include <Object3d.h>
#include <memory>
#include <Vector4.h>
#include <InstancedObject3d.h>

class Block
{
public:
    enum class Colors
    {
        White,
        Gray,
        Blue,
        Green,
        Red,
        Yellow,
        Purple,
        Orange,
    };

    /// ColorからVector4への変換
    static const Vector4& ColorToVector4(Block::Colors color);

public:
    // Blockの1辺のスケール
    static const float kScale;

    Block() = default;
    ~Block() = default;

    void Initialize(std::unique_ptr<ModelInstance> modelInstance);
    void Update();
    void Draw();
    void ImGui();

    Colors GetColor() const { return color_; }
    void SetColor(Colors color);
    void SetPosition(const Vector3& position);

private:
    Colors color_ = Colors::White;
    std::unique_ptr<ModelInstance> modelInstance_;
};