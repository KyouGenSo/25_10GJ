#ifndef ColorBall_HPP_
#define ColorBall_HPP_
#include <memory>

#include "InstancedObject3d.h"
#include "Collision/Collider/ColorBall/ColorBallCollider.hpp"
#include "Terrain/Block/Block.h"

class ColorBall
{
public:
    enum class Colors{
        GRAY,
        BLUE,
        YELLOW,
        RED
    };

private:
    std::unique_ptr<ModelInstance> model_;
    std::unique_ptr<ColorBallCollider> collider_;
    Vector3 velocity_{};

    Transform transform_{};

    float lifetime_ = 5.f;
    bool dead_ = false;

    Colors color_ = Colors::GRAY;

public:
    ~ColorBall();
    void Initialize(std::unique_ptr<ModelInstance> _model, Colors _colorForPaint);
    void Update();

    bool IsDead() const;
    Block::Colors GetColorForPaint() const;
    void imDead();

private:
    void SetCollider();
}; // class ColorBall

#endif // ColorBall_HPP_
