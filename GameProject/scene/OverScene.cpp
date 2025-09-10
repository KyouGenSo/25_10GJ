#include "OverScene.h"

#include "Draw2D.h"
#include "GPUParticle.h"
#include "Input.h"
#include "Object3dBasic.h"
#include "SceneManager.h"
#include "SpriteBasic.h"
#include "TextureManager.h"

#ifdef _DEBUG
#include"ImGui.h"
#include "DebugCamera.h"
#endif

void OverScene::Initialize()
{
#ifdef _DEBUG
    DebugCamera::GetInstance()->Initialize();
    Object3dBasic::GetInstance()->SetDebug(false);
    Draw2D::GetInstance()->SetDebug(false);
    GPUParticle::GetInstance()->SetIsDebug(false);
#endif

    /// ================================== ///
    ///              初期化処理              ///
    /// ================================== ///
    
    // 背景スプライトの初期化
    bgSprite_ = std::make_unique<Sprite>();
    bgSprite_->Initialize("black_BG.png");
    bgSprite_->SetPos(bgPos_);
    //bgSprite_->SetSize(Vector2(1920.0f, 1080.0f));
    bgSprite_->SetColor(Vector4(0.2f, 0.2f, 0.2f, 0.8f));
    
    // GameOverスプライトの初期化
    overSprite_ = std::make_unique<Sprite>();
    overSprite_->Initialize("gameOver_Text.png");
    overSprite_->SetPos(overPos_);
    //overSprite_->SetSize(overSize_);
    //overSprite_->SetAnchorPoint(Vector2(0.5f, 0.5f));
    overSprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    
    // ボタンスプライトの初期化
    buttonSprite_ = std::make_unique<Sprite>();
    buttonSprite_->Initialize("press_text.png");
    buttonSprite_->SetPos(buttonPos_);
    //buttonSprite_->SetSize(buttonSize_);
    //buttonSprite_->SetAnchorPoint(Vector2(0.5f, 0.5f));
    buttonSprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
}


void OverScene::Finalize()
{

}

void OverScene::Update()
{
#ifdef _DEBUG
    if (Input::GetInstance()->TriggerKey(DIK_F1)) {
        Object3dBasic::GetInstance()->SetDebug(!Object3dBasic::GetInstance()->GetDebug());
        Draw2D::GetInstance()->SetDebug(!Draw2D::GetInstance()->GetDebug());
        GPUParticle::GetInstance()->SetIsDebug(!GPUParticle::GetInstance()->GetIsDebug());
        isDebug_ = !isDebug_;
    }

    if (isDebug_) {
        DebugCamera::GetInstance()->Update();
    }
#endif
    /// ================================== ///
    ///              更新処理               ///
    /// ================================== ///
    
    // スプライトの更新処理
    bgSprite_->Update();
    overSprite_->Update();
    buttonSprite_->Update();

    if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->TriggerButton(XButtons.A))
    {
        SceneManager::GetInstance()->ChangeScene("title");
    }
}

void OverScene::Draw()
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

void OverScene::DrawWithoutEffect()
{
    /// ================================== ///
    ///              描画処理               ///
    /// ================================== ///

    //------------------背景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();
    
    bgSprite_->Draw();


    //-------------------Modelの描画-------------------//
    // 3Dモデル共通描画設定
    Object3dBasic::GetInstance()->SetCommonRenderSetting();





    //------------------前景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();
    
    overSprite_->Draw();
    buttonSprite_->Draw();


}

void OverScene::DrawImGui()
{
#ifdef _DEBUG

    /// ================================== ///
    ///             ImGuiの描画              ///
    /// ================================== ///
    
    ImGui::Begin("OverScene Debug");
    
    // 背景スプライト
    if (ImGui::TreeNode("Background Sprite"))
    {
        ImGui::DragFloat2("Position", &bgPos_.x);
        Vector4 bgColor = bgSprite_->GetColor();
        if (ImGui::ColorEdit4("Color", &bgColor.x))
        {
            bgSprite_->SetColor(bgColor);
        }
        ImGui::TreePop();
    }
    
    // GameOverスプライト
    if (ImGui::TreeNode("GameOver Sprite"))
    {
        ImGui::DragFloat2("Position", &overPos_.x);
        ImGui::DragFloat2("Size", &overSize_.x);
        Vector4 overColor = overSprite_->GetColor();
        if (ImGui::ColorEdit4("Color", &overColor.x))
        {
            overSprite_->SetColor(overColor);
        }
        ImGui::TreePop();
    }
    
    // ボタンスプライト
    if (ImGui::TreeNode("Button Sprite"))
    {
        ImGui::DragFloat2("Position", &buttonPos_.x);
        ImGui::DragFloat2("Size", &buttonSize_.x);
        Vector4 buttonColor = buttonSprite_->GetColor();
        if (ImGui::ColorEdit4("Color", &buttonColor.x))
        {
            buttonSprite_->SetColor(buttonColor);
        }
        ImGui::TreePop();
    }
    
    ImGui::End();

#endif // _DEBUG
}