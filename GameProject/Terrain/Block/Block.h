#pragma once

#include <Object3d.h>
#include <memory>
#include <Vector4.h>
#include <InstancedObject3d.h>

class Block
{
public:
    enum class Color
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
    static const Vector4& ColorToVector4(Color color);

public:
    // Blockの1辺のスケール
    static const float kScale;

    Block() = default;
    ~Block() = default;

    void Initialize(std::unique_ptr<ModelInstance> modelInstance);
    void Update();
    void Draw();

    Color GetColor() const { return color_; }
    void SetColor(Color color);
    void SetPosition(const Vector3& position);

private:
    Color color_ = Color::White;
    std::unique_ptr<ModelInstance> modelInstance_;
};