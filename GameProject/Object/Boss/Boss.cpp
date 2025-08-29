#include "Boss.h"
#include "Object3d.h"

Boss::Boss()
{
}

Boss::~Boss()
{
}

void Boss::Initialize()
{
  model_ = std::make_unique<Object3d>();
  model_->Initialize();
  model_->SetModel("enemy.gltf");

  transform_.translate = Vector3(0.0f, 2.5f, 10.0f);
  transform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
  transform_.scale = Vector3(1.0f, 1.0f, 1.0f);

  model_->SetTransform(transform_);
}

void Boss::Finalize()
{
}

void Boss::Update()
{
  model_->Update();
}

void Boss::Draw()
{
  model_->Draw();
}

void Boss::DrawImGui()
{
}