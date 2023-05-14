// most of the directx and imgui hooking was NOT by me.

#include "main.hpp"
#include "MD5.h"
#include "SHA1.hpp"
#include "SHA256.h"
#include "sha512.h"
#include "TextEditor.h"

#pragma warning(disable : 4996)

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    WNDCLASSEXW wc;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = nullptr;
    wc.hIcon = LoadIcon(0, IDI_WARNING);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = L"qHash";
    wc.lpszClassName = L"Class01";
    wc.hIconSm = LoadIcon(0, IDI_WARNING);

    RegisterClassExW(&wc);
    hwnd = CreateWindowExW(NULL, wc.lpszClassName, L"qHash Window", WS_POPUP, (GetSystemMetrics(SM_CXSCREEN) / 2) - (gui::size.x / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (gui::size.y / 2), gui::size.x, gui::size.y, 0, 0, 0, 0);

    SetWindowLongA(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    POINT mouse;
    rc = { 0 };
    GetWindowRect(hwnd, &rc);

    RenderBlur(hwnd);

    // initialize direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    TextEditor editor;
    editor.SetHandleKeyboardInputs(true);
    editor.SetReadOnly(FALSE);
    editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());

    {
        std::ifstream t(customHashFile);
        if (t.good())
        {
            std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            editor.SetText(str);
        }
    }

    // setup context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.IniFilename = NULL; // remove imgui.ini

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 5;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.ChildRounding = 5;
    style.FrameRounding = 5;
    style.ItemSpacing = ImVec2(5.0f, 5.0f);
    style.ScrollbarSize = 13.f;
    style.ScrollbarRounding = 1;
    style.GrabMinSize = 8.0;
    style.GrabRounding = 3;
    style.WindowPadding = ImVec2(5.0f, 5.0f);
    style.FramePadding = ImVec2(2.5f, 3.5f);
    style.ButtonTextAlign = ImVec2(0.5f, 0.6f);
    style.WindowBorderSize = 0;
    style.FrameBorderSize = 0;

    style.Colors[ImGuiCol_WindowBg]                 = ImVec4(0.06f, 0.06f, 0.06f, 0.75f);
    style.Colors[ImGuiCol_ChildBg]          	    = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    style.Colors[ImGuiCol_Text]                     = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]             = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_PopupBg]                  = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    style.Colors[ImGuiCol_Border]                   = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]                  = ImVec4(0.12f, 0.12f, 0.12f, 0.94f);
    style.Colors[ImGuiCol_FrameBgHovered]           = ImVec4(0.45f, 0.45f, 0.45f, 0.85f);
    style.Colors[ImGuiCol_FrameBgActive]            = ImVec4(0.63f, 0.63f, 0.63f, 0.63f);
    style.Colors[ImGuiCol_TitleBg]                  = ImVec4(0.13f, 0.13f, 0.13f, 0.99f);
    style.Colors[ImGuiCol_TitleBgActive]            = ImVec4(0.13f, 0.13f, 0.13f, 0.99f);
    style.Colors[ImGuiCol_TitleBgCollapsed]         = ImVec4(0.05f, 0.05f, 0.05f, 0.79f);
    style.Colors[ImGuiCol_MenuBarBg]                = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]              = ImVec4(0.13f, 0.13f, 0.13f, 0.99f);
    style.Colors[ImGuiCol_ScrollbarGrab]            = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]     = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]      = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]               = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]         = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_Button]                   = ImVec4(0.12f, 0.12f, 0.12f, 0.94f);
    style.Colors[ImGuiCol_ButtonHovered]            = ImVec4(0.34f, 0.34f, 0.35f, 0.89f);
    style.Colors[ImGuiCol_ButtonActive]             = ImVec4(0.21f, 0.21f, 0.21f, 0.81f);
    style.Colors[ImGuiCol_Header]                   = ImVec4(0.12f, 0.12f, 0.12f, 0.94f);
    style.Colors[ImGuiCol_HeaderHovered]            = ImVec4(0.34f, 0.34f, 0.35f, 0.89f);
    style.Colors[ImGuiCol_HeaderActive]             = ImVec4(0.12f, 0.12f, 0.12f, 0.94f);
    style.Colors[ImGuiCol_Separator]                = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_SeparatorHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]               = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    style.Colors[ImGuiCol_ResizeGripHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_PlotLines]                = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]         = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]            = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]     = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]           = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);

    // setup platform/renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    AddCustomDefaultFont(nullptr);

    // merge in icons from Font Awesome
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
    ImFont* FontAwesome = io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 14.f, &icons_config, icons_ranges);
    ImFont* FontAwesomeBig = io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 19.f, nullptr, icons_ranges);

    ImFont* TitleFont = io.Fonts->AddFontFromMemoryCompressedTTF(trebucbd_compressed_data, trebucbd_compressed_size, 30, nullptr, io.Fonts->GetGlyphRangesDefault());

    // loop
    bool loaderOpen = true;
    while (loaderOpen)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                loaderOpen = false;
        }
        if (!loaderOpen)
            break;

        // imgui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        auto cpos = editor.GetCursorPosition();
        ImGui::NewFrame();
        {
            ImGuiWindowFlags main_window_flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

            ImGui::SetNextWindowPos({ 0, 0 });
            ImGui::SetNextWindowSize(gui::size);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(((gui::title_height - ImGui::CalcTextSize(GUI_TITLE).y) / 2), ((gui::title_height - ImGui::CalcTextSize(GUI_TITLE).y) / 2)));
            ImGui::Begin(GUI_TITLE, &loaderOpen, main_window_flags);
            ImGui::PopStyleVar();
            {
                auto BackgroundDrawList = ImGui::GetBackgroundDrawList();
                auto ForegroundDrawList = ImGui::GetForegroundDrawList();
                auto WindowDrawList = ImGui::GetWindowDrawList();
                auto WindowSize = ImGui::GetWindowSize();
                auto WindowPos = ImGui::GetWindowPos();
                auto CursorPos = ImGui::GetCursorPos();
                //BackgroundDrawList->AddImageRounded((void*)bg_texture, ImVec2(WindowPos.x, WindowPos.y + gui::title_height), ImVec2(WindowPos.x + gui::size.x, WindowPos.y + gui::size.y), ImVec2(0, 0), ImVec2(1, 1), ImU32(IM_COL32(225, 225, 225, 225)), ImGui::GetStyle().WindowRounding*(4/3), ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersBottomRight);
                //BackgroundDrawList->AddRectFilled(WindowPos, ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + gui::title_height), IM_COL32(13, 13, 12, 255), style.WindowRounding, ImDrawFlags_RoundCornersTop); // Title
                //BackgroundDrawList->AddRectFilled(ImVec2(WindowPos.x, WindowPos.y + gui::title_height), ImVec2(WindowPos.x + gui::sidebar_width, WindowPos.y + WindowSize.y), IM_COL32(13, 13, 12, 255), style.WindowRounding, ImDrawFlags_RoundCornersBottomLeft); // sidebar
                BackgroundDrawList->AddRectFilled(WindowPos, ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + WindowSize.y), IM_COL32(13, 13, 12, 255 / 2), style.WindowRounding); // MainBg
                BackgroundDrawList->AddRectFilled(ImVec2(WindowPos.x + gui::sidebar_width, WindowPos.y + gui::title_height), ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + WindowSize.y), IM_COL32(25, 24, 24, 255), style.WindowRounding, ImDrawFlags_RoundCornersBottomRight | ImDrawFlags_RoundCornersTopLeft); // Main
                //BackgroundDrawList->AddRectFilled(ImVec2(WindowPos.x + 50, WindowPos.y + 200), ImVec2(WindowPos.x + (WindowSize.x-6), WindowPos.y + (WindowSize.y-15)), IM_COL32(31, 31, 31, 255), style.WindowRounding);

                ImGui::SetCursorPos(ImVec2(WindowPos.x + style.ItemSpacing.x, WindowPos.y + style.ItemSpacing.y));
                {
                    ImGui::PushFont(TitleFont); 
                    ImGui::Text(" " GUI_TITLE);
                    ImGui::PopFont();
                    ImGui::SameLine(WindowSize.x - (gui::title_height - style.WindowPadding.x));
                    {
                        ImGui::PushFont(FontAwesomeBig);
                        if (ImGui::Button(ICON_FA_XMARK, ImVec2(gui::title_height - style.WindowPadding.x * 2, gui::title_height - style.WindowPadding.x * 2)))
                            loaderOpen = false;
                        ImGui::PopFont();
                    }
                }
                ImGui::SetCursorPosY(gui::title_height + style.ItemSpacing.y);
                {
                    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 0.f);
                    ImGui::BeginChild("sidebar", ImVec2(gui::sidebar_width - style.WindowPadding.x * 2, 0), false, ImGuiWindowFlags_NoBackground);
                    {
                        auto nav_btn_size = ImVec2(gui::sidebar_width - style.WindowPadding.x * 2, gui::sidebar_width - style.WindowPadding.x * 2);
                        ImAdd::NavigationRadio(ICON_FA_SHIELD_HALVED, nav_btn_size, 0, &nav_num);
                        ImAdd::NavigationRadio(ICON_FA_CODE, nav_btn_size, 1, &nav_num);
                        ImAdd::NavigationRadio(ICON_FA_GEAR, nav_btn_size, 2, &nav_num);
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleVar();
                }
                ImGui::SetCursorPos(ImVec2(WindowPos.x + gui::sidebar_width + style.WindowPadding.x, WindowPos.y + gui::title_height + style.WindowPadding.y));
                {
                    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 0.f);
                    ImGui::BeginChild("content", ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground);
                    {
                        if (nav_num == 0)
                        {
                            ImGui::SameLine(ImGui::GetWindowSize().x / 2 - 200);
                            ImGui::PushItemWidth(350);
                            ImGui::Combo("Hash", &currentHashIndex, hashes, IM_ARRAYSIZE(hashes));
                            ImGui::PopItemWidth();

                            ImGui::NewLine();
                            ImGui::SameLine(ImGui::GetWindowSize().x / 2 - 200);
                            ImGui::PushItemWidth(350);
                            static char toBeHashed[512] = "Input (max:512)";
                            ImGui::InputTextMultiline("##", toBeHashed, IM_ARRAYSIZE(toBeHashed), ImVec2(350,100));
                            ImGui::PopItemWidth();

                            ImGui::NewLine();
                            ImGui::SameLine(ImGui::GetWindowSize().x / 2 - 200);
                            ImGui::InputTextMultiline("Output", (char*)digest.c_str(), 512, ImVec2(350,22), ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_Multiline);

                            ImGui::NewLine();
                            ImGui::SameLine(ImGui::GetWindowSize().x / 2 - 200);
                            ImGui::PushItemWidth(350);
                            if (ImGui::Button("Update", ImVec2(100, 22)))
                            {
                                const char* hash = hashes[currentHashIndex]; // selected hash as string

                                switch (currentHashIndex) {
                                    case 0: // MD5
                                        digest = GetMD5String(toBeHashed);
                                        break;
                                    case 1: // SHA-1
                                        checksum_sha1.update(toBeHashed);
                                        digest = checksum_sha1.final();
                                        break;
                                    case 2: // SHA-256
                                        checksum_sha256.update(toBeHashed);
                                        digest = SHA256::toString(checksum_sha256.digest());
                                        break;
                                    case 3: // SHA-512
                                        digest = sha512(toBeHashed);
                                        break;
                                }
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Clear", ImVec2(50, 22))) {
                                digest.clear();
                            }
                            ImGui::PopItemWidth();

                        }
                        else if (nav_num == 1)
                        {
                            ImGui::Text("Custom Hashes - Lua");
                            editor.Render("TextEditor");
                            ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
                                editor.IsOverwrite() ? "Ovr" : "Ins",
                                editor.CanUndo() ? "*" : " ",
                                editor.GetLanguageDefinition().mName.c_str(), customHashFile);

                        }
                        else if (nav_num == 2)
                        {
                            ImGui::Text("Made by Kobley ");
                            ImGui::SameLine();
                            ImGui::Text(ICON_FA_HEART);
                        } 
                        else
                        {
                            ImGui::Text("How did you get here...?");
                        }
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleVar();
                }
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0 };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
    }
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
	{
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xFFF0) == SC_KEYMENU) // Disable ALT application menu
			return 0L;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0L;
	case WM_LBUTTONDOWN:
		gui::guiPosition = MAKEPOINTS(lParam);
		return 0L;
	case WM_MOUSEMOVE:
		if (wParam == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(lParam);
			auto rect = ::RECT { };
			
			GetWindowRect(hWnd, &rect);
			
			rect.left += points.x - gui::guiPosition.x;
			rect.top += points.y - gui::guiPosition.y;
			
			if (gui::guiPosition.x >= 0 &&
                gui::guiPosition.x >= 0 && gui::guiPosition.x <= (gui::size.x - (gui::title_height + (ImGui::GetStyle().WindowBorderSize * 2)) ) &&
                gui::guiPosition.y >= 0 && gui::guiPosition.y <= gui::title_height)
			{
				SetWindowPos(
					hWnd,
					HWND_TOPMOST,
					rect.left, rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
			}
		}
		
		return 0L;
	}

    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
