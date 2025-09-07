#include "TitleAnimation.h"
#include <WinApp.h>

void TitleAnimation::Initialize()
{
    initialTitleSize1_ = pSpriteTitle1_->GetSize();
    initialTitleSize2_ = pSpriteTitle2_->GetSize();

    // Scale Animation
    {
        AnimationTween<float> tween(0.0f, 5.0f, 0.75f, 1.0f);
        tween.SetTransitionFunction([](float t) { return t * t; }); // Ease In Quad
        animScale_.AddTween(tween);
    }
    // Alpha Animation
    {
        AnimationTween<float> tween1(4.5f, 0.5f, 1.0f, 0.0f);
        AnimationTween<float> tween2(5.0f, 0.5f, 0.0f, 1.0f);
        animAlpha1_.AddTween(tween1);
        animAlpha2_.AddTween(tween2);
    }
    // PositionY2 Animation
    {
        const float targetY2 = static_cast<float>(WinApp::clientHeight) / 3.0f;
        AnimationTween<float> tween(5.0f, 1.5f, -50.0f, targetY2);
        tween.SetTransitionFunction([](float t) { return t * (2 - t); }); // Ease Out Quad
        animPositionY_.AddTween(tween);
    }
    // Background Colors Animation
    {
        AnimationTween<float> tween(0.0f, 5.0f, 0.0f, 1.0f);
        tween.SetTransitionFunction([](float t) { return t * t; }); // Ease In Quad
        animBackgroundRGB_.AddTween(tween);
    }
    // Background Alpha Animation
    {
        AnimationTween<float> tween(5.0f, 0.5f, 1.0f, 0.0f);
        animBackgroundAlpha_.AddTween(tween);
    }
    this->Reset();
}

void TitleAnimation::Update()
{
    scale_ = animScale_.Update();
    alpha1_ = animAlpha1_.Update();
    alpha2_ = animAlpha2_.Update();
    positionY_ = animPositionY_.Update();
    backgroundRGB_ = animBackgroundRGB_.Update();
    backgroundAlpha_ = animBackgroundAlpha_.Update();

    if (pSpriteTitle1_)
    {
        pSpriteTitle1_->SetSize(
            Vector2(
                initialTitleSize1_.x * scale_,
                initialTitleSize1_.y * scale_
            )
        );
        pSpriteTitle1_->SetPos(
            Vector2(
                static_cast<float>(WinApp::clientWidth) / 2.0f,
                positionY_
            )
        );
        Vector4 color = pSpriteTitle1_->GetColor();
        color.w = alpha1_;
        pSpriteTitle1_->SetColor(color);
    }

    if (pSpriteTitle2_)
    {
        pSpriteTitle2_->SetSize(
            Vector2(
                initialTitleSize2_.x * scale_,
                initialTitleSize2_.y * scale_
            )
        );
        pSpriteTitle2_->SetPos(
            Vector2(
                static_cast<float>(WinApp::clientWidth) / 2.0f,
                positionY_
            )
        );
        Vector4 color = pSpriteTitle2_->GetColor();
        color.w = alpha2_;
        pSpriteTitle2_->SetColor(color);
    }

    if (pSpriteBackground_)
    {
        pSpriteBackground_->SetColor(Vector4(backgroundRGB_, backgroundRGB_, backgroundRGB_, backgroundAlpha_));
    }
}

void TitleAnimation::Reset()
{
    animScale_.Start(0.0f);
    animAlpha1_.Start(1.0f);
    animAlpha2_.Start(0.0f);
    {
        const float targetY1 = static_cast<float>(WinApp::clientHeight) / 2.0f;
        animPositionY_.Start(targetY1);
    }
    animBackgroundRGB_.Start(0.0f);
    animBackgroundAlpha_.Start(1.0f);
}

void TitleAnimation::ImGui()
{
    #ifdef _DEBUG

    bool isOpen = ImGui::Begin("Title Animation Debug");
    if (isOpen)
    {
        if (ImGui::Button("Reset Animation"))
        {
            this->Reset();
        }

        ImGui::Text("Scale = %f", scale_);
        ImGui::Text("Alpha1 = %f", alpha1_);
        ImGui::Text("Alpha2 = %f", alpha2_);
        ImGui::Text("Position Y = %f", positionY_);
        ImGui::Text("Background RGB = %f", backgroundRGB_);
        ImGui::Text("Background Alpha = %f", backgroundAlpha_);
        animScale_.ImGui("Scale");
        animAlpha1_.ImGui("Alpha1");
        animAlpha2_.ImGui("Alpha2");
        animPositionY_.ImGui("Position Y");
        animBackgroundRGB_.ImGui("Background RGB");
        animBackgroundAlpha_.ImGui("Background Alpha");
    }
    ImGui::End();

    #endif // _DEBUG
}
