//**************************************************************************//
// The DirectX9 Thing3D now ported to DirextX10.  This module is based on	//
// Microsoft’s “Simple Sample”, but I have done quite a lot of poggeling.   //
// Apart from using my own Thing3D.											//
//																			//
// This (poggled) code is copyright of Dr Nigel Barlow, lecturer in			//
// computing, University of Plymouth, UK.  email: nigel@soc.plymouth.ac.uk.	//
//																			//
// You may use, modify and distribute this (rather cack-handed in places)	//
// code subject to the following conditions:								//
//																			//
//	1:	You may not use it, or sell it, or use it in any adapted form for	//
//		financial gain, without my written premission.						//
//																			//
//	2:	You must not remove the copyright messages.							//
//																			//
//	3:	You should correct at least 10% of the typig abd spekking errirs.   //
//																			//
// Poggeling ©Nigel Barlow nigel@soc.plymouth.ac.uk.						//
// "Simple Sample" ©Microsoft Corporation. All rights reserved.				//
//**************************************************************************//
//--------------------------------------------------------------------------------------
// File: Basic Thing3D.cpp
//
// Starting point for new Direct3D 11 samples.  For a more basic starting point, 
// use the EmptyProject11 sample instead.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTmisc.h"
#include "DXUTCamera.h"
#include "DXUTSettingsDlg.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "resource.h"
#include "Nig.h"
#include "Thing3D.h"
#include "HeightMap\HeightMap.h"
#include "Thing3DVS_PS_Pair.h"
#include "Object3D.h"
#include <vector>		//Standard lib, no ".h?
#include "Thing3D\BulletManager.h"

#include "../ObjectLoader/ObjectLoader.h"


//#define DEBUG_VS   // Uncomment this line to debug D3D9 vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug D3D9 pixel shaders 

//**************************************************************************//
// Global Variables.  There are many global variables here (we aren't OO	//
// yet.  I doubt  Roy Tucker (Comp Sci students will know him) will			//
// approve pf this either.  Sorry, Roy.										//
//**************************************************************************//
CModelViewerCamera          g_Camera;				// Not used by Nigel.
CDXUTDirectionWidget        g_LightControl;			// Not used by Nigel.
CDXUTDialogResourceManager  g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg             g_SettingsDlg;          // Device settings dialog
CDXUTTextHelper*            g_pTxtHelper = NULL;
CDXUTDialog                 g_HUD;                  // dialog for standard controls
CDXUTDialog                 g_SampleUI;             // dialog for sample specific controls

// Direct3D 9 resources
extern ID3DXFont*           g_pFont9;
extern ID3DXSprite*         g_pSprite9;

//**************************************************************************//
// Nigel added global variables.											//
//**************************************************************************//
Object3D		*cyborg;
Object3D        *cyborgGun;
Thing3D			*seaFloor;
Thing3D         *island;
Thing3D         *sky;

HeightMap       *floorHeightMap;

XMMATRIX		g_MatProjection;
XMMATRIX		g_matView;

int				g_width  = 800;
int				g_height = 600;;

bool			g_b_LeftArrowDown      = false;	//Status of keyboard.  Thess are set
bool			g_b_RightArrowDown     = false;	//in the callback KeyboardProc(), and 
bool			g_b_UpArrowDown	       = false; //are used in onFrameMove().
bool			g_b_DownArrowDown	   = false;
bool		    g_b_SpaceArrowDown     = false;
bool		    g_b_zArrowDown		   = false;
bool		    g_b_xArrowDown		   = false;
bool			g_b_f1ArrowDown        = false;
bool			g_b_f2ArrowDown        = false;
bool			g_b_ControlArrowDown   = false;

bool			g_b_culling			   = true;
float		    g_f_cullingAngle       = XMConvertToRadians(90);

int				resourcesCollected     = 0;
int				bulletsRemaining       = 20;
int				zombiesRemaining       = 10;

//
// List of objects 
//
std::vector <Object3D *> zombies(0);	
std::vector <Object3D *> buildings(0);
std::vector <Object3D *> resources(0);
std::vector <Object3D *> bulletStorage(0);    // a "stack" of bullets (in this case 20 - which will be added during load)
BulletManager *bullets = new BulletManager(); // class that manages the states of bullets (which are Object3Ds)

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3


//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext );
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );

extern bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
                                             bool bWindowed, void* pUserContext );
extern HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice,
                                            const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
extern HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                           void* pUserContext );
extern void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime,
                                        void* pUserContext );
extern void CALLBACK OnD3D9LostDevice( void* pUserContext );
extern void CALLBACK OnD3D9DestroyDevice( void* pUserContext );

bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext );
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                         const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D11DestroyDevice( void* pUserContext );
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                                 float fElapsedTime, void* pUserContext );
bool CheckVisible(Thing3DAbstract           *toCheck);
void InitApp();
void RenderText();

Object3D *NewCyborg(ID3D11Device *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);
Object3D *NewCyborgGun(ID3D11Device *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);

Thing3D *NewLand(ID3D11Device  *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);
Thing3D *NewIsland(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3dImmediateContext);
Thing3D *NewSky(ID3D11Device *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);

void NewMonsters(ID3D11Device        *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);
void NewBuildings(ID3D11Device  *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);
void NewResources(ID3D11Device *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);
void NewBullets(ID3D11Device *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);

HeightMap *NewFloor(ID3D11Device *PRenderingDevice, ID3D11DeviceContext *pImmediateContext);

