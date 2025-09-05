#include "TitleScene.h"
#include "SceneManager.h"
#include "TextureManager.h"
#include "Object3dBasic.h"
#include "SpriteBasic.h"
#include "ModelManager.h"
#include "Draw2D.h"
#include "GlobalVariables.h"
#include "GPUParticle.h"

#ifdef _DEBUG
#include"ImGui.h"
#include "DebugCamera.h"
#endif

void TitleScene::Initialize()
{
    #ifdef _DEBUG
    DebugCamera::GetInstance()->Initialize();
    Object3dBasic::GetInstance()->SetDebug(false);
    Draw2D::GetInstance()->SetDebug(false);
    GPUParticle::GetInstance()->SetIsDebug(false);
    #endif

    input_ = Input::GetInstance();

    /// ================================== ///
    ///              初期化処理              ///
    /// ================================== ///
    this->SpritesInitialize();
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
    #ifdef _DEBUG
    if (Input::GetInstance()->TriggerKey(DIK_F1))
    {
        Object3dBasic::GetInstance()->SetDebug(!Object3dBasic::GetInstance()->GetDebug());
        Draw2D::GetInstance()->SetDebug(!Draw2D::GetInstance()->GetDebug());
        GPUParticle::GetInstance()->SetIsDebug(!GPUParticle::GetInstance()->GetIsDebug());
        isDebug_ = !isDebug_;
    }

    if (isDebug_)
    {
        DebugCamera::GetInstance()->Update();
    }
    #endif
    /// ================================== ///
    ///              更新処理               ///
    /// ================================== ///

    this->SpritesUpdate();

    if (Input::GetInstance()->TriggerKey(DIK_RETURN))
    {
        SceneManager::GetInstance()->ChangeScene("game");
    }
}

void TitleScene::Draw()
{
    /// ================================== ///
    ///              描画処理               ///
    /// ================================== ///

    //------------------背景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();




    //-------------------Modelの描画-------------------//
    // 3Dモデル共通描画設定
    Object3dBasic::GetInstance()->SetCommonRenderSetting();




    //------------------前景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();



}

void TitleScene::DrawWithoutEffect()
{
    /// ================================== ///
    ///              描画処理               ///
    /// ================================== ///

    //------------------背景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();




    //-------------------Modelの描画-------------------//
    // 3Dモデル共通描画設定
    Object3dBasic::GetInstance()->SetCommonRenderSetting();





    //------------------前景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();
    if (currentTitleSpriteNumber_ == 0)
    {
        spriteToon_->Draw();
    }
    else
    {
        spriteRaider_->Draw();
    }

}

void TitleScene::DrawImGui()
{
    #ifdef _DEBUG

    /// ================================== ///
    ///             ImGuiの描画              ///
    /// ================================== ///


    #endif // _DEBUG
}

void TitleScene::SpritesInitialize()
{
    const Vector2 screenSize =
    {
        static_cast<float>(WinApp::clientWidth),
        static_cast<float>(WinApp::clientHeight)
    };

    const Vector2 position =
    {
        screenSize.x / 2.0f,
        screenSize.y / 2.0f
    };

    TextureManager::GetInstance()->LoadTexture("Toon.png");
    TextureManager::GetInstance()->LoadTexture("raider.png");

    spriteToon_ = std::make_unique<Sprite>();
    spriteToon_->Initialize("Toon.png");
    spriteToon_->SetSize(screenSize);
    spriteToon_->SetPos(position);
    spriteToon_->SetAnchorPoint(Vector2(0.5f, 0.5f));

    spriteRaider_ = std::make_unique<Sprite>();
    spriteRaider_->Initialize("raider.png");
    spriteRaider_->SetSize(screenSize);
    spriteRaider_->SetPos(position);
    spriteRaider_->SetAnchorPoint(Vector2(0.5f, 0.5f));
}

void TitleScene::SpritesUpdate()
{
    if (input_->TriggerKey(DIK_SPACE))
    {
        currentTitleSpriteNumber_++;
        currentTitleSpriteNumber_ %= 2;
    }

    spriteToon_->Update();
    spriteRaider_->Update();
}
