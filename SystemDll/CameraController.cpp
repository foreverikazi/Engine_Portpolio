#include "pch.h"
#include "CameraController.h"
#define _USE_MATH_DEFINES
#include <math.h>
EditorCameraController::EditorCameraController()
	: mUp(0.0f, 0.0f, 0.0f),
	mLook(0.0f, 0.0f, 0.0f),
	mPosition(0.0f, 0.0f, 0.0f),
	mRotation(0.0f, 0.0f, 0.0f)
{
}

EditorCameraController::~EditorCameraController()
{
}

void EditorCameraController::InitCamera(XMFLOAT3 up, XMFLOAT3 look, XMFLOAT3 position)
{
	mUp = up;
	mLook = look;
	mPosition = position;
}

void EditorCameraController::SetPosition(XMFLOAT3 position)
{
	mPosition = position;
}

void EditorCameraController::SetRotation(XMFLOAT3 rotation)
{
	mRotation = rotation;
}

XMFLOAT3 EditorCameraController::GetPosition()
{
	return mPosition;
}

XMFLOAT3 EditorCameraController::GetRotation()
{
	return mRotation;
}

void EditorCameraController::UpdateCamera()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// 위쪽을 가리키는 벡터를 설정합니다.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// XMVECTOR 구조체에 로드한다.
	upVector = XMLoadFloat3(&up);

	// 3D월드에서 카메라의 위치를 ​​설정합니다.
	position = mPosition;

	// XMVECTOR 구조체에 로드한다.
	positionVector = XMLoadFloat3(&position);

	// 기본적으로 카메라가 찾고있는 위치를 설정합니다.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// XMVECTOR 구조체에 로드한다.
	lookAtVector = XMLoadFloat3(&lookAt);

	// yaw (Y 축), pitch (X 축) 및 roll (Z 축)의 회전값을 라디안 단위로 설정합니다.
	pitch = mRotation.x * 0.0174532925f;
	yaw = mRotation.y * 0.0174532925f;
	roll = mRotation.z * 0.0174532925f;

	//  yaw, pitch, roll 값을 통해 회전 행렬을 만듭니다.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// lookAt 및 up 벡터를 회전 행렬로 변형하여 뷰가 원점에서 올바르게 회전되도록 합니다.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// 회전 된 카메라 위치를 뷰어 위치로 변환합니다.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// 마지막으로 세 개의 업데이트 된 벡터에서 뷰 행렬을 만듭니다.
	mViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void EditorCameraController::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = mViewMatrix;
}