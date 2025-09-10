#include "ColorBall.hpp"

#include "CollisionManager.h"
#include "Mat4x4Func.h"
#include "Collision/CollisionTypeIdDef.h"

ColorBall::~ColorBall()
{
    if (collider_){
        CollisionManager::GetInstance()->RemoveCollider(collider_.get());
    }
}

void ColorBall::Initialize(std::unique_ptr<ModelInstance> _model, Colors _colorForPaint, Dispenser* _dispenser, Vector3 _direction, bool _diffused)
{
    model_ = std::move(_model);
    color_ = _colorForPaint;
    dispenser_ = _dispenser;
    diffused_ = _diffused;

    transform_ = model_->GetTransform();
    
    velocity_ = _direction.Normalize() * 0.3f;
    velocity_.y = 1.3f;
    SetCollider();
}

void ColorBall::Update()
{
    lifetime_ -= 1.f / 60.f;

    if (lifetime_ <= 0.f){
        dead_ = true;
    }

    velocity_.y += -0.1f;

    transform_.translate += velocity_;

    model_->SetTransform(transform_);
}

bool ColorBall::IsDead() const
{
    return dead_;
}

Block::Colors ColorBall::GetColorForPaint() const
{
    Block::Colors color = Block::Colors::White;
    switch (color_){
    case Colors::GRAY:
        color = Block::Colors::Gray;
        break;
    case Colors::BLUE:
        color = Block::Colors::Blue;
        break;
    case Colors::YELLOW:
        color = Block::Colors::Yellow;
        break;
    case Colors::RED:
        color = Block::Colors::Red;
        break;
    }

    return color;
}

void ColorBall::imDead()
{
    dead_ = true;
}

bool ColorBall::IsDiffused() const {
    return diffused_;
}

void ColorBall::SetCollider()
{
    collider_ = std::make_unique<ColorBallCollider>(this, dispenser_);
    collider_->SetTransform(&transform_);
    collider_->SetRadius(1.0f);
    collider_->SetOffset({0,0,0});
    collider_->SetTypeID(static_cast<uint32_t>(CollisionTypeId::kColorBall));

    CollisionManager::GetInstance()->AddCollider(collider_.get());
    CollisionManager::GetInstance()->SetCollisionMask(static_cast<uint32_t>(CollisionTypeId::kColorBall), static_cast<uint32_t>(CollisionTypeId::kTerrain), true);
}
