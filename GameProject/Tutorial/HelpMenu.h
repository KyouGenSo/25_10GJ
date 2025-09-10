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
    std::unique_ptr<Sprite> main_ = nullptr;

    bool isDisplay_ = false;
};