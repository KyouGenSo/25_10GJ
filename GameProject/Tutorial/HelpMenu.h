#pragma once
#include <memory>
#include <Sprite.h>

class HelpMenu
{
public:
    HelpMenu() = default;
    ~HelpMenu() = default;
    void Initialize();
    void Update();
    void Draw2d();
    void Finalize();
    void ImGui();

    bool IsDisplay() const { return (isDisplayPad_ || isDisplayKey_); }

private:
    std::unique_ptr<Sprite> background_ = nullptr;
    std::unique_ptr<Sprite> mainPad_ = nullptr;
    std::unique_ptr<Sprite> mainKey_ = nullptr;

    bool isDisplayPad_ = false;
    bool isDisplayKey_ = false;
};