//**************************************************************************//
// A Windows program always kicks off in WinMain.							//
// Initializes everything and goes into a message processing				//
// loop.																	//
//																			//
// This version uses DXUT, and is much more complicated than previous		//
// versions you have seen.  This allows you to sync the frame rate to your  //
// monitor's vertical sync event.											//
//**************************************************************************//
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, 
					LPWSTR lpCmdLine,    int        nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device (either D3D9 or D3D11) 
    // that is available on the system depending on which D3D callbacks are set below

	//**************************************************************************//
	// Set DXUT callbacks.														//
    //**************************************************************************//
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );

    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );

    DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
    DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
    DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );
    DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );

    InitApp();
    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true );
    DXUTCreateWindow( L"Expanding on from Assignment 1 submission - Adam Stevenson" );

    // Only require 10-level hardware, change to D3D_FEATURE_LEVEL_11_0 to require 11-class hardware
    // Switch to D3D_FEATURE_LEVEL_9_x for 10level9 hardware
    DXUTCreateDevice( D3D_FEATURE_LEVEL_11_0, true, 640, 480 );

    DXUTMainLoop(); // Enter into the DXUT render loop

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent );
    int iY = 30;
    int iYo = 26;
    //g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, iY, 170, 22 );
    //g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 0, iY += iYo, 170, 22, VK_F3 );
    //g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 0, iY += iYo, 170, 22, VK_F2 );

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
    g_pTxtHelper->Begin();
    g_pTxtHelper->SetInsertionPos( 5, 5 );
    g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    g_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
    g_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );

	//
	// Controls
	//
	if(g_b_f1ArrowDown == false)
		g_pTxtHelper->DrawTextLine( L"Press F1 for controls");
	else 
	{
		g_pTxtHelper->DrawTextLine( L" X button: zoom out");
		g_pTxtHelper->DrawTextLine( L" Z button: zoom in");
	}
	
	if(g_b_f2ArrowDown == false)
		g_pTxtHelper->DrawTextLine( L"Press F2 for objectives");
	else
	{
		g_pTxtHelper->DrawTextLine( L"Gary the cyborgs wedding got too out of hand. Waking up the following day on an island\n "
			L"filled with zombies, the only way to survive until the rescue crews arrival is to: \n"
			L"1) Collect all of the resource crates (yes, ALL of them...for some reason). \n" 
			L"2) Exterminate all of the zombies");
	}

	// variables to keep track of game status
	bool wonGame = false;

	// The condition for winning the game
	if(resourcesCollected == 10 && zombiesRemaining == 0)
		wonGame = true;

	if(!wonGame)
	{
		//
		// How many resources have been collected
		//
		std::wstring s = std::wstring(L"\n\n\n\n\nResources collected: ") + std::to_wstring(resourcesCollected)
			+ std::wstring(L"/10");

		g_pTxtHelper->DrawTextLine(&s[0]); // From the beginning of the wstring, read until the end

		// Render how much ammo is remaining
		s = std::wstring(L"\n\n\nAmmo Remaining:      ") + std::to_wstring(bulletsRemaining)
			+ std::wstring(L"/20");

		g_pTxtHelper->DrawTextLine(&s[0]);

		// Render how many zombies are remaining
		s = std::wstring(L"\n\n\n\n\nZombies Remaining:   ") + std::to_wstring(zombiesRemaining)
			+ std::wstring(L"/10");

		g_pTxtHelper->DrawTextLine(&s[0]); 
	}
	else
	{
		std::wstring s = std::wstring(L"\n\n\n\nYou have saved Gary the Cyborg! Congratulations!");

		g_pTxtHelper->DrawTextLine(&s[0]);
	}


    g_pTxtHelper->End();
}


