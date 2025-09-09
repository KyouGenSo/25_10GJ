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
}

void TutorialSection::Update()
{
    progressBar_->Update();
    background_->Update();

    for (auto& text : texts_)
    {
        text->Update();
    }

    this->KeyTextUpdate();
}

void TutorialSection::Draw()
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
    *progressBar_ += numProgress;
}

void TutorialSection::KeyTextUpdate()
{
    for (auto& [numDik, textButton] : textsButton_)
    {
        if (pInput_->PushKey(numDik))
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
