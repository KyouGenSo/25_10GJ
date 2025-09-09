#pragma once
#include <vector>
#include <Sprite.h>
#include <Bar2d/Bar2d.h>

class TutorialSection
{
public:
    TutorialSection() = default;
    ~TutorialSection() = default;
    void Initialize();
    void Update();
    void Draw();
    void i();

private:
    std::vector<Sprite*> texts_;
    std::unique_ptr<Bar2d> progressBar_;
};