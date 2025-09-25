#include "./HelpMenu.h"
#include <Features/Color/Color.h>
#include <WinApp.h>
#include <Input.h>
#include <TextureManager.h>

void HelpMenu::Initialize()
{
    TextureManager::GetInstance()->LoadTexture("Tutorial/HelpMenu.png");
    TextureManager::GetInstance()->LoadTexture("Tutorial/HelpMenuKey.png");

    background_ = std::make_unique<Sprite>();
    background_->Initialize("white.png");
    background_->SetColor(Color(0x2e2e2eff).Vec4());
    background_->SetSize({ static_cast<float>(WinApp::clientWidth), static_cast<float>(WinApp::clientHeight) });
    background_->SetAnchorPoint({ 0.5f, 0.5f });
    background_->SetPos({ static_cast<float>(WinApp::clientWidth) / 2.0f, static_cast<float>(WinApp::clientHeight) / 2.0f });

    mainPad_ = std::make_unique<Sprite>();
    mainPad_->Initialize("Tutorial/HelpMenu.png");
    mainPad_->SetAnchorPoint({ 0.5f, 0.5f });
    mainPad_->SetPos({ static_cast<float>(WinApp::clientWidth) / 2.0f, static_cast<float>(WinApp::clientHeight) / 2.0f });

    mainKey_ = std::make_unique<Sprite>();
    mainKey_->Initialize("Tutorial/HelpMenuKey.png");
    mainKey_->SetAnchorPoint({ 0.5f, 0.5f });
    mainKey_->SetPos({ static_cast<float>(WinApp::clientWidth) / 2.0f, static_cast<float>(WinApp::clientHeight) / 2.0f });
}

void HelpMenu::Update()
{
    background_->Update();
    mainPad_->Update();
    mainKey_->Update();

    if (Input::GetInstance()->TriggerButton(XButtons.Start))
    {
        isDisplayPad_ = !isDisplayPad_;
    }

    if (Input::GetInstance()->TriggerKey(DIK_ESCAPE))
    {
        isDisplayKey_ = !isDisplayKey_;
    }
}

void HelpMenu::Draw2d()
{
    if (!isDisplayPad_ && !isDisplayKey_) return;

    background_->Draw();

    if (isDisplayKey_) mainKey_->Draw();
    if (isDisplayPad_) mainPad_->Draw();
}

void HelpMenu::Finalize()
{

}

void HelpMenu::ImGui()
{

}
