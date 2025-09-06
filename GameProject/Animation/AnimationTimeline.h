#pragma once
#include <memory>
#include <vector>
#include "./AnimationTween.hpp"
#include <Features/TimeMeasurer/TimeMeasurer.h>

template <typename ValueType>
class AnimationTimeline
{
public:
    inline AnimationTimeline()
    {
        currentTime_ = std::make_unique<TimeMeasurer>();
    }

    ~AnimationTimeline() = default;

    // Tweenを追加
    inline void AddTween(const AnimationTween<ValueType>& tween)
    {
        tweens_.emplace_back(tween);
    }

    void Start(ValueType initValue = {})
    {
        if (!currentTime_) return;
        currentTime_->Reset();
        currentTime_->Start();
        currentValue_ = initValue;
    }

    void Update();


private:
    std::unique_ptr<TimeMeasurer> currentTime_ = {};
    std::vector<AnimationTween<ValueType>> tweens_ = {};
    ValueType currentValue_ = {};
};

template<typename ValueType>
inline void AnimationTimeline<ValueType>::Update()
{
    if (!currentTime_) return;

    float time = currentTime_->GetNow<float>();
    for (auto& tween : tweens_)
    {
        if (!tween.IsFinished(time) && tween.GetStartSec() < time)
        {
            tween.Update(time, currentValue_);
            break;
        }
    }
}