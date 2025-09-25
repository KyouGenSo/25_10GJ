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

    bool IsDisplay() const { return isDisplayKeyboard_; }

private:
    std::unique_ptr<Sprite> background_ = nullptr;
    std::unique_ptr<Sprite> xbox_ = nullptr;
    std::unique_ptr<Sprite> keyboard_ = nullptr;

    bool isDisplay_ = false;
    bool isDisplayXbox_ = false;
    bool isDisplayKeyboard_ = false;
};