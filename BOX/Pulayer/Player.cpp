
#include "BOX/Pulayer/Player.h"
#include <ImGuiManager.h>
#include <cassert>
Player::~Player() {
	for (BoxType* box : boxs_) {
		delete box;
	}
}

void Player::Initialize(Model* model, uint32_t textureHandle) {
	/*画像*/
	assert(model);
	this->model_ = model;
	this->textureHandle_ = textureHandle;

	worldTransform_.Initialize();
	input_ = Input::GetInstance();
}
void Player::b() {
	if (tim <= 0) {
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
			if (bButtonReleased_) {
			BoxType* box2_ = new BoxType;
			
			// 弾の位置を計算してオフセットを適用
			Vector3 bulletPosition = Add(worldTransform_.translation_, bulletOffset);

			box2_->Initialize(model_, bulletPosition);
			boxs_.push_back(box2_);
			bButtonReleased_ = false; // Bボタンが押されたことを記録
			tim = 20;
			}
		}
	} else {
		bButtonReleased_ = true; // Bボタンがリリースされたことを記録
	}
	
	}
}

void Player::Update() {
	/*画像*/
	worldTransform_.TransferMatrix();
	/*操作キー*/
	Vector3 move = {0, 0, 0};
	const float kCharacterSpeed = 0.2f;
	float inputFloat3[3] = {
	    worldTransform_.translation_.x, worldTransform_.translation_.y,
	    worldTransform_.translation_.z};
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)joyState.Gamepad.sThumbRY / SHRT_MAX * kCharacterSpeed;
	}
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}
	if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}
	worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 移動制限
	const float kMoveLimitX = 33;
	const float kMoveLimitY = 18;
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	/*弾*/
	b();
	tim--;
	Vector3 move2 = {0, 0, 0};
	const float kCharacterSpeed2 = 2.0f;
	move2.y += kCharacterSpeed2;

	for (BoxType* box : boxs_) {
		box->Update(move2);
	}

	// 回転
	const float kRotSpeed = 0.2f;
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {

		worldTransform_.rotation_.y += kRotSpeed;
	}
}

void Player::Draw(ViewProjection viewProjection_) {
	/*画像*/
	model_->Draw(this->worldTransform_, viewProjection_, this->textureHandle_);
	/*操作キー*/
	input_ = Input::GetInstance();
	/*弾*/
	for (BoxType* box : boxs_) {
		box->Draw(viewProjection_);
	}
}