//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext )
{
    HRESULT hr;

    ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
    V_RETURN( g_DialogResourceManager.OnD3D11CreateDevice( pd3dDevice, pd3dImmediateContext ) );
    V_RETURN( g_SettingsDlg.OnD3D11CreateDevice( pd3dDevice ) );
    g_pTxtHelper = new CDXUTTextHelper( pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager, 15 );

	floorHeightMap = NewFloor(pd3dDevice, pd3dImmediateContext);
	cyborg         = NewCyborg(pd3dDevice, pd3dImmediateContext);
	cyborgGun      = NewCyborgGun(pd3dDevice, pd3dImmediateContext);
	seaFloor       = NewLand( pd3dDevice, pd3dImmediateContext );
	sky            = NewSky(pd3dDevice, pd3dImmediateContext);

	NewMonsters(pd3dDevice, pd3dImmediateContext);
	NewBuildings(pd3dDevice, pd3dImmediateContext);
	NewResources(pd3dDevice, pd3dImmediateContext);
	NewBullets(pd3dDevice, pd3dImmediateContext);

	//**************************************************************************//
	// Initialize the projection matrix.  Generally you will only want to create//
	// this matrix once and then forget it.	I think we should rebuild this		//
	// matrix if we re-size the swap chain, though.								//
	//**************************************************************************//
	g_MatProjection = XMMatrixPerspectiveFovLH( XM_PIDIV2,				// Field of view (pi / 2 radians, or 90 degrees
											 g_width / (FLOAT) g_height, // Aspect ratio.
											 0.01f,						// Near clipping plane.
											 350.0f );					// Far clipping plane.

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                         const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );
    V_RETURN( g_SettingsDlg.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );
	g_width  = pBackBufferSurfaceDesc->Width;
	g_height = pBackBufferSurfaceDesc->Height;

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
 
    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width - 170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 300 );
    g_SampleUI.SetSize( 170, 300 );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                                 float fElapsedTime, void* pUserContext )
{


    // If the settings dialog is being shown, then render it instead of rendering the app's scene
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.OnRender( fElapsedTime );
        return;
    }       

	// fps: 

    float ClearColor[4] = { 0.176f, 0.196f, 0.667f, 0.0f };
    ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
    pd3dImmediateContext->ClearRenderTargetView( pRTV, ClearColor );

    // Clear the depth stencil
    ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
    pd3dImmediateContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );

  	//****************************************************************************//
	//								Camera settings								  //
	//****************************************************************************//
	//
	// Initialise the view matrix
	//
	XMVECTOR Eye = XMVectorSet(0, 2, -10, 0);	// Un-necessary, we overwrite this.
	XMVECTOR At;
	XMVECTOR Up  = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	
	//
	// Determine the lookat point
	//
	At = XMVectorSet(cyborg->x, cyborg->y + 1.3 + cyborg->height, cyborg->z, 0);

	//
	// Set the things current direction vector
	//
	XMVECTOR localDir = cyborg->currentVecDir;

	localDir *= -2;		// 3 paces back

	//
	// Obtain the camera position
	//
	Eye = XMVectorSet(cyborg->x + XMVectorGetX(localDir),
					  cyborg->y + XMVectorGetY(localDir) + 2.5 + cyborg->cameraZoom,
					  cyborg->z + XMVectorGetZ(localDir),
					  0);

	g_matView= XMMatrixLookAtLH( Eye,	// The eye, or viewer's position.
								 At,	// The look at point.
								 Up );	// Which way is up.

	//
	// Render the main player - the flying thing
	//
	cyborg->matView       = g_matView;
	cyborg->matProjection = g_MatProjection;
	cyborg->Render(pd3dDevice, pd3dImmediateContext);
	
	// position relative to the height map. This isnt complete yet.
	// I will be working more on this in my spare time in the future
	//
	// In addition to this, the height of the cyborg is used later on, so
	// I floorf the X/Z here to avoid additional computations later on.
	int cyborgFloorX = (int) floorf(0.5 + cyborg->x); if(cyborgFloorX < 0) cyborgFloorX *=-1;
	int cyborgFloorZ = (int) floorf(0.5 + cyborg->z); if(cyborgFloorZ < 0) cyborgFloorZ *=-1;

	float playerRelativeToHM = floorHeightMap->getHeight(cyborgFloorX, cyborgFloorZ);
	
	// and then move the player
	cyborg->y = playerRelativeToHM - 25.5f; // minus the initial height difference
											// which is 255 / 10

	//
	// The sky
	//
	sky->matView       = cyborg->matView;
	sky->matProjection = cyborg->matProjection;
	sky->ry            = -cyborg->ry;
	sky->x             =  cyborg->x;
	sky->z             =  cyborg->z;
	sky->Render(pd3dDevice, pd3dImmediateContext);

	//
	// The height map
	//
	floorHeightMap->matView = g_matView;
	floorHeightMap->matProjection = g_MatProjection;
	floorHeightMap->Render(pd3dDevice, pd3dImmediateContext);

	//
	// Render the (very small) ocean/sea
	//
	seaFloor->matView = g_matView;
	seaFloor->matProjection = g_MatProjection;
	seaFloor->Render(pd3dDevice, pd3dImmediateContext);

	//
	// zombies
	//
	int numZombies = zombies.size();
	if(numZombies > 0)
	{
		for (int i = 0; i < numZombies; i++)
		{
			Object3D *zombieObject  = (Object3D *) zombies[i];

			//
			// Render the zombies
			// 
			if(CheckVisible(zombieObject))
			{
				if (i == 0) zombieObject->buffersAndtexturesSet = false; 
				else        zombieObject->buffersAndtexturesSet = true;

				zombieObject->matView       = g_matView;
				zombieObject->matProjection = g_MatProjection;
				zombieObject->Render(pd3dDevice, pd3dImmediateContext);
			
				//
				// Move the zombies
				//
				int zombiePositionX = (int) floorf(0.5 + zombieObject->x); if(zombiePositionX < 0) zombiePositionX *=-1;
				int zombiePositionZ = (int) floorf(0.5 + zombieObject->z); if(zombiePositionZ < 0) zombiePositionZ *=-1;

				if(zombiePositionX != cyborgFloorX)
				{
					if(zombiePositionX < cyborgFloorX) zombieObject->x += 0.04;
					if(zombiePositionX > cyborgFloorX) zombieObject->x -= 0.04;
				}
				if(zombiePositionZ != cyborgFloorZ)
				{
					if(zombiePositionZ < cyborgFloorZ) zombieObject->z -= 0.04;
					if(zombiePositionZ > cyborgFloorZ) zombieObject->z += 0.04;
				}

				zombieObject->y = floorHeightMap->getHeight(zombiePositionX, zombiePositionZ) - 24.0f;
			
				//
				// Check if there is a collision between a bullet and a zombie
				//
				for(int j = 0; j < bullets->bulletManager.amountOfBullets; j++)
				{
					bool isVisible = bullets->bulletManager.bullets[j].isVisible;

					// Make sure the bullet is visible, to save on computations
					if(isVisible)
					{
						Object3D *bullet = bullets->bulletManager.bullets[j].bulletObj;

						int bulletPositionX = (int) floorf(0.5 + bullet->x); if(bulletPositionX < 0) bulletPositionX *=-1;
						int bulletPositionZ = (int) floorf(0.5 + bullet->z); if(bulletPositionZ < 0) bulletPositionZ *=-1;

						// determine the distance between the two points
						float differenceX, differenceZ;

						if(zombiePositionX >= bulletPositionX) differenceX = zombiePositionX - bulletPositionX;
						else						           differenceX = bulletPositionX - zombiePositionZ;

						if(zombiePositionZ >= bulletPositionZ) differenceZ = zombiePositionZ - bulletPositionZ;
						else						           differenceZ = bulletPositionZ - zombiePositionZ;

						if(differenceX < 1 && differenceX > -1)
							if(differenceZ < 1 && differenceZ > -1){
							{
								// Minus one to the remaining zombies count
								zombiesRemaining --;

								// erase the index of the zombies from the vector
								zombies.erase(zombies.begin() + i);

								numZombies --;

								// make this particular bullet invisible
								bullets->bulletManager.bullets[j].isVisible = false;
							}
						}
					}
				}
			}
		}
	}

	//
	// Buildings
	//
	int numBuildings = buildings.size();
	if(numBuildings > 0)
	for(int i = 0; i < numBuildings; i++)
	{
		Object3D *buildingObject = buildings[i];

		if(CheckVisible(buildingObject))
		{
			if(i == 0) buildingObject->buffersAndtexturesSet = false;
			else       buildingObject->buffersAndtexturesSet = true;

			// render the building
			buildingObject->matView = g_matView;
			buildingObject->matProjection = g_MatProjection;
			buildingObject->Render(pd3dDevice, pd3dImmediateContext);
		}
	}

	//
	// Collectables
	//
	if(resources.size() > 0)
	for(int i = 0; i < resources.size(); i++)
	{
		Object3D *resource = resources[i];

		if(i == 0) resource->buffersAndtexturesSet = false;
		else       resource->buffersAndtexturesSet = true;

		if(CheckVisible(resource))
		{
			// render the crate/resource to collect
			resource->matView = g_matView;
			resource->matProjection = g_MatProjection;
			resource->Render(pd3dDevice, pd3dImmediateContext);

			//
			// Detect a collision with the player
			//
			int resourceX = (int) floorf(0.5 + resource->x); if(resourceX < 0) resourceX *=-1;
			int resourceZ = (int) floorf(0.5 + resource->z); if(resourceZ < 0) resourceZ *=-1;
		
			// check the players x/z and the resources x/z
			if(resourceX == cyborgFloorX && resourceZ == cyborgFloorZ)
			{
				// if the floor value of the heightmap is the same for both the player and the
				// resource, then clear the resource and add the value to the text mapping on the left
				// hand side of the application.
				resourcesCollected +=1;
				resources.erase(resources.begin() + i);
			}
		}
	}

	//
	// Render the bullets
	//
	for(int i = 0; i < bullets->bulletManager.amountOfBullets; i++)
	{
		if(bullets->bulletManager.bullets[i].isActive)
		{
			// This is also used to toggle the rendering state. It is disabled
			// when the bullet has been on the screen for too long (in other words,
			// to make it dissapear after a certain distance).
			bool isObjectVisible = bullets->bulletManager.bullets[i].isVisible;
			
			if(isObjectVisible)
			{
				Object3D *bullet = bullets->bulletManager.bullets[i].bulletObj;

				bullet->matView = g_matView;
				bullet->matProjection = g_MatProjection;
				bullet->Render(pd3dDevice, pd3dImmediateContext);
			}
		}
	}

	//****************************************************************************//
	// And finally, render the DXUT buttons and the thing which grandly calls	  //
	// itself a HUD (Head Up Display).											  //
	//****************************************************************************//
    DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
    g_HUD.OnRender( fElapsedTime );
    g_SampleUI.OnRender( fElapsedTime );
    RenderText();
    DXUT_EndPerfEvent();

    static DWORD dwTimefirst = GetTickCount();
    if ( GetTickCount() - dwTimefirst > 5000 )
    {    
        OutputDebugString( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
        OutputDebugString( L"\n" );
        dwTimefirst = GetTickCount();
    }
}

