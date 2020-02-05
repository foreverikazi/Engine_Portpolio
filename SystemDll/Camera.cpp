#include "pch.h"
#include "Camera.h"
#include "Input.h"
#define _USE_MATH_DEFINES
#include <math.h>

Camera::Camera()
	: mUp(0.0f, 1.0f, 0.0f),
	mLook(0.0f, 0.0f, 1.0f),
	mPosition(0.0f, 0.0f, 0.0f),
	mRotation(0.0f, 0.0f, 0.0f)
{
}

Camera::~Camera()
{
}

void Camera::InitCamera(XMFLOAT3 up, XMFLOAT3 look, XMFLOAT3 position)
{
	mUp = up;
	mLook = look;
	mPosition = position;
}

void Camera::SetPosition(XMFLOAT3 position)
{
	mPosition = position;
}

void Camera::SetRotation(XMFLOAT3 rotation)
{
	mRotation = rotation;
}

XMFLOAT3 Camera::GetPosition()
{
	return mPosition;
}

XMFLOAT3 Camera::GetRotation()
{
	return mRotation;
}

void Camera::UpdateCamera()
{
	CheckKeyboardInput();
	UpdateViewMatrix();
}

void Camera::CheckKeyboardInput()
{
	Input* input = (*Input::GetInst());

	/*if (input->GetKeyBoardState(DIK_UP))
	{
		XMFLOAT3 dir = XMFLOAT3(mLook.x - mPosition.x, mLook.y - mPosition.y, mLook.z - mPosition.z);
	}*/
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR upVector, positionVector, lookAtVector;
	XMMATRIX rotationMatrix;
	float yaw, pitch, roll;

	upVector = XMLoadFloat3(&mUp);
	positionVector = XMLoadFloat3(&mPosition);
	lookAtVector = XMLoadFloat3(&mLook);

	// angle to radian
	pitch = mRotation.x * 0.0174532925f;
	yaw = mRotation.y * 0.0174532925f;
	roll = mRotation.z * 0.0174532925f;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	mViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = mViewMatrix;
}