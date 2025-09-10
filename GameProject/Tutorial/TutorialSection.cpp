#include "TutorialSection.h"
#include <WinApp.h>
#include <TextureManager.h>

void TutorialSection::Initialize(float targetProgress)
{
    pInput_ = Input::GetInstance();
    TextureManager::GetInstance()->LoadTexture("tutorial/bg.png");

    progressBar_ = std::make_unique<Bar2d>();
    progressBar_->Initialize("", { 766.0f, 17.0f }, false);
    progressBar_->SetAnchorPoint({ 0.5f, 0.0f });
    progressBar_->SetPosition({ static_cast<float>(WinApp::clientWidth / 2.0f), 205.0f });
    progressBar_->SetMaxValue(targetProgress);

    background_ = std::make_unique<Sprite>();
    background_->Initialize("tutorial/bg.png");
    background_->SetAnchorPoint({ 0.5f, 0.0f });
    background_->SetPos({ WinApp::clientWidth / 2.0f, 65.0f });

    timer_.Reset();
    timer_.Start();
}

void TutorialSection::Initialize(float targetProgress, const AnimationTween<float>& _in, const AnimationTween<float>& _out)
{
    fadeInTween_ = std::make_unique<AnimationTween<float>>(_in);
    fadeOutTween_ = std::make_unique<AnimationTween<float>>(_out);
    this->Initialize(targetProgress);
}

void TutorialSection::Update()
{
    this->FadeInOutUpdate();
    progressBar_->Update();
    background_->Update();

    for (auto& text : texts_)
    {
        text->Update();
    }

    this->KeyTextUpdate();
}

void TutorialSection::Draw2d()
{
    background_->Draw();
    progressBar_->Draw2D();

    for (auto& text : texts_)
    {
        text->Draw();
    }

    for (auto& textButton : textsButton_)
    {
        textButton.second->Draw();
    }
}

void TutorialSection::ImGui()
{
    progressBar_->ImGui();
}

void TutorialSection::Progress(float numProgress)
{
    if (isReached_) return;

    *progressBar_ += numProgress;

    if (progressBar_->GetCurrentRatio() >= 1.0f)
    {
        isReached_ = true;
        timer_.Reset();
        timer_.Start();
    }
}

void TutorialSection::KeyTextUpdate()
{
    for (auto& [button, textButton] : textsButton_)
    {
        if (IsPush(pInput_, button))
        {
            textButton->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        }
        else
        {
            textButton->SetColor({ 0.5f, 0.5f, 0.5f, 1.0f });
        }

        textButton->Update();
    }
}

void TutorialSection::FadeInOutUpdate()
{
    float alpha = 1.0f;
    if (fadeInTween_ && !isReached_)
    {
        fadeInTween_->Update(timer_.GetNow<float>(), alpha);
    }
    if (fadeOutTween_ && isReached_)
    {
        fadeOutTween_->Update(timer_.GetNow<float>(), alpha);
    }

    background_->SetColor({ 1.0f, 1.0f, 1.0f, alpha });
    progressBar_->SetOpacity(alpha);
    for (auto& text : texts_)
    {
        text->SetColor({ 1.0f, 1.0f, 1.0f, alpha });
    }
    for (auto& [button, textButton] : textsButton_)
    {
        textButton->SetColor({ 1.0f, 1.0f, 1.0f, alpha });
    }
    if (alpha <= 0.0f)
    {
        isComplete_ = true;
    }
}

bool TutorialSection::IsPush(Input* pInput, InputButtons button)
{
    bool isKey = button < InputButtons::KeyboardEnd;
    bool isTrigger = button > InputButtons::TriggerBegin && button < InputButtons::TriggerEnd;
    bool isButton = button > InputButtons::ButtonBegin && button < InputButtons::ButtonEnd;
    bool isDPadAll = button == InputButtons::DPadAll;

    if (isKey)
    {
        return pInput->PushKey(ToDIK(button));
    }
    else if (isTrigger)
    {
        if (button == InputButtons::LTrigger)
        {
            return pInput->GetLeftTrigger() > 0.0f;
        }
        else if (button == InputButtons::RTrigger)
        {
            return pInput->GetRightTrigger() > 0.0f;
        }
    }
    else if (isDPadAll)
    {
        return pInput->PushButton(XButtons.DPad_Up) ||
            pInput->PushButton(XButtons.DPad_Down) ||
            pInput->PushButton(XButtons.DPad_Left) ||
            pInput->PushButton(XButtons.DPad_Right);
    }
    else if (isButton)
    {
        return pInput->PushButton(ToXInput(button));
    }
}

BYTE TutorialSection::ToDIK(InputButtons button)
{
    static const std::unordered_map<InputButtons, BYTE> dikTable =
    {
        {InputButtons::Enter, DIK_RETURN},
        {InputButtons::Space, DIK_SPACE},
        {InputButtons::W, DIK_W},
        {InputButtons::A, DIK_A},
        {InputButtons::S, DIK_S},
        {InputButtons::D, DIK_D},
    };

    return dikTable.at(button);
}

int TutorialSection::ToXInput(InputButtons button)
{
    static const std::unordered_map<InputButtons, int> xInputTable =
    {
        {InputButtons::ButtonA, XButtons.A},
        {InputButtons::ButtonB, XButtons.B},
        {InputButtons::ButtonX, XButtons.X},
        {InputButtons::ButtonY, XButtons.Y},
        {InputButtons::LShoulder, XButtons.L_Shoulder},
        {InputButtons::RShoulder, XButtons.R_Shoulder},
        {InputButtons::DPadUp, XButtons.DPad_Up},
        {InputButtons::DPadDown, XButtons.DPad_Down},
        {InputButtons::DPadLeft, XButtons.DPad_Left},
        {InputButtons::DPadRight, XButtons.DPad_Right},
    };
    return xInputTable.at(button);
}
