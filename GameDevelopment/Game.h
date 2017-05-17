//
// Game.h
//

#pragma once

#include "StepTimer.h"
#include <SpriteFont.h>
#include <SpriteBatch.h>
#include <SimpleMath.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <GamePad.h>
#include "ADX2Le.h"
#include "Resources\Music\Basic.h"
#include "Resources\Music\Aikatsu_ChangeScene.h"
#include "JoyPad.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game
{
public:

    Game();
	virtual ~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();
    void Present();

    void CreateDevice();
    void CreateResources();

    void OnDeviceLost();

    // Device resources.
    HWND                                            m_window;
    int                                             m_outputWidth;
    int                                             m_outputHeight;

    D3D_FEATURE_LEVEL                               m_featureLevel;
    Microsoft::WRL::ComPtr<ID3D11Device>            m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice1;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>     m_d3dContext;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext1;

    Microsoft::WRL::ComPtr<IDXGISwapChain>          m_swapChain;
    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain1;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;
	std::wstring m_str;
	int m_count;

    // Rendering loop timer.
    DX::StepTimer                                   m_timer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_attackTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_guardTexture;
	DirectX::SimpleMath::Vector2 m_screenPos;		// スプライト表示の座標
	DirectX::SimpleMath::Vector2 m_origin;			// スプライトの原点

	// キーボード
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	// キーボードトラッカー
	DirectX::Keyboard::KeyboardStateTracker m_keytracker;

	// マウス
	std::unique_ptr<DirectX::Mouse> m_mouse;
	DirectX::Mouse::ButtonStateTracker m_tracker;

	// ゲームパッド
	std::unique_ptr<DirectX::GamePad> m_gamePad;
	std::unique_ptr<JoyPad> m_pJouyPad;

	bool m_attack;
	bool m_guard;
	bool m_mode;
};