//**************************************************************************//
// Handle updates to the scene.  This is called regardless of which D3D		//
// API is used (we are only using Dx11).									//
//**************************************************************************//
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	float speedIncrease = 0.015;

	if (g_b_LeftArrowDown)  cyborg->ry -= speedIncrease*3.0f;
	if (g_b_RightArrowDown) cyborg->ry += speedIncrease*3.0f; 
	if (g_b_UpArrowDown)    cyborg->speed += speedIncrease*3.0f;
	if (g_b_DownArrowDown)
	{
		//
		// Increase the speed
		//
		cyborg->speed  -= speedIncrease*6.0f;

		//
		// Increate the height
		//
		if(cyborg->height > 0)
		{
			cyborg->height -= speedIncrease*2.0f;
			cyborg->cameraZoom -=speedIncrease*2.0f;
		}
	}

	//
	// If Z or X, zoom the camera in or out. 
	//
	if (g_b_xArrowDown) cyborg->cameraZoom += speedIncrease*3.0f;
	if(cyborg->cameraZoom >= 1.5) cyborg->cameraZoom = 1.5;

	if (g_b_zArrowDown) cyborg->cameraZoom -= speedIncrease*3.0f;
	if(cyborg->cameraZoom <= -2) cyborg->cameraZoom = -2;
	
	//
	// Move the cyborg
	//
	cyborg->frameTime = fElapsedTime;
	cyborg->DoBehaviour(); 

	//
	// Bullets
	//
	if(g_b_ControlArrowDown)
	{
		// Determine if a bullet has been altered
		bool bulletChanged = false;

		for(int i = 0; i < bullets->bulletManager.amountOfBullets; i++)
		{
			if(!bulletChanged)
			{
				Object3D *bullet = bullets->bulletManager.bullets[i].bulletObj;
				bool isActive  = bullets->bulletManager.bullets[i].isActive;

				if(!isActive)
				{
					bullets->SetBulletActive(i, cyborg->x, cyborg->y, cyborg->z, cyborg->ry);

					// indicate that a bullet has been changed
					bulletChanged = true;

					// after the loop ends, ensure that it is not entered until the user
					// presses the control button again
					g_b_ControlArrowDown = false;

					// reduce the amount of available bullets by one
					bulletsRemaining --;
				}
			}
		}
	}
	else
	{
		for(int i = 0; i < bullets->bulletManager.amountOfBullets; i++)
		{
			Object3D *bullet = bullets->bulletManager.bullets[i].bulletObj;
			bool isActive    = bullets->bulletManager.bullets[i].isActive;
			bool isVisible   = bullets->bulletManager.bullets[i].isVisible;

			if(bullets->bulletManager.bullets[i].isActive)
			{
				bullet->frameTime = fElapsedTime *10.0f;
				bullet->MoveForward();
			}

			// Check the start X and Z of the object, against
			// where it currently is. If there is a specific distance on either end, then
			// make it not visible anymore (which is used to determine whether it is rendered or not
			// and also allows for determining how much "ammo" is remaining).
			//
			// Stored X and Z starting values
			if(isActive && isVisible)
			{
				float objectStartX = floorf(bullets->bulletManager.bullets[i].startX); if(objectStartX < 0) objectStartX *=-1;
				float objectStartZ = floorf(bullets->bulletManager.bullets[i].startZ); if(objectStartZ < 0) objectStartZ *=-1;

				// Current X and Z values
				float currentX = floorf(bullet->x); if(currentX < 0) currentX *=-1;
				float currentZ = floorf(bullet->z); if(currentZ < 0) currentZ *=-1;

				// determine the distance between the two points
				float differenceX, differenceZ;

				if(currentX >= objectStartX) differenceX = currentX - objectStartX;
				else						 differenceX = objectStartX - currentX;

				if(currentZ >= objectStartZ) differenceZ = currentZ - objectStartZ;
				else						 differenceZ = objectStartZ - currentZ;

				// if the distance is high enough, disable the visability (subsequently it will not be rendered as
				// a result of this).
				if(differenceX > 20 || differenceZ > 20)
				{
					bullets->bulletManager.bullets[i].isVisible = false;
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11ReleasingSwapChain();
}

//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D11 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    if( pDeviceSettings->ver == DXUT_D3D9_DEVICE )
    {
        IDirect3D9* pD3D = DXUTGetD3D9Object();
        D3DCAPS9 Caps;
        pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal, pDeviceSettings->d3d9.DeviceType, &Caps );

        // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
        // then switch to SWVP.
        if( ( Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
            Caps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
        {
            pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }

        // Debugging vertex shaders requires either REF or software vertex processing 
        // and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
        if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
        {
            pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
            pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
            pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }
#endif
#ifdef DEBUG_PS
        pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
    }

    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( ( DXUT_D3D9_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF ) ||
            ( DXUT_D3D11_DEVICE == pDeviceSettings->ver &&
            pDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE ) )
        {
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
        }

    }

    return true;
}

//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext )
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to settings dialog if its active
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

 
    return 0;
}


//**************************************************************************//
// Handle key presses.														//
//**************************************************************************//
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	HRESULT hr;

    switch( nChar )
    {

		//******************************************************************//
		// Nigel code to rotate and move the tiger.							//
		//******************************************************************//
		case VK_LEFT:  g_b_LeftArrowDown  = bKeyDown; break;
		case VK_RIGHT: g_b_RightArrowDown = bKeyDown; break;
		case VK_UP:    g_b_UpArrowDown    = bKeyDown; break;
		case VK_DOWN:  g_b_DownArrowDown  = bKeyDown; break;
		case 'A':      g_b_LeftArrowDown  = bKeyDown; break;
		case 'D':      g_b_RightArrowDown = bKeyDown; break;
		case 'W':      g_b_UpArrowDown    = bKeyDown; break;
		case 'S':      g_b_DownArrowDown  = bKeyDown; break;
		case VK_SPACE: g_b_SpaceArrowDown = bKeyDown; break;
		case 'Z':      g_b_zArrowDown = bKeyDown; break;
		case 'X':      g_b_xArrowDown = bKeyDown; break;
		case VK_F1 :   g_b_f1ArrowDown = bKeyDown; break;
		case VK_F2 :   g_b_f2ArrowDown = bKeyDown; break;
		case VK_CONTROL : g_b_ControlArrowDown = bKeyDown; break;
    }

	V(hr);
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN:
            DXUTToggleFullScreen();
            break;
        case IDC_TOGGLEREF:
            DXUTToggleREF();
            break;
        case IDC_CHANGEDEVICE:
            g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() );
            break;
    }
}

