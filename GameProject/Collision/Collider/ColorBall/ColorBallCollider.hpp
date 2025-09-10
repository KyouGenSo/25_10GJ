#ifndef ColorBallCollider_HPP_
#define ColorBallCollider_HPP_
#include "SphereCollider.h"


class ColorBall;

class ColorBallCollider final : public SphereCollider
{
    ColorBall* colorBall_ = nullptr;
public:
    ColorBallCollider(ColorBall* _owner);
    void OnCollisionEnter(Collider* other) override;
}; // class ColorBallCollider

#endif // ColorBallCollider_HPP_
