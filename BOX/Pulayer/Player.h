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
	void Initialize(Model* model, uint32_t textureHandle);

	void Update();

	void Draw(ViewProjection viewProjection_);

	void b();

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
	Vector3 bulletOffset = {2.0f, 0.0f, 0.0f};
	bool bButtonReleased_ = true;
};
