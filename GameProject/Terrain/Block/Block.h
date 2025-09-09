#pragma once

#include <Object3d.h>
#include <memory>
#include <Vector4.h>
#include <InstancedObject3d.h>

#include "AABBCollider.h"

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
    static Vector4 ColorToVector4(Block::Colors color);

public:
    // Blockの1辺のスケール
    static const float kScale;

    Block() = default;
    ~Block() = default;

    void Initialize(std::unique_ptr<ModelInstance> modelInstance);
    void Update();
    void Draw();
    void ImGui();

    void SetColor(Colors color);
    void SetPosition(const Vector3& position);

    Colors GetColor() const { return color_; }
    Vector3 GetPosition() const;
    AABBCollider* GetCollider() const { return collider_.get(); }

private:
    Transform transform_{};
    Colors color_ = Colors::White;
    std::unique_ptr<ModelInstance> modelInstance_;
    std::unique_ptr<AABBCollider> collider_;
};