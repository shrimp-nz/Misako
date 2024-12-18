#include "dx11_renderer.hpp"
#include "fontcustom_data.hpp"

#include <mutex>
#include <filesystem>
#include <dependencies/xorstring/xorstring.hpp>

bool Initialize;

DXGI_SWAP_CHAIN_DESC D11SwapChain;
IDXGISwapChain* D11GSwapChain = nullptr;

ID3D11Present D11Present = nullptr;
ID3D11ResizeBuffers D11Resize = nullptr;
ID3D11WindowProcess D11WindowProcess = nullptr;

ID3D11Device* D11Device = nullptr;
ID3D11DeviceContext* D11Context = nullptr;
ID3D11RenderTargetView* D11RenderTargetView;

ID3D11Texture2D* D11TextureBuffer = nullptr;
ImFont* Consolas;

int WindowWidth;
int WindowHeight;
int WindowTab = 1;

HWND Window = nullptr;
RECT WindowSize;

namespace font_mapper
{
	// doing this in a sussy X way
	std::vector<ImFont*> font;

	auto push_font(ImFont* idx) -> void { font.emplace_back(idx); }
	auto return_font(std::int32_t idx) -> ImFont* { return font.at(idx); }
}

LRESULT __stdcall WindowProcess(const HWND WindowProcess, UINT Message, WPARAM W_Param, LPARAM L_Param)
{
	if (true && ImGui_ImplWin32_WndProcHandler(WindowProcess, Message, W_Param, L_Param))
		return true;

	return CallWindowProc(D11WindowProcess, WindowProcess, Message, W_Param, L_Param);
}

