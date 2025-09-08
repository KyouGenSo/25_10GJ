#ifndef ColorBall_HPP_
#define ColorBall_HPP_
#include <memory>

#include "InstancedObject3d.h"
#include "SphereCollider.h"

class ColorBall
{
    std::unique_ptr<ModelInstance> model_;
    std::unique_ptr<SphereCollider> collider_;
    Vector3 velocity_{};

    Transform transform_{};

public:
    void Initialize(std::unique_ptr<ModelInstance> _model);
    void Update();

private:
    void SetCollider();
}; // class ColorBall

#endif // ColorBall_HPP_
