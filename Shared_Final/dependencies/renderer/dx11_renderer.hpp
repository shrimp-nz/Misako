#pragma once
#include <cstdint>
#include <intrin.h>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_internal.h"

#include <environment/drawing/drawing.hpp>

typedef HRESULT(__stdcall* ID3D11Present)(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags);
typedef HRESULT(__stdcall* ID3D11ResizeBuffers)(IDXGISwapChain* SwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
typedef LRESULT(__stdcall* ID3D11WindowProcess)(HWND, UINT, WPARAM, LPARAM);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace drawing_renderer
{
	extern void hook();
}