HRESULT __stdcall PresentHook(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags)
{
	if (!Initialize)
	{
		if (SUCCEEDED(D11GSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&D11Device))))
		{
			swap_chain->GetDesc(&D11SwapChain);
			swap_chain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&D11Device));
			swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&D11TextureBuffer));

			Window = D11SwapChain.OutputWindow;
			D11WindowProcess = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(Window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcess)));

			D11Device->GetImmediateContext(&D11Context);
			D11Device->CreateRenderTargetView(D11TextureBuffer, nullptr, &D11RenderTargetView); D11TextureBuffer->Release();

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();

			ImFontConfig font_config;
			font_config.FontDataOwnedByAtlas = false;

			font_mapper::push_font(io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 20.0f, &font_config));
			font_mapper::push_font(io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 20.0f, &font_config));
			font_mapper::push_font(io.Fonts->AddFontDefault(&font_config));

			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
			io.IniFilename = NULL;

			ImGui_ImplWin32_Init(Window);
			ImGui_ImplDX11_Init(D11Device, D11Context);

			Initialize = true;
		}

		return D11Present(swap_chain, sync_interval, flags);
	}

	if (D11RenderTargetView == nullptr)
	{
		swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&D11TextureBuffer));
		D11Device->CreateRenderTargetView(D11TextureBuffer, nullptr, &D11RenderTargetView);

		D11TextureBuffer->Release();
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	const auto render_list = ImGui::GetBackgroundDrawList();

	for (const auto shape : drawing_vector)
	{
		if (shape->type == line)
		{
			const auto line = reinterpret_cast<line_t*>(shape);

			if (line->visible)
				render_list->AddLine({ line->from.x, line->from.y }, { line->to.x, line->to.y }, ImColor(line->color.x, line->color.y, line->color.z, line->transparency), line->thickness);
		}
		if (shape->type == circle)
		{
			const auto circle = reinterpret_cast<circle_t*>(shape);

			if (circle->visible)
			{
				if (circle->filled)
					render_list->AddCircleFilled({ circle->position.x, circle->position.y }, circle->radius, ImColor(circle->color.x, circle->color.y, circle->color.z, circle->transparency), circle->numsides);
				else
					render_list->AddCircle({ circle->position.x, circle->position.y }, circle->radius, ImColor(circle->color.x, circle->color.y, circle->color.z, circle->transparency), circle->numsides, circle->thickness);
			}
		}
		if (shape->type == square)
		{
			const auto square = reinterpret_cast<square_t*>(shape);

			if (square->visible)
			{
				if (square->filled)
					render_list->AddRectFilled({ square->position.x, square->position.y }, { square->position.x + square->size.x, square->position.y + square->size.y }, ImColor(square->color.x, square->color.y, square->color.z, square->transparency));
				else
					render_list->AddRect({ square->position.x, square->position.y }, { square->position.x + square->size.x, square->position.y + square->size.y }, ImColor(square->color.x, square->color.y, square->color.z, square->transparency), 0, 0, square->thickness);
			}
		}
		if (shape->type == triangle)
		{
			const auto triangle = reinterpret_cast<triangle_t*>(shape);

			if (triangle->visible)
			{
				if (triangle->filled)
					render_list->AddTriangleFilled({ triangle->pointa.x, triangle->pointa.y }, { triangle->pointb.x, triangle->pointb.y }, { triangle->pointc.x, triangle->pointc.y }, ImColor(triangle->color.x, triangle->color.y, triangle->color.z, triangle->transparency));
				else
					render_list->AddTriangle({ triangle->pointa.x, triangle->pointa.y }, { triangle->pointb.x, triangle->pointb.y }, { triangle->pointc.x, triangle->pointc.y }, ImColor(triangle->color.x, triangle->color.y, triangle->color.z, triangle->transparency), triangle->thickness);
			}
		}
		if (shape->type == quad)
		{
			const auto quad = reinterpret_cast<quad_t*>(shape);

			if (quad->visible)
			{
				if (quad->filled)
					render_list->AddQuadFilled({ quad->pointa.x, quad->pointa.y }, { quad->pointb.x, quad->pointb.y }, { quad->pointc.x, quad->pointc.y }, { quad->pointd.x, quad->pointd.y }, ImColor(quad->color.x, quad->color.y, quad->color.z, quad->transparency));
				else
					render_list->AddQuad({ quad->pointa.x, quad->pointa.y }, { quad->pointb.x, quad->pointb.y }, { quad->pointc.x, quad->pointc.y }, { quad->pointd.x, quad->pointd.y }, ImColor(quad->color.x, quad->color.y, quad->color.z, quad->transparency), quad->thickness);
			}
		}
		if (shape->type == text)
		{
			const auto text = reinterpret_cast<text_t*>(shape);

			if (text->visible)
			{
				auto size = 0;
				const auto TextSize = ImGui::CalcTextSize(text->text, nullptr, true);

				if (text->center)
				{
					if (text->outline)
					{
						// add text->font support to the renderer
						render_list->AddText(font_mapper::return_font(text->font), text->size, ImVec2(text->position.x - TextSize.x / 2.0f + 1, text->position.y + TextSize.y * size + 1), ImColor(ImVec4(0, 0, 0, text->transparency)), text->text);
						render_list->AddText(font_mapper::return_font(text->font), text->size, ImVec2(text->position.x - TextSize.x / 2.0f - 1, text->position.y + TextSize.y * size - 1), ImColor(ImVec4(0, 0, 0, text->transparency)), text->text);
						render_list->AddText(font_mapper::return_font(text->font), text->size, ImVec2(text->position.x - TextSize.x / 2.0f + 1, text->position.y + TextSize.y * size - 1), ImColor(ImVec4(0, 0, 0, text->transparency)), text->text);
						render_list->AddText(font_mapper::return_font(text->font), text->size, ImVec2(text->position.x - TextSize.x / 2.0f - 1, text->position.y + TextSize.y * size + 1), ImColor(ImVec4(0, 0, 0, text->transparency)), text->text);
					}

					render_list->AddText(font_mapper::return_font(text->font), text->size, ImVec2(text->position.x - TextSize.x / 2.0f, text->position.y), ImColor(text->color.x, text->color.y, text->color.z, text->transparency), text->text);
				}
				else
				{
					if (text->outline)
					{
						render_list->AddText(font_mapper::return_font(text->font), text->size, ImVec2(text->position.x / 1.1f + 16, text->position.y + 1), ImColor(ImVec4(0, 0, 0, text->transparency)), text->text);
						render_list->AddText(font_mapper::return_font(text->font), text->size, ImVec2(text->position.x / 1.1f + 14, text->position.y - 1), ImColor(ImVec4(0, 0, 0, text->transparency)), text->text);
						render_list->AddText(font_mapper::return_font(text->font), text->size, ImVec2(text->position.x / 1.1f + 16, text->position.y - 1), ImColor(ImVec4(0, 0, 0, text->transparency)), text->text);
						render_list->AddText(font_mapper::return_font(text->font), text->size, ImVec2(text->position.x / 1.1f + 14, text->position.y + 1), ImColor(ImVec4(0, 0, 0, text->transparency)), text->text);
					}

					render_list->AddText(font_mapper::return_font(text->font), text->size, ImVec2(text->position.x / 1.1f + 15, text->position.y), ImColor(text->color.x, text->color.y, text->color.z, text->transparency), text->text);
				}

				size++;
			}
		}
		if (shape->type == image) // PLEASE IDK HOW THIS WORKS :sob: -Rexi
		{
			const auto image = reinterpret_cast<image_t*>(shape);

			if (image->visible)
			{
				auto size = 0;
				const auto TextSize = ImGui::CalcTextSize(image->data, nullptr, true);

				render_list->AddText(font_mapper::return_font(1), 16, ImVec2(image->position.x / 1.1f + 15, image->position.y), ImColor(image->color.x, image->color.y, image->color.z, image->transparency), image->data);
			}
		}
	}

	ImGui::EndFrame();
	ImGui::Render();

	D11Context->OMSetRenderTargets(1, &D11RenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return D11Present(swap_chain, sync_interval, flags);
}

