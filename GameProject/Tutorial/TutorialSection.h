#pragma once
#include <vector>
#include <Sprite.h>
#include <Bar2d/Bar2d.h>
#include <Input.h>

class TutorialSection
{
public:
    TutorialSection() = default;
    ~TutorialSection() = default;

    virtual void Initialize();
    virtual void Update();
    virtual void Draw();
    virtual void ImGui();

    void AddText(Sprite* text) { texts_.emplace_back(text); }
    void SetTextButton(Sprite* textButton) { textButton_ = textButton; }

protected:
    std::vector<Sprite*> texts_;
    Sprite* textButton_ = nullptr;
    std::unique_ptr<Sprite> background_ = nullptr;
    std::unique_ptr<Bar2d> progressBar_;

    Input* pInput_ = nullptr;
};