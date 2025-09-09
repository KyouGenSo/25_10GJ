#pragma once
#include <vector>
#include <Sprite.h>
#include <Bar2d/Bar2d.h>
#include <Input.h>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include <Windows.h>

class TutorialSection
{
public:
    enum class InputButtons
    {
        Enter,
        Space,
        W,
        A,
        S,
        D,
        KeyboardEnd,

        TriggerBegin,
        LTrigger,
        RTrigger,
        TriggerEnd,

        ButtonBegin,
        ButtonA,
        ButtonB,
        ButtonX,
        ButtonY,
        LShoulder,
        RShoulder,
        ButtonEnd,
    };

    TutorialSection() = default;
    ~TutorialSection() = default;

    void Initialize(float targetProgress);
    void Update();
    void Draw();
    void ImGui();
    inline bool IsComplete() const { return isComplete_; }

    void Progress(float numProgress);

    void AddText(Sprite* text) { texts_.emplace_back(text); }
    void AddTextButton(InputButtons numDik, Sprite* textButton) { textsButton_[numDik] = textButton; }

protected:
    void KeyTextUpdate();

    static bool IsPush(Input* pInput, InputButtons button);

    std::vector<Sprite*> texts_ = {};
    std::unordered_map<InputButtons, Sprite*> textsButton_ = {};
    std::unique_ptr<Sprite> background_ = nullptr;
    std::unique_ptr<Bar2d> progressBar_ = nullptr;
    bool isComplete_ = false;

    Input* pInput_ = nullptr;
};