#pragma once
#include "Input.h"
#include <Model.h>
#include "BOX/BoxType.h"
#include"BOX/math/math.h"
#include <WorldTransform.h>
#include <list>

class Player {
public:
	~Player();
	void Initialize(Model* model);

	void Update();

	void Draw(ViewProjection& viewProjection_);

	void b();

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	AABB GetAABB();

	void Move();

	float GetPlayerHeight() const { return playerHeight_; };

	void SetVelocity(const Vector3& newVelocity);

	void AdjustPositionOnBox(float newPosY);

private:
	/*画像*/
	WorldTransform worldTransform_;

	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	/*操作キー*/
	Input* input_ = nullptr;
	//
	BoxType* box_ = nullptr; 
	std::list<BoxType*> boxs_;
	//
	XINPUT_STATE joyState;
	int32_t tim = 20;
	Vector3 bulletOffset = {0.0f, 0.0f, 0.0f};
	bool bButtonReleased_ = true;
	bool lLetGo = true;
	bool rLetGo = true;
	bool red = true;
	bool blue = true;
	uint32_t redBox_ = TextureManager::Load("red.png");
	uint32_t blueBox_ = TextureManager::Load("blue.png");

	//移動
	Vector3 acce = {0.0f, -0.8f, 0.0f};
	Vector3 velocity = {0.0f, 0.0f, 0.0f};

	bool playerRotateLeftFlag = false;
	bool playerRotateRightFlag = false;

	float playerWidth_ = 2.0f;
	float playerHeight_ = 2.0f;
	float playerDepth_ = 2.0f;
	float kPlayerSpeed = 0.2f;
};
