#include "vivid/vivid.h"
#include "vivid/renderer.h"
#include "vivid/light.h"
#include "vivid/material.h"
#include "vivid/world.h"

bool CheckInputs();
int fillMode = 0;
Mesh* mesh;
Transform camera;
Transform lightTransform;

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE prevInstance,
				   PSTR cmdLine,
				   int showCmd)
{
	vvd::OpenLog("VividApp.log");
	if(!vvd::Init(hInstance, 1024, 768, "VividApp", false, D3DFMT_A8R8G8B8, D3DPRESENT_INTERVAL_IMMEDIATE)) {
		exit(1);
	}
	vvd::Log("VividApp is up and running!");

	Renderer renderer;

	ImageFilter filter("filter.fx");
	renderer.EnableFilter(&filter);

	Renderer shadowRenderer;

	camera.SetPosition(30.0f, 40.0f, -20.0f);

	World world(1000.0f, 1000.0f, 1000.0f, 1000.0f, 1000.0f, 1000.0f);

	D3DXVECTOR3 pos(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 ambientColor(0.0f, 0.0f, 0.0f);
	world.SetAmbientColor(pos, ambientColor);

	Light light;
	light.SetRange(500.0f);
	light.SetColor(1.0f, .75f, 0.5f);
	lightTransform.SetPosition(-100.0f, 50.0f, 100.0f);

	Light light2;
	light2.SetPosition(100.0f, 50.0f, 100.0f, 1.0f);
	light2.SetRange(500.0f);
	light2.SetColor(0.5f, 0.75f, 1.0f);

	Light light3;
	light3.SetPosition(75.0f, 175.0f, 200.0f, 1.0f);
	light3.SetRange(500.0f);
	light3.SetColor(1.0f, 1.0f, 1.0f);

	mesh = new Mesh("droid.x");
	mesh->transform.SetPosition(0.0f, 24.0f, 50.0f);
	mesh->transform.SetRotation(D3DXToRadian(-90), 0.0f, 0.0f);
	

	Mesh mesh2("plane.x");
	mesh2.transform.SetPosition(-100.0f, 50.0f, 50.0f);
	mesh2.transform.SetRotation(0.0f, D3DXToRadian(-45), 0.0f);
	mesh2.transform.SetScale(10.0f, 10.0f, 10.0f);

	Mesh mesh3("building.x");

	vvd::SetMinKeyPressTime(DIK_F, 0.25f);

	while(true) {
		vvd::Update();

		if(!CheckInputs())
			break;

		renderer.SetCameraPosition(&camera.GetPosition());
		renderer.SetCameraRotation(&camera.GetLookVector(), &camera.GetUpVector());

		light.SetPosition(lightTransform.GetPosition().x, lightTransform.GetPosition().y, lightTransform.GetPosition().z, 1.0f);

		switch(fillMode) {
		case 0:
			renderer.SetFillMode(D3DFILL_SOLID);
			break;
		case 1:
			renderer.SetFillMode(D3DFILL_WIREFRAME);
			break;
		case 2:
			renderer.SetFillMode(D3DFILL_POINT);
			break;
		}

		world.Update();
		shadowRenderer.DrawShadows();
		renderer.Draw();

		MSG msg;
		if(!::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) {
				vvd::Log("Recieved quit message from windows; exiting message loop...");
				break;
			}
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	vvd::DeInit();
	return 0;
}
bool CheckInputs() {
	if(vvd::keyDown(DIK_ESCAPE)) {
		vvd::Log("User pressed escape; exiting message loop...");
		return false;
	}
	if(vvd::keyDown(DIK_F)) {
		vvd::Log("Switched fill mode");
		fillMode++;
		if(fillMode > 2) {
			fillMode = 0;
		}
	}

	float speed = 75.0f;
	if(vvd::mouseButtonDown(0)) {
		if(vvd::keyDown(DIK_W))
			mesh->transform.AddPositionRelative(speed * vvd::GetDelta(), 0.0f, 0.0f);
		if(vvd::keyDown(DIK_A))
			mesh->transform.AddPositionRelative(0.0f, -speed * vvd::GetDelta(), 0.0f);
		if(vvd::keyDown(DIK_S))
			mesh->transform.AddPositionRelative(-speed * vvd::GetDelta(), 0.0f, 0.0f);
		if(vvd::keyDown(DIK_D))
			mesh->transform.AddPositionRelative(0.0f, speed * vvd::GetDelta(), 0.0f);
		if(vvd::keyDown(DIK_SPACE))
			mesh->transform.AddPositionRelative(0.0f, 0.0f, speed * vvd::GetDelta());
		if(vvd::keyDown(DIK_LCONTROL))
			mesh->transform.AddPositionRelative(0.0f, 0.0f, -speed * vvd::GetDelta());
		mesh->transform.AddRotation(0.0f, vvd::mouseDX() * 0.004f, 0.0f);
	} else if(vvd::mouseButtonDown(1)) {
		if(vvd::keyDown(DIK_W))
			lightTransform.AddPositionRelative(0.0f, 0.0f, speed * vvd::GetDelta());
		if(vvd::keyDown(DIK_A))
			lightTransform.AddPositionRelative(-speed * vvd::GetDelta(), 0.0f, 0.0f);
		if(vvd::keyDown(DIK_S))
			lightTransform.AddPositionRelative(0.0f, 0.0f, -speed * vvd::GetDelta());
		if(vvd::keyDown(DIK_D))
			lightTransform.AddPositionRelative(speed * vvd::GetDelta(), 0.0f, 0.0f);
		if(vvd::keyDown(DIK_SPACE))
			lightTransform.AddPositionRelative(0.0f, speed * vvd::GetDelta(), 0.0f);
		if(vvd::keyDown(DIK_LCONTROL))
			lightTransform.AddPositionRelative(0.0f, -speed * vvd::GetDelta(), 0.0f);
	} else if(vvd::keyDown(DIK_C)) {
		camera.SetPosition(&mesh->transform.GetPosition());
		D3DXVECTOR3 offset(0.0f, 0.0f, mesh->GetRadius() * -1.5f);
		camera.AddRotation(0.0f, 0.5f * vvd::GetDelta(), 0.0f);
		D3DXMATRIX matrix = camera.GetRotationMatrix();
		D3DXVec3TransformCoord(&offset, &offset, &matrix);
		camera.AddPosition(&offset);
	} else {
		if(vvd::keyDown(DIK_W))
			camera.AddPositionRelative(0.0f, 0.0f, speed * vvd::GetDelta());
		if(vvd::keyDown(DIK_A))
			camera.AddPositionRelative(-speed * vvd::GetDelta(), 0.0f, 0.0f);
		if(vvd::keyDown(DIK_S))
			camera.AddPositionRelative(0.0f, 0.0f, -speed * vvd::GetDelta());
		if(vvd::keyDown(DIK_D))
			camera.AddPositionRelative(speed * vvd::GetDelta(), 0.0f, 0.0f);
		if(vvd::keyDown(DIK_SPACE))
			camera.AddPositionRelative(0.0f, speed * vvd::GetDelta(), 0.0f);
		if(vvd::keyDown(DIK_LCONTROL))
			camera.AddPositionRelative(0.0f, -speed * vvd::GetDelta(), 0.0f);
		camera.AddRotation(vvd::mouseDY() * 0.004f, vvd::mouseDX() * 0.004f, 0.0f);
	}

	return true;
}