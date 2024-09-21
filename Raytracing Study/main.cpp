// 해당 프로젝트는 홍정모의 컴퓨터 그래픽스 새싹코스의 Introduction to Computer Graphics with DirectX 11 - Part 1. Basic Concepts of Rendering의 내용을 바탕으로 구현한 프로젝트입니다. 

#define GLM_ENABLE_EXPERIMENTAL
#include <windows.h>
#include <memory>
#include <iostream>
#include "Engine.h"

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main() {
	const int width = 1280, height = 720;

	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0L,
		0L,
		GetModuleHandle(NULL),
		NULL,
		NULL,
		NULL,
		NULL,
		L"Raytracing",
		NULL 
	};

	RegisterClassEx(&wc);

	RECT wr = { 0, 0, width, height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hwnd = CreateWindow(
		wc.lpszClassName,
		L"Raytracing Study",
		WS_OVERLAPPEDWINDOW,
		100, 100,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL, NULL,
		wc.hInstance,
		NULL
	);

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	auto engine = std::make_unique<Engine>(hwnd, width, height);

	MSG msg = {};
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			engine->Update();
			engine->Render();

			engine->swapChain->Present(1, 0);
		}
	}

	engine->Clean();
	DestroyWindow(hwnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}