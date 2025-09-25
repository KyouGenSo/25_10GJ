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

    bool IsDisplay() const { return isDisplay_; }

private:
    std::unique_ptr<Sprite> background_ = nullptr;
    std::unique_ptr<Sprite> xbox_ = nullptr;
    std::unique_ptr<Sprite> keyboard_ = nullptr;

    bool isXbox_ = true;
    bool isDisplay_ = false;
};