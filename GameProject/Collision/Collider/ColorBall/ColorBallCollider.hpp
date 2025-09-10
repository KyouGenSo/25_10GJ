#ifndef ColorBallCollider_HPP_
#define ColorBallCollider_HPP_
#include "SphereCollider.h"

class ColorBall;
class Dispenser;

class ColorBallCollider final : public SphereCollider
{
    ColorBall* colorBall_ = nullptr;
    Dispenser* dispenser_ = nullptr;

public:
    ColorBallCollider(ColorBall* _owner, Dispenser* _dispenser);
    void OnCollisionEnter(Collider* other) override;
}; // class ColorBallCollider

#endif // ColorBallCollider_HPP_