HRESULT __stdcall ResizeBuffersHook(IDXGISwapChain* this_ptr, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags)
{
	if (D11RenderTargetView)
	{
		D11RenderTargetView->Release();
		D11RenderTargetView = nullptr;
	}

	Window = D11SwapChain.OutputWindow;

	GetWindowRect(Window, &WindowSize);
	GetClientRect(Window, &WindowSize);

	WindowWidth = WindowSize.right - WindowSize.left;
	WindowHeight = WindowSize.bottom - WindowSize.top;

	return D11Resize(this_ptr, buffer_count, width, height, new_format, swap_chain_flags);
}

void drawing_renderer::hook()
{
	Window = FindWindowW(0, xorstr(L"Roblox"));

	GetWindowRect(Window, &WindowSize);
	GetClientRect(Window, &WindowSize);

	WindowWidth = WindowSize.right - WindowSize.left;
	WindowHeight = WindowSize.bottom - WindowSize.top;

	D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
	D3D_FEATURE_LEVEL obtained_level;

	DXGI_SWAP_CHAIN_DESC SwapChain;
	ZeroMemory(&SwapChain, sizeof(SwapChain));

	SwapChain.BufferCount = 1;
	SwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	SwapChain.OutputWindow = Window;
	SwapChain.SampleDesc.Count = 1;
	SwapChain.Windowed = ((GetWindowLongPtrA(Window, GWL_STYLE) & WS_POPUP) != 0) ? false : true;
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChain.BufferDesc.Width = 1;
	SwapChain.BufferDesc.Height = 1;
	SwapChain.BufferDesc.RefreshRate.Numerator = 0;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, levels, sizeof(levels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &SwapChain, &D11GSwapChain, &D11Device, &obtained_level, &D11Context);

	std::uintptr_t* vt_swapchain;
	std::memcpy(&vt_swapchain, reinterpret_cast<LPVOID>(D11GSwapChain), sizeof(std::uintptr_t));

	// callcheck createeventa method
	typedef BOOL(WINAPI* VirtualProtectFunction)(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
	VirtualProtectFunction vt_protect = reinterpret_cast<VirtualProtectFunction>(reinterpret_cast<void*>(GetProcAddress(GetModuleHandleW(xorstr(L"kernelbase.dll")), xorstr("VirtualProtect")))); // make sure to xorstring

	DWORD old_protection;
	vt_protect(vt_swapchain, sizeof(std::uintptr_t), PAGE_EXECUTE_READWRITE, &old_protection);

	D11Present = reinterpret_cast<decltype(D11Present)>(vt_swapchain[8]);
	D11Resize = reinterpret_cast<decltype(D11Resize)>(vt_swapchain[13]);

	vt_swapchain[8] = reinterpret_cast<std::uintptr_t>(&PresentHook);
	vt_swapchain[13] = reinterpret_cast<std::uintptr_t>(&ResizeBuffersHook);

	vt_protect(vt_swapchain, sizeof(std::uintptr_t), old_protection, &old_protection);
}