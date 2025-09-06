#pragma once
#include <functional>

template <typename ValueType>
class AnimationTween
{
public:
    AnimationTween(float startSec, float durationSec, const ValueType& targetValue, const ValueType& startValue) :
        startSec_(startSec),
        durationSec_(durationSec),
        targetValue_(targetValue),
        startValue_(startValue),
        currentValue_(startValue)
    {};

    ~AnimationTween() = default;

    // 補間関数を設定
    inline void SetTransitionFunction(const std::function<float(float)>& func)
    {
        transitionFunction_ = func;
    }

    // アニメーションの開始時間を取得
    inline uint32_t GetDuration() const { return durationSec_; }

    // 現在の値を取得
    const ValueType& GetValue() const { return currentValue_; }

    // アニメーションが終了したかどうかを判定
    bool IsFinished(float currentTime) const
    {
        return currentTime >= (startSec_ + durationSec_);
    }

    // アニメーションの更新
    void Update(float currentTime);

private:
    const float     startSec_       = 0.0f;
    const float     durationSec_    = 0.0f;
    const ValueType targetValue_    = {};
    const ValueType startValue_     = {};
    ValueType       currentValue_   = {};

    // 補間関数
    std::function<float(float)> transitionFunction_ = nullptr;
};

template<typename ValueType>
inline void AnimationTween<ValueType>::Update(float currentTime)
{
    if (currentTime < startSec_) return;

    float t = (currentTime - startSec_) / durationSec_;
    if (t > 1.0f) t = 1.0f;

    // 補間関数が設定されていれば適用
    if (transitionFunction_)
    {
        t = transitionFunction_(t);
    }

    currentValue_ = startValue_ + (targetValue_ - startValue_) * t; // ここでは単純な線形補間を仮定
}
