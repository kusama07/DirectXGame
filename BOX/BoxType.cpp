#include "BoxType.h"


void BoxType::Initialize(Model* model,  Vector3& position) {
	model_ = model;
	redBox_ = TextureManager::Load("red.png");
	worldTransform_.Initialize();
	worldTransform_.translation_.x = 64;
	worldTransform_.translation_ = position;
	
}

void BoxType::Update(Vector3 move) { 
	worldTransform_.UpdateMatrix();

	worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	

}

void BoxType::Draw(const ViewProjection& viewProjection_) {

	model_->Draw(worldTransform_, viewProjection_, redBox_); 

}