Thing3D *NewLand(ID3D11Device        *pRenderingDevice,
		 	      ID3D11DeviceContext *pImmediateContext)
{

	HRESULT hr = S_OK;

	//
	// Create and set the shaders
	//
	Thing3DVS_PS_Pair *pShader = new Thing3DVS_PS_Pair();
	pShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_VS");

	pShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "Thing3D_Textured_PS");

	pShader->CreateConstandBuffersAndSampler(pRenderingDevice);

	Thing3D *newLand = new Thing3D(pRenderingDevice, pImmediateContext);
	newLand->SetVS_PS_ShaderPair(pShader);

	newLand->SetVS_PS_ShaderPair(pShader);

	// The link for the water image: h ttp://www.seodesign.us/photoshop-patterns/45-free-water-photoshop-patterns-textures/
	newLand->CreateMesh(pRenderingDevice, pImmediateContext, L"Media\\floor\\sea\\seafloor.sdkmesh", 0);
	newLand->SetInitialDirection(0, 0, -1);
	newLand->SetScale(10, 2, 10);
	newLand->MoveTo(cyborg->x, cyborg->y + 1, -cyborg->z);
	newLand->RotateTo(0, XMConvertToRadians(180), 0);
	
	return newLand;
}

Thing3D *NewSky(ID3D11Device *pRenderingDevice,
				ID3D11DeviceContext *pImmediateContext)
{
	// create shader
	Thing3DVS_PS_Pair *pShader = new Thing3DVS_PS_Pair();

	pShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_VS");

	pShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "Thing3D_NoLighting_PS");

	pShader->CreateConstandBuffersAndSampler(pRenderingDevice);

	Thing3D *newSky = new Thing3D(pRenderingDevice, pImmediateContext);
	newSky->SetVS_PS_ShaderPair(pShader);
	newSky->CreateMesh(pRenderingDevice, pImmediateContext, L"Media\\Skybox\\skysphere.sdkmesh",0);
	newSky->SetScale(11, 6, 11);
	newSky->MoveTo(0, 20, 0);
	return newSky;
}

Thing3D *NewIsland(ID3D11Device        *pRenderingDevice,
		 	      ID3D11DeviceContext *pImmediateContext)
{
	//
	// Create and set the shaders
	//
	Thing3DVS_PS_Pair *pShader = new Thing3DVS_PS_Pair();

	pShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_VS");

	pShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "Thing3D_Textured_PS");

	pShader->CreateConstandBuffersAndSampler(pRenderingDevice);

	Thing3D *newIsland = new Thing3D(pRenderingDevice, pImmediateContext);
	newIsland->SetVS_PS_ShaderPair(pShader);

	newIsland->CreateMesh(pRenderingDevice, pImmediateContext, L"Media\\Island\\island.sdkmesh", 0);
	newIsland->SetInitialDirection(0, 0, -1);
	newIsland->SetScale(0.1, 0.1, 0.1);
	newIsland->MoveTo(45, 11, 0);
	newIsland->RotateTo(0, XMConvertToRadians(300), 0);

	return newIsland;
}

