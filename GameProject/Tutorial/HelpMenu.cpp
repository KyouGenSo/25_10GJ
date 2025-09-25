#include "./HelpMenu.h"
#include <Features/Color/Color.h>
#include <WinApp.h>
#include <Input.h>
#include <TextureManager.h>

void HelpMenu::Initialize()
{
    TextureManager::GetInstance()->LoadTexture("Tutorial/HelpMenu_Xbox.png");
    TextureManager::GetInstance()->LoadTexture("Tutorial/HelpMenu_Keyboard.png");

    background_ = std::make_unique<Sprite>();
    background_->Initialize("white.png");
    background_->SetColor(Color(0x2e2e2eff).Vec4());
    background_->SetSize({ static_cast<float>(WinApp::clientWidth), static_cast<float>(WinApp::clientHeight) });
    background_->SetAnchorPoint({ 0.5f, 0.5f });
    background_->SetPos({ static_cast<float>(WinApp::clientWidth) / 2.0f, static_cast<float>(WinApp::clientHeight) / 2.0f });

    xbox_ = std::make_unique<Sprite>();
    xbox_->Initialize("Tutorial/HelpMenu_Xbox.png");
    xbox_->SetAnchorPoint({ 0.5f, 0.5f });
    xbox_->SetPos({ static_cast<float>(WinApp::clientWidth) / 2.0f, static_cast<float>(WinApp::clientHeight) / 2.0f });
    
    keyboard_ = std::make_unique<Sprite>();
    keyboard_->Initialize("Tutorial/HelpMenu_Keyboard.png");
    keyboard_->SetAnchorPoint({ 0.5f, 0.5f });
    keyboard_->SetPos({ static_cast<float>(WinApp::clientWidth) / 2.0f, static_cast<float>(WinApp::clientHeight) / 2.0f });
}

void HelpMenu::Update()
{
    if (Input::GetInstance()->TriggerButton(XButtons.Start))
    {
        isXbox_ = true;
        isDisplay_ = !isDisplay_;
    }
    else if (Input::GetInstance()->TriggerKey(DIK_TAB))
    {
        isXbox_ = false;
        isDisplay_ = !isDisplay_;
    }

    // 表示していないなら更新しない
    if (!isDisplay_) return;

    background_->Update();
    xbox_->Update();
    keyboard_->Update();
}

void HelpMenu::Draw2d()
{
    if (!isDisplay_) return;

    background_->Draw();

    if (isXbox_) xbox_->Draw();
    else keyboard_->Draw();
}

void HelpMenu::Finalize()
{

}

void HelpMenu::ImGui()
{

}
