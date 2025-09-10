#pragma once
#include <vector>
#include <Sprite.h>
#include <Bar2d/Bar2d.h>
#include <Input.h>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include <Windows.h>
#include <Animation/AnimationTween.hpp>
#include <Features/TimeMeasurer/TimeMeasurer.h>

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
        DPadAll,
        DPadUp,
        DPadDown,
        DPadLeft,
        DPadRight,
        ButtonEnd,
    };

    TutorialSection() = default;
    ~TutorialSection() = default;

    void Initialize(float targetProgress);
    void Initialize(float targetProgress, const AnimationTween<float>& _in, const AnimationTween<float>& _out);
    void Update();
    void Draw2d();
    void ImGui();
    inline bool IsComplete() const { return isComplete_; }

    void Progress(float numProgress);

    void AddText(Sprite* text) { texts_.emplace_back(text); }
    void AddTextButton(InputButtons numDik, Sprite* textButton) { textsButton_[numDik] = textButton; }

protected:
    void KeyTextUpdate();

    void FadeInOutUpdate();

    static bool IsPush(Input* pInput, InputButtons button);
    static BYTE ToDIK(InputButtons button);
    static int ToXInput(InputButtons button);

    TimeMeasurer timer_;
    std::vector<Sprite*> texts_ = {};
    std::unordered_map<InputButtons, Sprite*> textsButton_ = {};
    std::unique_ptr<Sprite> background_ = nullptr;
    std::unique_ptr<Bar2d> progressBar_ = nullptr;
    std::unique_ptr<AnimationTween<float>> fadeInTween_;
    std::unique_ptr<AnimationTween<float>> fadeOutTween_;

    bool isReached_ = false;
    bool isComplete_ = false;

    Input* pInput_ = nullptr;
};