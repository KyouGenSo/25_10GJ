#include "./Bar3d.h"
#include <WinApp.h>
#include <Mat4x4Func.h>
//#include "CellBasedFiltering.h"

void Bar3d::Initialize(const Color& colorContext, const Color& colorBG)
{
    // Initialize the background sprite
    InitializeSprite(sprite_max_, colorBG);
    InitializeSprite(sprite_current_, colorContext);
}

void Bar3d::Update()
{
    UpdateDisplayFlagByTimer();

    if (isDisplay_ == false) return;

    CurrentValueSpriteUpdate();

    sprite_max_->Update();
    sprite_current_->Update();
}

void Bar3d::Draw2d()
{
    if (isDisplay_ == false) return;


    sprite_max_->Draw();
    sprite_current_->Draw();
}

void Bar3d::Display(float _sec)
{
    timerDisplay_.Reset();
    timerDisplay_.Start();
    isDisplay_ = true;
    displayTime_ = _sec;
}

void Bar3d::SetPosition(const Vector2& _position)
{
    sprite_current_->SetPos(_position);
    sprite_max_->SetPos(_position);
}

void Bar3d::SetSize(const Vector2& _size)
{
    sprite_current_->SetSize(_size);
    sprite_max_->SetSize(_size);
}

Vector2 Bar3d::GetHeadUpPositionOnScreen(const Vector3& _targetPos, const Vector2& _hpbarSize, const Camera& _camera, float _offsetY)
{
    // 3D空間上のHPバー座標
    Vector3 pos3d = _targetPos;
    pos3d.y += _offsetY;

    const auto& vpMatrix    = _camera.GetViewProjectionMatrix();
    auto        vMatrix     = Mat4x4::MakeViewport(0, 0, static_cast<float>(WinApp::clientWidth), static_cast<float>(WinApp::clientHeight), 0.0f, 1.0f);
    auto        vpvMatrix   = Mat4x4::Multiply(vpMatrix, vMatrix);
    auto        pos2d       = Mat4x4::Transform(vpvMatrix, pos3d);

    pos2d.x -= _hpbarSize.x * 0.5f;
    pos2d.y -= _hpbarSize.y * 0.5f;

    return Vector2(pos2d.x, pos2d.y);
}


void Bar3d::InitializeSprite(std::unique_ptr<Sprite>& _sprite, const Color& _color)
{
    _sprite = std::make_unique<Sprite>();
    _sprite->Initialize("white.png");
    _sprite->SetColor(_color.Vec4());
}

void Bar3d::CurrentValueSpriteUpdate()
{
    float percentage = valueCurrent_ / valueMax_;
    Vector2 size = sprite_max_->GetSize();
    Vector2 newSize = Vector2(size.x * percentage, size.y);
    sprite_current_->SetSize(newSize);
}

void Bar3d::UpdateDisplayFlagByTimer()
{
    if (isDisplay_ && enableTimer_)
    {
        if (timerDisplay_.GetNow<float>() >= displayTime_)
        {
            isDisplay_ = false;
            timerDisplay_.Stop();
        }
    }
}