Object3D *NewCyborg(ID3D11Device        *pRenderingDevice,
		 	      ID3D11DeviceContext *pImmediateContext)
{
	HRESULT hr = S_OK;

	//
	// Create and set the shader
	//
	Thing3DVS_PS_Pair *pShader = new Thing3DVS_PS_Pair();
	pShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_CreateFur_VS");

	pShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "ObjectL_PS");

	pShader->CreateConstandBuffersAndSampler(pRenderingDevice);

	// Create the cyborg
	Object3D *newCyborg = new Object3D(pRenderingDevice, pImmediateContext);
	newCyborg->SetVS_PS_ShaderPair(pShader);

	newCyborg->SetObjMeshFromFile(L"cyborg.obj", L"Media\\soldier\\cyborg\\", pRenderingDevice);

	newCyborg->Create(pRenderingDevice, pImmediateContext);
	newCyborg->SetScale(0.7, 0.7, 0.7);
	newCyborg->MoveTo(0, -1, 0);
	newCyborg->RotateTo(0, 0, 0);
	newCyborg->SetInitialDirection(0, 0, 1);

	// set new values for specific lighting variables 
    newCyborg->materialDiffuseColour = XMFLOAT4(1, 1, 1, 1);
    newCyborg->LightAmbientColour    = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.4f);
    newCyborg->lightDiffuseColour    = XMFLOAT4(1, 1, 1, 1);
    newCyborg->vecLightDirection     = XMVectorSet(2, -10, -13, 0);	//W unused.

	// set the x and z of the cyborg to be the medium of the height map
	// but the z needs to be inverted because of the rotation
	// of the height map (see the NewFloor() below)
	XMFLOAT3 floorHMCenterXYZ = floorHeightMap->getAveragePosition();
	newCyborg->MoveTo(273.75 , -5, -304);

	// change initial direction so that we dont start with the player staring
	// at a random hill.
	newCyborg->ry = 130;
	
	V(hr);

	return newCyborg;
}

Object3D *NewCyborgGun(ID3D11Device        *pRenderingDevice,
		 	      ID3D11DeviceContext *pImmediateContext)
{

	HRESULT hr = S_OK;

	//
	// Create and set the shader
	//
	Thing3DVS_PS_Pair *pShader = new Thing3DVS_PS_Pair();
	pShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_CreateFur_VS");

	pShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "ObjectL_PS");

	pShader->CreateConstandBuffersAndSampler(pRenderingDevice);

	// Create the cyborg
	Object3D *newCyborgGun = new Object3D(pRenderingDevice, pImmediateContext);
	newCyborgGun->SetVS_PS_ShaderPair(pShader);

	newCyborgGun->SetObjMeshFromFile(L"Flintlock_pistol.obj", L"Media\\soldier\\", pRenderingDevice);

	newCyborgGun->Create(pRenderingDevice, pImmediateContext);
	newCyborgGun->SetScale(0.7, 0.7, 0.7);
	newCyborgGun->MoveTo(0, -1, 0);
	newCyborgGun->RotateTo(0, 0, 0);
	newCyborgGun->SetInitialDirection(0, 0, 1);

	// set new values for specific lighting variables 
    newCyborgGun->materialDiffuseColour = XMFLOAT4(1, 1, 1, 1);
    newCyborgGun->LightAmbientColour    = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.4f);
    newCyborgGun->lightDiffuseColour    = XMFLOAT4(1, 1, 1, 1);
    newCyborgGun->vecLightDirection     = XMVectorSet(2, -10, -13, 0);	//W unused.

	// Move to cyborg
	newCyborgGun->MoveTo(cyborg->x , cyborg->y, -cyborg->z);

	V(hr);

	return newCyborgGun;
}

HeightMap *NewFloor(ID3D11Device        *pRenderingDevice,
		     ID3D11DeviceContext *pImmediateContext)
{
	// load and store a new texture
	ID3D11ShaderResourceView* tempTexture;
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile( pRenderingDevice,  L"media\\grass.jpg",
									NULL, NULL, &tempTexture, NULL );

	//
	// create and allocate the shaders
	//
	Thing3DVS_PS_Pair *pShader = new Thing3DVS_PS_Pair();
	pShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Terrain_VS");

	pShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "Terrain_PS");

	pShader->CreateConstandBuffersAndSampler(pRenderingDevice);
	pShader->SetTexture(pImmediateContext, tempTexture, 1);

	//
	// Create the heightmap
	//
	HeightMap *heightMap = new HeightMap(pRenderingDevice, pImmediateContext);

	heightMap->SetVS_PS_ShaderPair(pShader);
	heightMap->HeightMapLoad("Media\\maps\\HeightMap.bmp");
	heightMap->LoadColourMap("Media\\maps\\HeightMapColour.bmp");
	heightMap->initScene(pRenderingDevice, pImmediateContext);

	// This is required to obtain additional position values (Y positions only). 
	// Without this, the player will go down mountains, and up pits (opposite to what
	// should happen). However, IF after loading up this application,
	// this is not required, simply comment it out, in addition to the
	// "heightmap->RotateBy(180" -- method call below...
	heightMap->SetInvertedMappings("Media\\maps\\HeightMapInverted.bmp");

	// set the scale
	heightMap->SetScale(1.0f, 1.0f, 1.0f);

	// set new values for specific lighting variables 
    heightMap->materialDiffuseColour = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.1f); // 0.5f
    heightMap->LightAmbientColour    = XMFLOAT4(0.02f, 0.02f, 0.02f, 0.02f); // 0.8f
    heightMap->lightDiffuseColour    = XMFLOAT4(0.8f, 0.8f, 0.8f, 0.8f); // 0.8f
    heightMap->vecLightDirection     = XMVectorSet(2, -10, -13, 0);

	// Rotation and movement - although movement is specified in Thing3DAbstract, I am
	// including it here too. 
	// The latter is required for viewing the surface. Or only the higher height positions
	// of the vertices are viewable.
	heightMap->MoveTo(0, 0, 0);
	heightMap->RotateBy(XMConvertToRadians(180), 0 , 0);

	V(hr);

	return heightMap;
}

