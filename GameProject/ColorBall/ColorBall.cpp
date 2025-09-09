#include "ColorBall.hpp"

#include "CollisionManager.h"
#include "Mat4x4Func.h"
#include "Collision/CollisionTypeIdDef.h"

void ColorBall::Initialize(std::unique_ptr<ModelInstance> _model)
{
    model_ = std::move(_model);
    
    if (model_) {
        transform_ = model_->GetTransform();
        
        // 回転行列を作成
        Matrix4x4 rotationMatrix = Mat4x4::MakeRotateXYZ(transform_.rotate);
        
        // デフォルトの前方向ベクトル（Z軸正方向）を回転させてvelocity_を計算
        Vector3 forward = {0.0f, 0.0f, 1.0f};
        velocity_ = (Mat4x4::TransformNormal(rotationMatrix, forward)/3.f);
        velocity_.y = 1.3f;
        SetCollider();
    } else {
        velocity_ = {0.0f, 0.0f, 1.0f};
    }
}

void ColorBall::Update() {
    //velocity_.x *= 0.86f;
    //velocity_.z *= 0.86f;

    velocity_.y += -0.1f;

    transform_.translate += velocity_;

    model_->SetTransform(transform_);
}

void ColorBall::SetCollider() {
    collider_ = std::make_unique<SphereCollider>();
    Transform transform = model_->GetTransform();
    collider_->SetTransform(&transform);
    collider_->SetRadius(1.0f);
    collider_->SetOffset({0,0,0});
    collider_->SetTypeID(static_cast<uint32_t>(CollisionTypeId::kColorBall));
    collider_->SetOwner(this);

    CollisionManager::GetInstance()->AddCollider(collider_.get());
    CollisionManager::GetInstance()->SetCollisionMask(static_cast<uint32_t>(CollisionTypeId::kColorBall), static_cast<uint32_t>(CollisionTypeId::kActiveTerrain), true);
}