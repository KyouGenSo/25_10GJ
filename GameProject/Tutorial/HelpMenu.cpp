#include "./HelpMenu.h"
#include <Features/Color/Color.h>
#include <WinApp.h>
#include <Input.h>
#include <TextureManager.h>

void HelpMenu::Initialize()
{
    TextureManager::GetInstance()->LoadTexture("Tutorial/HelpMenu.png");

    background_ = std::make_unique<Sprite>();
    background_->Initialize("white.png");
    background_->SetColor(Color(0x2e2e2eff).Vec4());
    background_->SetSize({ static_cast<float>(WinApp::clientWidth), static_cast<float>(WinApp::clientHeight) });
    background_->SetAnchorPoint({ 0.5f, 0.5f });
    background_->SetPos({ static_cast<float>(WinApp::clientWidth) / 2.0f, static_cast<float>(WinApp::clientHeight) / 2.0f });

    main_ = std::make_unique<Sprite>();
    main_->Initialize("Tutorial/HelpMenu.png");
    main_->SetAnchorPoint({ 0.5f, 0.5f });
    main_->SetPos({ static_cast<float>(WinApp::clientWidth) / 2.0f, static_cast<float>(WinApp::clientHeight) / 2.0f });
}

void HelpMenu::Update()
{
    background_->Update();
    main_->Update();

    if (Input::GetInstance()->TriggerKey(DIK_TAB) || Input::GetInstance()->TriggerButton(XButtons.Start))
    {
        isDisplay_ = !isDisplay_;
    }
}

void HelpMenu::Draw2d()
{
    if (!isDisplay_) return;

    background_->Draw();
    main_->Draw();
}

void HelpMenu::Finalize()
{

}

void HelpMenu::ImGui()
{

}
