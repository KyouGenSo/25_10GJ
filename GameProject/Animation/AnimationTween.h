#pragma once
#include <cstdint>

template <typename ValueType>
class AnimationTween
{
public:
    AnimationTween() = default;
    ~AnimationTween() = default;

    inline uint32_t GetNumFrame() const { return numFrame; }

protected:
    float duration = 0.0f;
    ValueType target;
};