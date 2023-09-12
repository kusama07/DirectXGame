#include "BOX/Pulayer/Player.h"
#include <ImGuiManager.h>
#include <cassert>

Player::~Player() {
	for (BoxType* box : boxs_) {
		delete box;
	}
}

void Player::Initialize(Model* model) {
	/*画像*/
	assert(model);
	this->model_ = model;

	worldTransform_.Initialize();
	input_ = Input::GetInstance();

}

void Player::Update() {

	/*画像*/
	worldTransform_.TransferMatrix();
	
	/*移動*/
	Move();

	/*弾*/
	b();
	tim--;

}

void Player::Draw(ViewProjection& viewProjection_) {
	/*画像*/
	model_->Draw(this->worldTransform_, viewProjection_);
	/*操作キー*/
	input_ = Input::GetInstance();
	/*弾*/
	for (BoxType* box : boxs_) {
		box->Draw(viewProjection_);
	}
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Player::GetAABB() {
	// プレイヤーのAABBの最小座標と最大座標を計算
	Vector3 min = {
	    worldTransform_.translation_.x - playerWidth_ / 2,
	    worldTransform_.translation_.y - playerHeight_ / 2,
	    worldTransform_.translation_.z - playerDepth_ / 2};
	Vector3 max = {
	    worldTransform_.translation_.x + playerWidth_ / 2,
	    worldTransform_.translation_.y + playerHeight_ / 2,
	    worldTransform_.translation_.z + playerDepth_ / 2};

	// 計算した最小座標と最大座標からAABBを構築
	AABB aabb;
	aabb.min = min;
	aabb.max = max;

	return aabb;
}

void Player::Move() {
	worldTransform_.TransferMatrix();

#pragma region キーボード操作

	Vector3 Keyboardmove = {0, 0, 0};

	if (input_->PushKey(DIK_LEFT)) {
		Keyboardmove.x -= kPlayerSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		Keyboardmove.x += kPlayerSpeed;
	}

#pragma region ジャンプ

	acce.y = -0.08f;

	if ((input_->TriggerKey(DIK_SPACE) || joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
	    worldTransform_.translation_.y == worldTransform_.scale_.y) {
		velocity.y = 1.0f;
	}

	velocity.y += acce.y;
	worldTransform_.translation_.y += velocity.y;

	if (worldTransform_.translation_.y <= worldTransform_.scale_.y) {
		worldTransform_.translation_.y = worldTransform_.scale_.y;
	}

#pragma endregion

#pragma region 回転


	if (input_->TriggerKey(DIK_LEFT)) {
		playerRotateLeftFlag = true;
	}

	if (input_->TriggerKey(DIK_RIGHT)) {
		playerRotateRightFlag = true;
	}

	// 左
	if (playerRotateLeftFlag == 1) {
		worldTransform_.rotation_.y = 2.5f;

		playerRotateLeftFlag = false;
	}

	// 右
	if (playerRotateRightFlag == 1) {
		worldTransform_.rotation_.y = 13.5f;

		playerRotateRightFlag = false;
	}

#pragma endregion

#pragma endregion

#pragma region コントローラ

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// 右スティックのX軸の値を取得
		float rightThumbstickX = (float)joyState.Gamepad.sThumbRX / SHRT_MAX;

		//// 右にスティックを倒した場合
		//if (rightThumbstickX > 0.5f) {
		//	playerRotateRightFlag = true;
		//	playerRotateLeftFlag = false; // 右に倒す場合、左のフラグをクリア
		//}
		//// 左にスティックを倒した場合
		//else if (rightThumbstickX < -0.5f) {
		//	playerRotateLeftFlag = true;
		//	playerRotateRightFlag = false; // 左に倒す場合、右のフラグをクリア
		//}
		//// スティックが十分に動かない場合、両方のフラグをクリア
		//else {
		//	playerRotateLeftFlag = false;
		//	playerRotateRightFlag = false;
		//}

		// スティックの値に基づいて移動を計算
		Vector3 move = {rightThumbstickX, 0.0f, 0.0f};
		move = Multiply(kPlayerSpeed, Normalize(move));

		worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	}

#pragma endregion

	worldTransform_.translation_ = Add(worldTransform_.translation_, Keyboardmove);
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.UpdateMatrix();

	// 移動制限
	const float kMoveLimitX = 33;
	const float kMoveLimitY = 18;
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);
}