void NewMonsters(ID3D11Device        *pRenderingDevice,
		     ID3D11DeviceContext *pImmediateContext)
{
	HRESULT hr = S_OK;

	// arrays of location information
	std::vector<XMFLOAT3> zombieLocations (10);

	zombieLocations[0].x = 227; zombieLocations[0].y = 0; zombieLocations[0].z = -70;
	zombieLocations[1].x = 205; zombieLocations[1].y = 0; zombieLocations[1].z = -91;
	zombieLocations[2].x = 153; zombieLocations[2].y = 0; zombieLocations[2].z = -98;
	zombieLocations[3].x = 121; zombieLocations[3].y = 0; zombieLocations[3].z = -106;
	zombieLocations[4].x = 114; zombieLocations[4].y = 0; zombieLocations[4].z = -300;
	zombieLocations[5].x = 89;  zombieLocations[5].y = 0; zombieLocations[5].z = -352;
	zombieLocations[6].x = 209; zombieLocations[6].y = 0; zombieLocations[6].z = -452;
	zombieLocations[7].x = 368; zombieLocations[7].y = 0; zombieLocations[7].z = -445;
	zombieLocations[8].x = 380; zombieLocations[8].y = 0; zombieLocations[8].z = -423;
	zombieLocations[9].x = 235; zombieLocations[9].y = 0; zombieLocations[9].z = -255;

	//
	// Create and set the shaders
	//
	Thing3DVS_PS_Pair *pixelShader = new Thing3DVS_PS_Pair();

	pixelShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_VS");

	pixelShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "ObjectL_PS");

	//pixelShader->SetTexture(pImmediateContext, pTexture, 1);
	pixelShader->CreateConstandBuffersAndSampler(pRenderingDevice);

	Object3D *zombieData = new Object3D(pRenderingDevice, pImmediateContext);
	zombieData->SetObjMeshFromFile(L"Slasher.obj", L"Media\\zombies\\necro\\", pRenderingDevice);

	for(int i = 0; i < zombieLocations.size(); i ++)
	{
		Object3D *zombie = new Object3D(pRenderingDevice, pImmediateContext);

		zombie->SetVS_PS_ShaderPair(pixelShader);
		zombie->g_meshData = zombieData->g_meshData; // transfer across building data (saves ALOT of time)
		zombie->Create(pRenderingDevice, pImmediateContext);
		zombie->SetScale(1.5, 1.5, 1.5);
		zombie->MoveTo(273, -3, -304); 
		zombie->RotateTo(0, 0, 0);

		// individual building data
		float zombieX = zombieLocations[i].x;
		float zombieZ = zombieLocations[i].z;

		zombie->MoveTo(zombieX, 0, zombieZ); 
		zombie->y = floorHeightMap->getHeight(zombieX, zombieZ) - 24.5f; // -25.5f for heightmap rotation difference
		zombie->RotateTo(0, 0, 0);

		// set new values for specific lighting variables 
		zombie->materialDiffuseColour = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.4f);
		zombie->LightAmbientColour    = XMFLOAT4(0.8f, 0.8f, 0.8f, 0.5f);
		zombie->lightDiffuseColour    = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		zombie->vecLightDirection     = XMVectorSet(2, -10, -13, 0);

		zombies.push_back(zombie);
	}

	delete zombieData;
	zombieLocations.clear();

	V(hr);
}

void NewBuildings(ID3D11Device *pRenderingDevice, 
				  ID3D11DeviceContext *pImmediateContext)
{
	HRESULT hr = S_OK;

	// arrays of location information
	std::vector<XMFLOAT3> buildingLocations (4);

	buildingLocations[0].x = 185; buildingLocations[0].y = 0; buildingLocations[0].z = -83;
	buildingLocations[1].x = 463; buildingLocations[1].y = 0; buildingLocations[1].z = -125;
	buildingLocations[2].x = 395; buildingLocations[2].y = 0; buildingLocations[2].z = -442;
	buildingLocations[3].x = 95; buildingLocations[3].y = 0; buildingLocations[3].z = -315;

	//
	// Create and set the shaders
	//
	Thing3DVS_PS_Pair *pixelShader = new Thing3DVS_PS_Pair();

	pixelShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_VS");

	pixelShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "ObjectL_PS");

	//pixelShader->SetTexture(pImmediateContext, pTexture, 1);
	pixelShader->CreateConstandBuffersAndSampler(pRenderingDevice);

	Object3D *buildingData = new Object3D(pRenderingDevice, pImmediateContext);
	buildingData->SetObjMeshFromFile(L"Barrack.obj", L"Media\\environment\\barrack\\", pRenderingDevice);

	for(int i = 0; i < buildingLocations.size(); i ++)
	{
		Object3D *building = new Object3D(pRenderingDevice, pImmediateContext);

		building->SetVS_PS_ShaderPair(pixelShader);
		building->g_meshData = buildingData->g_meshData; // transfer across building data (saves ALOT of time)
		building->Create(pRenderingDevice, pImmediateContext);
		building->SetScale(1, 1, 1);

		// individual building data
		float buildingX = buildingLocations[i].x;
		float buildingZ = buildingLocations[i].z;

		building->MoveTo(buildingX, 0, buildingZ); 
		building->y = floorHeightMap->getHeight(buildingX, buildingZ) - 25.5f; // -25.5f for heightmap rotation difference
		building->RotateTo(0, 0, 0);

		// set new values for specific lighting variables 
		building->materialDiffuseColour = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.4f);
		building->LightAmbientColour    = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.4f);
		building->lightDiffuseColour    = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		building->vecLightDirection     = XMVectorSet(2, -10, -13, 0);

		buildings.push_back(building);
	}

	delete buildingData;
	buildingLocations.clear();

	V(hr);
}

