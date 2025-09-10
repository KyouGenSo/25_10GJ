#ifndef ColorBall_HPP_
#define ColorBall_HPP_
#include <memory>

#include "InstancedObject3d.h"
#include "Collision/Collider/ColorBall/ColorBallCollider.hpp"
#include "Terrain/Block/Block.h"

class Dispenser;

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
    Dispenser* dispenser_ = nullptr;

    std::unique_ptr<ModelInstance> model_;
    std::unique_ptr<ColorBallCollider> collider_;
    Vector3 velocity_{};

    Transform transform_{};

    float lifetime_ = 5.f;
    bool dead_ = false;
    bool diffused_ = false;

    Colors color_ = Colors::GRAY;
	uint32_t hitVH_ = 0;

public:
    ~ColorBall();
    void Initialize(std::unique_ptr<ModelInstance> _model, Colors _colorForPaint, Dispenser* _dispenser, Vector3 _direction, bool _diffused);
    void Update();
    void PlaySe();

    bool IsDead() const;
    Block::Colors GetColorForPaint() const;
    void imDead();
    bool IsDiffused() const;
    
private:
    void SetCollider();
}; // class ColorBall

#endif // ColorBall_HPP_