void Player::b() {
	if (tim <= 0) {
		if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
			return;
		}
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
			red = true;
			blue = false;
			if (rLetGo == true) {
				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
					lLetGo = false;
					if (bButtonReleased_) {
						BoxType* box2_ = new BoxType;
						bulletOffset.x += 2.0f;
						// 弾の位置を計算してオフセットを適用
						Vector3 bulletPosition = Add(worldTransform_.translation_, bulletOffset);

						box2_->Initialize(model_, bulletPosition, redBox_);

						boxs_.push_back(box2_);
						bButtonReleased_ = false; // Bボタンが押されたことを記録
						tim = 20;
					}
				} else {
					bButtonReleased_ = true; // Bボタンがリリースされたことを記録
				}
			}
			if (lLetGo == true) {
				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
					rLetGo = false;
					if (bButtonReleased_) {
						BoxType* box2_ = new BoxType;
						bulletOffset.x -= 2.0f;
						// 弾の位置を計算してオフセットを適用
						Vector3 bulletPosition = Add(worldTransform_.translation_, bulletOffset);

						box2_->Initialize(model_, bulletPosition, redBox_);

						boxs_.push_back(box2_);
						bButtonReleased_ = false; // Bボタンが押されたことを記録
						tim = 20;
					}
				} else {
					bButtonReleased_ = true; // Bボタンがリリースされたことを記録
				}
			}

		} else {
			if (red == true) {

				lLetGo = true;
				rLetGo = true;
				bulletOffset.x = 0.0f;
				Vector3 move2 = {0, 0, 0};
				const float kCharacterSpeed2 = 2.0f;
				move2.y += kCharacterSpeed2;

				for (BoxType* box : boxs_) {
					box->Update(move2);
				}
			}
		}
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
			blue = true;
			red = false;
			if (rLetGo == true) {
				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
					lLetGo = false;
					if (bButtonReleased_) {
						BoxType* box2_ = new BoxType;
						bulletOffset.x += 2.0f;
						// 弾の位置を計算してオフセットを適用
						Vector3 bulletPosition = Add(worldTransform_.translation_, bulletOffset);

						box2_->Initialize(model_, bulletPosition, blueBox_);

						boxs_.push_back(box2_);
						bButtonReleased_ = false; // Bボタンが押されたことを記録
						tim = 20;
					}
				} else {
					bButtonReleased_ = true; // Bボタンがリリースされたことを記録
				}
			}
			if (lLetGo == true) {
				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
					rLetGo = false;
					if (bButtonReleased_) {
						BoxType* box2_ = new BoxType;
						bulletOffset.x -= 2.0f;
						// 弾の位置を計算してオフセットを適用
						Vector3 bulletPosition = Add(worldTransform_.translation_, bulletOffset);

						box2_->Initialize(model_, bulletPosition, blueBox_);

						boxs_.push_back(box2_);
						bButtonReleased_ = false; // Bボタンが押されたことを記録
						tim = 20;
					}
				} else {
					bButtonReleased_ = true; // Bボタンがリリースされたことを記録
				}
			}

		} else {
			if (blue == true) {

				lLetGo = true;
				rLetGo = true;
				bulletOffset.x = 0.0f;
				Vector3 move2 = {0, 0, 0};
				const float kCharacterSpeed2 = 2.0f;
				move2.x += kCharacterSpeed2;

				for (BoxType* box : boxs_) {
					box->Update(move2);
				}
			}
		}
	}
}

void Player::AdjustPositionOnBox(float newPosY) {
	worldTransform_.translation_.y = newPosY;
	// ここでジャンプの状態をリセットするかどうかの処理を追加
	velocity.y = 0.0f;
}

void Player::SetVelocity(const Vector3& newVelocity) { velocity = newVelocity; }