void NewResources(ID3D11Device        *pRenderingDevice,
		     ID3D11DeviceContext *pImmediateContext)
{
	HRESULT hr = S_OK;

	// arrays of location information
	std::vector<XMFLOAT3> resourceLocations (10);

	resourceLocations[0].x = 311; resourceLocations[0].y = 0; resourceLocations[0].z = -336;
	resourceLocations[1].x = 400; resourceLocations[1].y = 0; resourceLocations[1].z = -371;
	resourceLocations[2].x = 405; resourceLocations[2].y = 0; resourceLocations[2].z = -461;
	resourceLocations[3].x = 204; resourceLocations[3].y = 0; resourceLocations[3].z = -459;
	resourceLocations[4].x = 83;  resourceLocations[4].y = 0; resourceLocations[4].z = -352;
	resourceLocations[5].x = 139; resourceLocations[5].y = 0; resourceLocations[5].z = -165;
	resourceLocations[6].x = 158; resourceLocations[6].y = 0; resourceLocations[6].z = -75;
	resourceLocations[7].x = 222; resourceLocations[7].y = 0; resourceLocations[7].z = -61;
	resourceLocations[8].x = 321; resourceLocations[8].y = 0; resourceLocations[8].z = -89;
	resourceLocations[9].x = 409; resourceLocations[9].y = 0; resourceLocations[9].z = -36;

	//
	// Create and set the shaders
	//
	Thing3DVS_PS_Pair *pixelShader = new Thing3DVS_PS_Pair();

	pixelShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_VS");

	pixelShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "ObjectL_PS");

	//pixelShader->SetTexture(pImmediateContext, pTexture, 1);
	pixelShader->CreateConstandBuffersAndSampler(pRenderingDevice);

	Object3D *resourceData = new Object3D(pRenderingDevice, pImmediateContext);
	resourceData->SetObjMeshFromFile(L"resource_box.obj", L"Media\\environment\\resourcebox\\", pRenderingDevice);

	for(int i = 0; i < resourceLocations.size(); i ++)
	{
		Object3D *resource = new Object3D(pRenderingDevice, pImmediateContext);

		resource->SetVS_PS_ShaderPair(pixelShader);
		resource->g_meshData = resourceData->g_meshData; // transfer across building data (saves ALOT of time)
		resource->Create(pRenderingDevice, pImmediateContext);
		resource->SetScale(1.5, 1.5, 1.5);
		resource->MoveTo(273, -3, -304); 
		resource->RotateTo(0, 0, 0);

		// individual building data
		float resourceX = resourceLocations[i].x;
		float resourceZ = resourceLocations[i].z;

		resource->MoveTo(resourceX, 0, resourceZ); 
		resource->y = floorHeightMap->getHeight(resourceX, resourceZ) - 25.5f; // -25.5f for heightmap rotation difference
		resource->RotateTo(0, 0, 0);

		// set new values for specific lighting variables 
		resource->materialDiffuseColour = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.1f);
		resource->LightAmbientColour    = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
		resource->lightDiffuseColour    = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		resource->vecLightDirection     = XMVectorSet(2, -10, -13, 0);

		resources.push_back(resource);
	}

	delete resourceData;
	resourceLocations.clear();

	V(hr);
}

void NewBullets(ID3D11Device        *pRenderingDevice,
		     ID3D11DeviceContext *pImmediateContext)
{
	HRESULT hr = S_OK;

	//
	// Create and set the shaders
	//
	Thing3DVS_PS_Pair *pixelShader = new Thing3DVS_PS_Pair();

	pixelShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_VS");

	pixelShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "ObjectL_PS");

	//pixelShader->SetTexture(pImmediateContext, pTexture, 1);
	pixelShader->CreateConstandBuffersAndSampler(pRenderingDevice);

	Object3D *bulletData = new Object3D(pRenderingDevice, pImmediateContext);
	bulletData->SetObjMeshFromFile(L"bullet.obj", L"Media\\bullet\\", pRenderingDevice);

	for(int i = 0; i < 20; i++) // amount of bullets to store up
	{
		Object3D *bullet = new Object3D(pRenderingDevice, pImmediateContext);

		bullet->SetVS_PS_ShaderPair(pixelShader);
		bullet->g_meshData = bulletData->g_meshData; // transfer across building data (saves ALOT of time)
		bullet->Create(pRenderingDevice, pImmediateContext);
		bullet->SetScale(0.2, 0.2, 0.2);
		bullet->RotateTo(90, 90, 0);

		// set new values for specific lighting variables 
		bullet->materialDiffuseColour = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.1f);
		bullet->LightAmbientColour    = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
		bullet->lightDiffuseColour    = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		bullet->vecLightDirection     = XMVectorSet(2, -10, -13, 0);

		// change the speed
		bullet->speed = 0.10;

		// Push back to vector
		bulletStorage.push_back(bullet);
	}

	delete bulletData;

	//
	// Set the bullet information
	//
	bullets->SetBullets(bulletStorage);

	V(hr);
}


//
// Nigels code to cull objects based on the current vector direction
// of the cyborg (main player).
//
bool CheckVisible(Thing3DAbstract *toCheck)
{
	if (!g_b_culling) return true;
		else
		{
		XMVECTOR vecToThing = XMVectorSet ( toCheck->x - cyborg->x,
											toCheck->y - cyborg->y,
											toCheck->z - cyborg->z,
											0);						//unused.
		
		XMVECTOR vecThingDir = XMVector3Normalize(cyborg->currentVecDir);
		vecToThing           = XMVector3Normalize(vecToThing);
		
		
		//******************************************************************//
		// Weird, the  XMVector3Dot returns a scalar dot product with the	//
		// dot product between V1 and V2 is replicated into each component. //
		//******************************************************************//
		XMVECTOR  vecDot     = XMVector3Dot(vecThingDir, vecToThing);
		float     dotProduct = XMVectorGetX(vecDot);

		if (dotProduct > cos(g_f_cullingAngle)) return true; else return false;}
}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11DestroyDevice();
    g_SettingsDlg.OnD3D11DestroyDevice();
    DXUTGetGlobalResourceCache().OnDestroyDevice();
    SAFE_DELETE( g_pTxtHelper );
	
	cyborg->CleanUp();
	seaFloor->CleanUp();

	if(buildings.size() > 0)
	{
		buildings[0]->CleanUp();

		for(int i = 0; i < buildings.size(); i++)
			delete buildings[i];
	}

	if(zombies.size() > 0)
	{
		zombies[0]->CleanUp();

		for(int i = 0; i < zombies.size(); i++)
			delete zombies[i];
	}

	if(resources.size() > 0)
	{
		resources[0]->CleanUp();

		for(int i = 0; i < resources.size(); i++)
			delete resources[i];
	}

	if(bulletStorage.size() > 0)
	{
		bulletStorage[0]->CleanUp();

		for(int i = 0; i < bulletStorage.size(); i++)
			delete bulletStorage[i];
	}

	delete cyborg;
	delete cyborgGun;
	delete floorHeightMap;
	delete bullets;
}
