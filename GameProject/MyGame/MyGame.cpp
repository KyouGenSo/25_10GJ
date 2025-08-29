#include "MyGame.h"
#include"Audio.h"
#include"Input.h"
#include "scene/SceneFactory.h"
#include "SceneManager.h"
#include "TextureManager.h"
#include "Draw2D.h"
#include "Object3dBasic.h"
#include "PostEffectManager.h"
#include "FrameTimer.h"
#include "GlobalVariables.h"
#include "ModelManager.h"
#include "GPUParticle.h"
#include "SpriteBasic.h"
#include "Transition.h"

void MyGame::Initialize()
{

  TakoFramework::Initialize();

  winApp_->SetWindowSize(1280, 720);
  winApp_->SetWindowTitle(L"TakoEngine Sample Game");

#pragma region 汎用機能初期化-------------------------------------------------------------------------------------------------------------------
  // 入力クラスの初期化
  Input::GetInstance()->Initialize(winApp_);

  // オーディオの初期化
  Audio::GetInstance()->Initialize("resources/Sound/");

#pragma endregion

  // シーンの初期化
  sceneFactory_ = new SceneFactory();
  SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);
  SceneManager::GetInstance()->ChangeScene("title", 0.0f);

  TextureManager::GetInstance()->LoadTexture("white.png");
  TextureManager::GetInstance()->LoadTexture("circle.png");
  TextureManager::GetInstance()->LoadTexture("my_skybox.dds");

  spriteBasicOnresizeId = winApp_->RegisterOnResizeFunc(std::bind(&SpriteBasic::OnResize, SpriteBasic::GetInstance(), std::placeholders::_1));

  // GPUパーティクルの初期化
  GPUParticle::GetInstance()->Initialize(dx12_, defaultCamera_);
}

void MyGame::Finalize()
{
  winApp_->UnregisterOnResizeFunc(spriteBasicOnresizeId);

  // GPUパーティクルの解放
  GPUParticle::GetInstance()->Finalize();

  // Audioの解放
  Audio::GetInstance()->Finalize();

  // 入力クラスの解放
  Input::GetInstance()->Finalize();

  TakoFramework::Finalize();
}

void MyGame::Update()
{
  // カメラの更新
  defaultCamera_->Update();

  // 入力情報の更新
  Input::GetInstance()->Update();

  // F11キーでフルスクリーン切り替え
  if (Input::GetInstance()->TriggerKey(DIK_F11))
  {
    ToggleFullScreen();
  }

  // GPUパーティクルの更新
  GPUParticle::GetInstance()->Update();

  TakoFramework::Update();

  //　サウンドの更新
  Audio::GetInstance()->Update();

  // ゲームパッドの状態をリスレッシュ
  Input::GetInstance()->RefreshGamePadState();
}

void MyGame::Draw()
{
  /// ============================================= ///
  /// ------------------シーン描画-------------------///
  /// ============================================= ///

  //ポストエフェクト適用対象のレンダーテクスチャを描画先に設定
  dx12_->SetEffectRenderTexture();

  // テクスチャ用のsrvヒープの設定
  SrvManager::GetInstance()->BeginDraw();

  SceneManager::GetInstance()->Draw();

  GPUParticle::GetInstance()->Draw();

  Draw2D::GetInstance()->Draw();

  /// ===================================================== ///
  /// ------------------ポストエフェクト描画-------------------///
  /// ===================================================== ///

    // ポストエフェクトの描画
  PostEffectManager::GetInstance()->Draw();

  /// ===================================================== ///
  /// ------------ポストエフェクト非適用対象の描画---------------///
  /// ===================================================== ///
  // ポストエフェクト非適用対象のレンダーテクスチャを描画先に設定
  dx12_->SetNonEffectRenderTexture();

  // シーンの描画
  SceneManager::GetInstance()->DrawWithoutEffect();

  Transition::GetInstance()->Draw();

  Draw2D::GetInstance()->Reset();

  /// ============================================= ///
  /// ---------最終結果をスワップチェーンに描画---------///
  /// ============================================= ///
  PostEffectManager::GetInstance()->DrawFinalResult();


  /// ========================================= ///
  ///-------------------ImGui-------------------///
  /// ========================================= ///
#ifdef _DEBUG

  imguiManager_->Begin();

  SceneManager::GetInstance()->DrawImGui();

  Draw2D::GetInstance()->ImGui();

  // GlobalVariablesの更新
  GlobalVariables::GetInstance()->Update();

  ImGui::Begin("Option");
  // buttonでFPSの表示を切り替え
  if (ImGui::Button("Display FPS"))
  {
    FPSWindowVisible = !FPSWindowVisible;
  }
  ImGui::SameLine();
  if (ImGui::Button("PostEffect Option"))
  {
    PostEffectWindowVisible = !PostEffectWindowVisible;
  }

  ImGui::End();

  // fpsの表示
  if (FPSWindowVisible)
  {
    ImGui::Begin("FPS", &FPSWindowVisible);
    ImGui::ProgressBar(FrameTimer::GetInstance()->GetFPS() / 60.0f, ImVec2(0.0f, 0.0f), "");
    ImGui::SameLine();
    ImGui::Text("FPS : %.0f", FrameTimer::GetInstance()->GetFPS());
    ImGui::End();
  }


  // PostEffectのパラメータ調整
  if (PostEffectWindowVisible) {
    PostEffectManager::GetInstance()->DrawImgui();
  }

  imguiManager_->End();

  //imguiの描画
  imguiManager_->Draw();
#endif


  // 描画後の処理
  dx12_->EndDraw();
}
