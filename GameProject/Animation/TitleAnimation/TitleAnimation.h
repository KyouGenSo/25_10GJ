#pragma once

#include <Animation/AnimationTimeline.h>
#include <Sprite.h>
#include <Vector2.h>

class TitleAnimation
{
public:
    TitleAnimation(Sprite* pSpriteTitle1, Sprite* pSpriteTitle2, Sprite* pSpriteBackground) :
        pSpriteTitle1_(pSpriteTitle1),
        pSpriteTitle2_(pSpriteTitle2),
        pSpriteBackground_(pSpriteBackground)
    {}

    ~TitleAnimation() = default;
    void Initialize();
    void Update();
    void Reset();
    void ImGui();

private:
    AnimationTimeline<float> animScale_;
    AnimationTimeline<float> animAlpha1_;
    AnimationTimeline<float> animAlpha2_;
    AnimationTimeline<float> animPositionY_;
    AnimationTimeline<float> animBackgroundRGB_;
    AnimationTimeline<float> animBackgroundAlpha_;

    float scale_ = 1.0f;
    float alpha1_ = 1.0f;
    float alpha2_ = 1.0f;
    float positionY_ = 0.0f;

    float backgroundRGB_ = 0.0f;
    float backgroundAlpha_ = 1.0f;

    Sprite* pSpriteTitle1_ = nullptr;
    Sprite* pSpriteTitle2_ = nullptr;
    Sprite* pSpriteBackground_ = nullptr;

    Vector2 initialTitleSize1_ = { 0.0f, 0.0f };
    Vector2 initialTitleSize2_ = { 0.0f, 0.0f };
};