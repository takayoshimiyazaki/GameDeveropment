//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <sstream>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <CommonStates.h>

extern void ExitGame();

using namespace DirectX;
using Microsoft::WRL::ComPtr;

Game::Game() :
    m_window(0),
    m_outputWidth(800),
    m_outputHeight(600),
    m_featureLevel(D3D_FEATURE_LEVEL_9_1)
{
}

Game::~Game()
{
	// サウンド処理の破棄
	ADX2Le::Finalize();
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_window = window;
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateDevice();

    CreateResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

	m_spriteBatch = std::make_unique<SpriteBatch>(m_d3dContext.Get());
	m_spriteFont = std::make_unique<SpriteFont>(m_d3dDevice.Get(), L"Resources/myfile.spritefont");

	m_count = 0;

	
	ComPtr<ID3D11Resource> resource;
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(m_d3dDevice.Get(), L"Resources/Attack.jpg",
			resource.GetAddressOf(),
			m_attackTexture.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(m_d3dDevice.Get(), L"Resources/Guard.jpg",
			resource.GetAddressOf(),
			m_guardTexture.ReleaseAndGetAddressOf()));
	/*DX::ThrowIfFailed(
		CreateDDSTextureFromFile(m_d3dDevice.Get(), L"Resources/cat.dds",
			resource.GetAddressOf(),
			m_texture.ReleaseAndGetAddressOf()));*/
	// 猫のテクスチャ
	/*ComPtr<ID3D11Texture2D> cat;
	DX::ThrowIfFailed(resource.As(&cat));*/
	// 攻撃のテクスチャ
	ComPtr<ID3D11Texture2D> attack;
	DX::ThrowIfFailed(resource.As(&attack));
	// 防御のテクスチャ
	ComPtr<ID3D11Texture2D> guard;
	DX::ThrowIfFailed(resource.As(&guard));

	// テクスチャの設定
	CD3D11_TEXTURE2D_DESC attackDesc;
	attack->GetDesc(&attackDesc);
	CD3D11_TEXTURE2D_DESC guardDesc;
	guard->GetDesc(&guardDesc);

	// テクスチャの原点をテクスチャの中心にする
	m_origin.x = float(attackDesc.Width / 2);
	m_origin.y = float(attackDesc.Height / 2);
	m_origin.x = float(guardDesc.Width / 2);
	m_origin.y = float(guardDesc.Height / 2);

	// 表示座標を画面中心に設定
	m_screenPos.x = m_outputWidth / 2.f;
	m_screenPos.y = m_outputHeight / 2.f;

	// キーボードのオブジェクトの生成
	m_keyboard = std::make_unique<Keyboard>();

	// マウスのオブジェクトの生成
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);

	// acfファイルの読み込み
	//ADX2Le::Initialize("Resources/Music/ADX2_samples.acf");
	ADX2Le::Initialize("Resources/Music/Aikatsu_ChangeScene.acf");
	// ACB,AWBファイルの読み込み
	ADX2Le::LoadAcb("Resources/Music/Aikatsu_ChangeScene.acb", "Resources/Music/Aikatsu_ChangeScene.awb");
	ADX2Le::Play(CRI_AIKATSU_CHANGESCENE__CUE_ID_1);

	// ゲームパッドの生成
	m_gamePad = std::make_unique<GamePad>();

	m_pJouyPad = std::make_unique<JoyPad>();
	m_pJouyPad->Initialize(window);

	m_attack = false;
	m_guard = false;
	m_mode = false;
}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;

	// サウンドライブラリの毎更新
	ADX2Le::Update();

	m_count++;

	//m_str = L"aaaaaa";
	std::wstringstream ss;

	ss << L"Please Push";

	m_str = ss.str();

	// キーボードの情報を取得
	auto kb = m_keyboard->GetState();

	m_keytracker.Update(kb);

	if (m_keytracker.pressed.Space)
	{
		// Space was just pressed down
		m_str = L"SpacePush";
	}
	if (m_keytracker.IsKeyReleased(Keyboard::Space))
	{
		// F1 key was just released
		m_str = L"Released";
	}
	if (m_keytracker.IsKeyReleased(Keyboard::Back))
	{
		// F1 key was just released
		m_str = L"Released";
	}
	
	if (kb.Back)
	{
		// Backspace key is down
		m_str = L"BackSpace";
	}
	if (kb.W)
	{
		// W key is down
	}
	if (kb.A)
	{
		// A key is down
	}
	if (kb.S)
	{
		// S key is down
	}
	if (kb.D)
	{
		// D key is down
	}
	if (kb.LeftShift)
	{
		// Left shift key is down
	}
	if (kb.RightShift)
	{
		// Right shift key is down
	}

	// マウスの状態を取得
	Mouse::State state = m_mouse->GetState();
	m_tracker.Update(state);

	if (state.leftButton)
	{
		// Left button is down
		m_str = L"Mouse LeftButton";
	}
	if (state.rightButton)
	{
		// Right button is down
		//m_str = L"Mouse RightButton";
	}
	if (state.middleButton)
	{
		// Middle button is down
		m_str = L"Mouse MiddleButton";
	}

	SimpleMath::Vector2::XMFLOAT2 mousePosInPixels(float(state.x), float(state.y));
	// This is the absolute position of the mouse relative
	// to the upper-left corner of the window
	// スプライトを動かす
	//m_screenPos = mousePosInPixels;

	if (m_tracker.rightButton == Mouse::ButtonStateTracker::PRESSED)
	{
		// Take an action when Right mouse button is first pressed,
		// but don't do it again until the button is released and
		// then pressed again
		m_str = L"Triger";
	}
	if (m_tracker.rightButton == Mouse::ButtonStateTracker::RELEASED)
	{
		// Take an action when Right mouse button is first pressed,
		// but don't do it again until the button is released and
		// then pressed again
	}
	
	if (state.positionMode == Mouse::MODE_RELATIVE)
	{
		// state.x and state.y are relative values; system cursor is not visible
	}
	else
	{
		// state.x and state.y are absolute pixel values; system cursor is visible
	}

	/*if (m_tracker.leftButton == Mouse::ButtonStateTracker::ButtonState::PRESSED)
	{
		m_mouse->SetMode(Mouse::MODE_RELATIVE);
	}
	else if (m_tracker.leftButton == Mouse::ButtonStateTracker::ButtonState::RELEASED)
	{
		m_mouse->SetMode(Mouse::MODE_ABSOLUTE);
	}*/

	// ゲームパッドの状態取得
	DirectX::GamePad::State padState = m_gamePad->GetState(0, GamePad::DEAD_ZONE_CIRCULAR);

	if (padState.IsConnected())
	{
		if (padState.IsAPressed())
		{
			// Do action for button A being down
		}
		if (padState.buttons.y)
		{
			// Do action for button Y being down
		}
		if (padState.buttons.a)
		{
			// Do action for button A being down
			m_gamePad->SetVibration(0, 1.0f, 1.0f);
			if (m_mode)
			{
				m_guard = true;
			}
			else
			{
				m_attack = true;
			}
		}		
		else if (padState.buttons.b)
		{
			// Do action for button B being down
			if (m_mode)
			{
				m_attack = true;
			}
			else
			{
				m_guard = true;
			}
		}
		else
		{
			m_attack = false;
			m_guard = false;
		}
		
		if (padState.IsDPadLeftPressed())
		{
			// Do action for DPAD Left being down
		}
		// 方向キー
		if (padState.dpad.up || padState.dpad.down || padState.dpad.left || padState.dpad.right)
		{
			// Do action based on any DPAD change
		}
		// 左スティック左右
		float posx = padState.thumbSticks.leftX;
		// 左スティック上下
		float posy = padState.thumbSticks.leftY;
		// These values are normalized to -1 to 1

		float throttle = padState.triggers.right;
		// This value is normalized 0 -> 1

		// 振動の調整
		//m_gamePad->SetVibration(0, 1.0f, 1.0f);

		if (padState.IsLeftTriggerPressed())
		{
			// Do action based on a left trigger pressed more than halfway
		}
		if (padState.IsBackPressed())
		{
			// Do action based on a Back pressed more than halfway
			if (m_mode)
			{
				m_mode = false;
			}
			else
			{
				m_mode = true;
			}
		}
		if (padState.IsViewPressed())
		{	
			// This is an alias for the Xbox 360 'Back' button
			// which is called 'View' on the Xbox One controller. 
		}
	}
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    // TODO: Add your rendering code here.
	CommonStates states(m_d3dDevice.Get());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());

	// テクスチャの切り取り
	//RECT rect;

	/*rect.left = 30;
	rect.right = 70;
	rect.top = 30;
	rect.bottom = 70;*/

	// スプライトの描画
	if (m_attack)
	{
		m_spriteBatch->Draw(m_attackTexture.Get(), m_screenPos, nullptr, Colors::White,
			XMConvertToRadians(0.0), m_origin);
	}
	if (m_guard)
	{
		m_spriteBatch->Draw(m_guardTexture.Get(), m_screenPos, nullptr, Colors::White,
			XMConvertToRadians(0.0), m_origin);
	}

	//m_spriteFont->DrawString(m_spriteBatch.get(), m_str.c_str(), XMFLOAT2(100, 100));

	m_spriteBatch->End();
    Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::CornflowerBlue);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    static const D3D_FEATURE_LEVEL featureLevels [] =
    {
        // TODO: Modify for supported Direct3D feature levels (see code below related to 11.1 fallback handling).
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Create the DX11 API device object, and get a corresponding context.
    HRESULT hr = D3D11CreateDevice(
        nullptr,                                // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        m_d3dDevice.ReleaseAndGetAddressOf(),   // returns the Direct3D device created
        &m_featureLevel,                        // returns feature level of device created
        m_d3dContext.ReleaseAndGetAddressOf()   // returns the device immediate context
        );

    if (hr == E_INVALIDARG)
    {
        // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it.
        hr = D3D11CreateDevice(nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            &featureLevels[1],
            _countof(featureLevels) - 1,
            D3D11_SDK_VERSION,
            m_d3dDevice.ReleaseAndGetAddressOf(),
            &m_featureLevel,
            m_d3dContext.ReleaseAndGetAddressOf()
            );
    }

    DX::ThrowIfFailed(hr);

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(m_d3dDevice.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide [] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    // DirectX 11.1 if present
    if (SUCCEEDED(m_d3dDevice.As(&m_d3dDevice1)))
        (void)m_d3dContext.As(&m_d3dContext1);

    // TODO: Initialize device dependent objects here (independent of window size).
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory1> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        ComPtr<IDXGIFactory2> dxgiFactory2;
        if (SUCCEEDED(dxgiFactory.As(&dxgiFactory2)))
        {
            // DirectX 11.1 or later

            // Create a descriptor for the swap chain.
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
            swapChainDesc.Width = backBufferWidth;
            swapChainDesc.Height = backBufferHeight;
            swapChainDesc.Format = backBufferFormat;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = backBufferCount;

            DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = { 0 };
            fsSwapChainDesc.Windowed = TRUE;

            // Create a SwapChain from a Win32 window.
            DX::ThrowIfFailed(dxgiFactory2->CreateSwapChainForHwnd(
                m_d3dDevice.Get(),
                m_window,
                &swapChainDesc,
                &fsSwapChainDesc,
                nullptr,
                m_swapChain1.ReleaseAndGetAddressOf()
                ));

            DX::ThrowIfFailed(m_swapChain1.As(&m_swapChain));
        }
        else
        {
            DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
            swapChainDesc.BufferCount = backBufferCount;
            swapChainDesc.BufferDesc.Width = backBufferWidth;
            swapChainDesc.BufferDesc.Height = backBufferHeight;
            swapChainDesc.BufferDesc.Format = backBufferFormat;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.OutputWindow = m_window;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.Windowed = TRUE;

            DX::ThrowIfFailed(dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &swapChainDesc, m_swapChain.ReleaseAndGetAddressOf()));
        }

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain1.Reset();
    m_swapChain.Reset();
    m_d3dContext1.Reset();
    m_d3dContext.Reset();
    m_d3dDevice1.Reset();
    m_d3dDevice.Reset();

    CreateDevice();

    CreateResources();
}