#pragma once

#include "./TutorialSection.h"
#include <memory>
#include <vector>
#include <Sprite.h>
#include <TextureManager.h>
#include "TutorialEventRecorder.h"

class Tutorial
{
public:
    enum class Sections
    {
        ShootTheCBD,
        ChangeColor,
        End,
    };

    Tutorial() = default;
    ~Tutorial() = default;
    void Initialize();
    void Update();
    void Draw2d();
    void Finalize();
    void ImGui();
    void NextSection();

    TutorialEventRecorder* GetEventRecorder() { return eventRecorder_.get(); }

private:
    void CreateSection(const std::string& sectionName);
    void CreateSpriteExplanation(const std::string& filePath, const Vector2& position);
    void CreateSpriteButton(const std::string& filePath, const Vector2& position);
    void CreateSectionShootTheCBD();
    void CreateSectionChangeColor();

    // 進捗を更新する関数
    // イベントを監視し、条件が満たされたら進捗を更新する
    void SectionProgressUpdate();

    Sections currentSection_ = Sections::ShootTheCBD;
    std::unique_ptr<TutorialEventRecorder> eventRecorder_;
    std::unique_ptr<TutorialSection> section_;
    std::vector<std::unique_ptr<Sprite>> spriteButtonText_;
    std::vector<std::unique_ptr<Sprite>> spriteExplanation_;

    TextureManager* pTextureManager_ = TextureManager::GetInstance();
    Input* pInput_ = nullptr;
};