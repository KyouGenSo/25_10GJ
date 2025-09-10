#include "Tutorial.h"
#include <imgui.h>


void Tutorial::CreateSection(const std::string& sectionName)
{
    if (sectionName == "ShootTheCBD")
    {
        this->CreateSectionShootTheCBD();
    }
}

void Tutorial::CreateSpriteExplanation(const std::string& filePath, const Vector2& position)
{
    auto& sprite = spriteExplanation_.emplace_back(std::make_unique<Sprite>());
    sprite->Initialize(filePath);
    sprite->SetPos(position);
}

void Tutorial::CreateSpriteButton(const std::string& filePath, const Vector2& position)
{
    auto& sprite = spriteButtonText_.emplace_back(std::make_unique<Sprite>());
    sprite->Initialize(filePath);
    sprite->SetPos(position);
}

void Tutorial::CreateSectionShootTheCBD()
{
    spriteExplanation_.clear();
    spriteButtonText_.clear();

    pTextureManager_->LoadTexture("Tutorial/Text-1.png");
    pTextureManager_->LoadTexture("Tutorial/L-Trigger.png");
    this->CreateSpriteExplanation("Tutorial/Text-1.png", { 697.0f, 156.0f });
    this->CreateSpriteButton("Tutorial/L-Trigger.png", { 617.0f, 156.0f });

    auto inTween = AnimationTween<float>(0.0f, 1.5f, 0.0f, 1.0f);
    inTween.SetTransitionFunction([](float t)
    {
        return t * t * t * t * t; // 二次関数でイージング
    });
    auto outTween = AnimationTween<float>(0.0f, 0.5f, 1.0f, 0.0f);

    section_ = std::make_unique<TutorialSection>();
    section_->Initialize(100.0f, inTween, outTween);
    for (auto& sprite : spriteExplanation_) section_->AddText(sprite.get());
    section_->AddTextButton(TutorialSection::InputButtons::LTrigger, spriteButtonText_[0].get());
}

void Tutorial::CreateSectionChangeColor()
{
    spriteExplanation_.clear();
    spriteButtonText_.clear();

    pTextureManager_->LoadTexture("Tutorial/Text-2.png");
    pTextureManager_->LoadTexture("Tutorial/DPad.png");
    this->CreateSpriteExplanation("Tutorial/Text-2.png", { 716.0f, 156.0f });
    this->CreateSpriteButton("Tutorial/DPad.png", { 630.0f, 156.0f });
    section_ = std::make_unique<TutorialSection>();
    section_->Initialize(100.0f);

    for (auto& sprite : spriteExplanation_) section_->AddText(sprite.get());
    section_->AddTextButton(TutorialSection::InputButtons::DPadAll, spriteButtonText_.back().get());
}

void Tutorial::SectionProgressUpdate()
{
    switch (currentSection_)
    {
    case Sections::ShootTheCBD:
        if (eventRecorder_->HasEvent("ShootTheCBD"))
        {
            section_->Progress(50.0f);
            eventRecorder_->ResetEvent("ShootTheCBD");
        }
        break;
    case Sections::ChangeColor:
        if (eventRecorder_->HasEvent("ChangeColor"))
        {
            section_->Progress(50.0f);
            eventRecorder_->ResetEvent("ChangeColor");
        }
        break;
    };
}

void Tutorial::Initialize()
{
    pInput_ = Input::GetInstance();

    this->CreateSectionShootTheCBD();
    eventRecorder_ = std::make_unique<TutorialEventRecorder>();
}

void Tutorial::Update()
{
    if (pInput_->TriggerKey(DIK_P))
    {
        this->NextSection();
    }

    this->SectionProgressUpdate();
    section_->Update();
}

void Tutorial::Draw2d()
{
    section_->Draw2d();
}

void Tutorial::ImGui()
{
    if (ImGui::Begin("Tutorial"))
    {
        ImGui::SeparatorText("Position");
        ImGui::Indent(15.0f);
        for (int i = 0; i < spriteExplanation_.size(); ++i)
        {
            Vector2 pos = spriteExplanation_[i]->GetPos();
            std::string label = "Explanation Position" + std::to_string(i);
            ImGui::DragFloat2(label.c_str(), &pos.x, 0.01f);
            spriteExplanation_[i]->SetPos(pos);
        }
        for (int i = 0; i < spriteButtonText_.size(); ++i)
        {
            Vector2 pos = spriteButtonText_[i]->GetPos();
            std::string label = "Button Text Position" + std::to_string(i);
            ImGui::DragFloat2(label.c_str(), &pos.x, 0.01f);
            spriteButtonText_[i]->SetPos(pos);
        }
        ImGui::Unindent(15.0f);
        ImGui::End();
    }
}

void Tutorial::NextSection()
{
    int iCurrentSection = static_cast<int>(currentSection_);
    currentSection_ = static_cast<Sections>((iCurrentSection + 1) % static_cast<int>(Sections::End));
    switch (currentSection_)
    {
    case Sections::ShootTheCBD:
        this->CreateSectionShootTheCBD();
        break;
    case Sections::ChangeColor:
        this->CreateSectionChangeColor();
        break;
    }
}
