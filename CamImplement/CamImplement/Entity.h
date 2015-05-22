//
//
//

#ifndef ENTITY__H
#define ENTITY__H

#include <Windows.h>
#include <DirectXCollision.h>
#include <DirectXMath.h>

#include "LQueue.h"

#define KEYDOWN(vkey)	(GetAsyncKeyState(vkey) & 0x8000)

namespace Collision
{
	enum Action
	{
		Attack1,
		Attack2,
		Block,
		Dodge,
		MoveUp,
		MoveDown,
		MoveRight,
		MoveLeft,
		Idle
	};

	class Entity
	{
	public:
		Entity();
		virtual ~Entity();

		virtual HRESULT Update(float deltaTime);

		// Positional
		DirectX::XMMATRIX GetTransform();
		DirectX::XMVECTOR GetPosition();
		void MoveTo(DirectX::XMVECTOR target);//
		void SetPosition(DirectX::XMVECTOR position);//

		// Collision
		DirectX::ContainmentType Intersect(Entity *entity);
		void CollisionHit(Entity *entity);
		void Push(DirectX::XMVECTOR force);

		// Combat
		void SetMovementSpeed(float speed);
		void PerformAction(Action action);
		Action GetCurrentAction();
		virtual void Attack() = 0;

	protected:
		
		// Movement data.
		DirectX::XMVECTOR m_Position;
		DirectX::XMVECTOR m_Rotation;
		DirectX::XMVECTOR m_Move = DirectX::XMVectorZero();
		DirectX::XMVECTOR m_TargetLocation;

		// Combat data.
		float m_AttackRange = 1.f;
		float m_HitPoints = 100.f;
		float m_Speed = 1.f;
		Action m_CurrentAction = Idle;
		int m_currentActionFrame = 0;

	private:
		float m_Mass = 1.f;
		float m_Radius = 1.f;
	};

	class Player : public Entity
	{
	public:
		Player(DirectX::XMVECTOR position,
			DirectX::XMVECTOR rotation);
		virtual ~Player();

		HRESULT Update(float deltaTime) override;

		void Attack() override;

		void SetAttackDirection(POINT clientCursorNDC);
		void SetInputKey(Action action, int key);

		float GetHitPoints();

	private:
		int m_Controls[8];

	};

	class Enemy : public Entity
	{
	public:
		Enemy(float x, float z);
		Enemy(DirectX::XMFLOAT3 position);

		virtual ~Enemy();

		void enqueueAction(Action action);
		Action dequeueAction();

		HRESULT Update(float deltaTime);
		void Attack() override;

	private:
		LQueue<Action> orders;
	};
	

} /// namespace Collision


#endif /// ENTITY__H