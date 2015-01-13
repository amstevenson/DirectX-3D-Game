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
#include "FlyingThing3D.h"
#include "Thing3D.h"
#include "Thing3DVS_PS_Pair.h"
#include "Tile.h"
#include "Object3D.h"
#include <vector>		//Standard lib, no ".h?

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
FlyingThing3D	*thing;
Thing3D			*seaFloor;
Thing3D         *grassFloor;
Thing3D         *island;

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

//**************************************************************************//
// A horrible syntax to create a list (vector) of pointers to tiles, with   //
// an initial size of 0.													//
//																			//
// As it is a list of Thing3DAbstract, then we can put any child of			//
// Thing3DAbstract into it.													//
//**************************************************************************//
std::vector <Thing3DAbstract *> bearsAndZebras(0);	
std::vector <Thing3DAbstract *> tiles(0);		
std::vector <Thing3DAbstract *> objModels(0);	


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

void InitApp();
void RenderText();
FlyingThing3D *NewFlyingThing(ID3D11Device *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);
void NewHarbour(ID3D11Device      *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);
Thing3D *NewLand(ID3D11Device  *pRenderingDevice, ID3D11DeviceContext *pImmediateContext, char type[]);
Thing3D *NewIsland(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3dImmediateContext);
void NewCup(ID3D11Device        *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);
void NewThings(ID3D11Device        *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);
void NewBearsAndZebras(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3dImmediateContext);


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
    DXUTCreateWindow( L"Flying Tiger Island Adventure - Adam Stevenson" );

    // Only require 10-level hardware, change to D3D_FEATURE_LEVEL_11_0 to require 11-class hardware
    // Switch to D3D_FEATURE_LEVEL_9_x for 10level9 hardware
    DXUTCreateDevice( D3D_FEATURE_LEVEL_10_0, true, 640, 480 );

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
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, iY, 170, 22 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 0, iY += iYo, 170, 22, VK_F3 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 0, iY += iYo, 170, 22, VK_F2 );

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

	if(g_b_f1ArrowDown == false)
		g_pTxtHelper->DrawTextLine( L"Press F1 for controls");
	else 
	{
		g_pTxtHelper->DrawTextLine( L"Space bar: start flying the tiger upwards");
		g_pTxtHelper->DrawTextLine( L" X button: zoom out");
		g_pTxtHelper->DrawTextLine( L" Z button: zoom in");
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


	thing = NewFlyingThing(pd3dDevice, pd3dImmediateContext);
	NewHarbour(pd3dDevice,  pd3dImmediateContext);
	seaFloor    = NewLand( pd3dDevice, pd3dImmediateContext, "sea");
	grassFloor  = NewLand( pd3dDevice, pd3dImmediateContext, "grass");
	island		= NewIsland(pd3dDevice, pd3dImmediateContext);

	NewBearsAndZebras(pd3dDevice, pd3dImmediateContext);
	NewCup(pd3dDevice, pd3dImmediateContext);
	//**************************************************************************//
	// Initialize the projection matrix.  Generally you will only want to create//
	// this matrix once and then forget it.	I think we should rebuild this		//
	// matrix if we re-size the swap chain, though.								//
	//**************************************************************************//
	g_MatProjection = XMMatrixPerspectiveFovLH( XM_PIDIV2,				// Field of view (pi / 2 radians, or 90 degrees
											 g_width / (FLOAT) g_height, // Aspect ratio.
											 0.01f,						// Near clipping plane.
											 100.0f );					// Far clipping plane.


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
	At = XMVectorSet(thing->x, thing->y + thing->height, thing->z, 0); // add the things height (for user experience whilst flying)

	//
	// Set the things current direction vector
	//
	XMVECTOR localDir = thing->currentVecDir;

	localDir *= -3;		// 3 paces back

	//
	// Obtain the camera position
	//
	Eye = XMVectorSet(thing->x + XMVectorGetX(localDir),
					  thing->y + XMVectorGetY(localDir) + 2 + thing->cameraZoom,
					  thing->z + XMVectorGetZ(localDir),
					  0);

	g_matView= XMMatrixLookAtLH( Eye,	// The eye, or viewer's position.
								 At,	// The look at point.
								 Up );	// Which way is up.

	//
	// Render the main player - the flying thing
	//
	thing->matView       = g_matView;
	thing->matProjection = g_MatProjection;
	thing->Render(pd3dDevice, pd3dImmediateContext);
	
	//
	// The sky follows the main player around - I may place it here later on
	//

	//****************************************************************************//
	//							Render the objects								  //
	//****************************************************************************//
	//
	// Render the (very small) ocean/sea
	//
	seaFloor->matView = g_matView;
	seaFloor->matProjection = g_MatProjection;
	seaFloor->Render(pd3dDevice, pd3dImmediateContext);

	//
	// Render the grass
	//
	grassFloor->matView = g_matView;
	grassFloor->matProjection = g_MatProjection;
	grassFloor->Render(pd3dDevice, pd3dImmediateContext);

	//
	// Render the island
	//
	island->matView = g_matView;
	island->matProjection = g_MatProjection;
	island->Render(pd3dDevice, pd3dImmediateContext);

	//
	// models that have been produced from .obj files
	// at the moment I have chosen to use sdkmesh's, but the loader from the tutorials has been adapated
	// and put into an object loader - and this section deals with displaying those (although the render
	// method for the objects doesnt work fully yet, but displays an object of sorts. Without a sort of specific colour scheme. 
	//
	int numObjects = objModels.size();

	if(numObjects > 0)
	{
		Object3D *pTobj = (Object3D *) objModels[0];

		for (int i = 0; i < numObjects; i++)
		{
			Object3D *pTobj = (Object3D *) objModels[i];
			if (i == 0) pTobj->buffersAndtexturesSet = false; 
			else
						pTobj->buffersAndtexturesSet = true;

			pTobj->matView       = g_matView;
			pTobj->matProjection = g_MatProjection;
			pTobj->Render(pd3dDevice, pd3dImmediateContext);
		}
	}


	//
	// Render Nigels animals
	//
	int numBears = bearsAndZebras.size();
	for (int i = 0; i < numBears; i++)
	{
		Thing3D *pAnimal       = (Thing3D *) bearsAndZebras[i];
		pAnimal->matView       = g_matView;
		pAnimal->matProjection = g_MatProjection;
		
		pAnimal->Render(pd3dDevice, pd3dImmediateContext);
	}

	//
	// Render the harbour tiles - need to add in ships later on once my object loader works fully. 
	//
	Tile *pTtile = (Tile *) tiles[0];
	pd3dImmediateContext->PSSetShaderResources(1, 1, &pTtile->pThing3DVS_PS_Pair->pTextureResourceView);

	int numTiles = (int) tiles.size();
	for (int i = 0; i < numTiles; i++)
	{
		pTtile = (Tile *) tiles[i];
		if (i == 0) pTtile->buffersAndtexturesSet = false; 
		else
					pTtile->buffersAndtexturesSet = true;

		pTtile->matView       = g_matView;
		pTtile->matProjection = g_MatProjection;
		pTtile->Render(pd3dDevice, pd3dImmediateContext);

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



//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11ReleasingSwapChain();
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
	
	// delete the vertex/pixel shaders
	tiles[0]->CleanUp();
	objModels[0]->CleanUp();
	bearsAndZebras[0]->CleanUp();
	thing->CleanUp();
	grassFloor->CleanUp();
	seaFloor->CleanUp();
	island->CleanUp();

	// delete arrays
	for(int i = 1; i < tiles.size(); i++)
		delete tiles[i];
	
	
	for(int i = 0; i < objModels.size(); i++)
		delete objModels[i];

	for(int i = 0; i < bearsAndZebras.size(); i++)
		delete bearsAndZebras[i];

	delete thing;
	//delete seaFloor;	  //-- block still valid error and illegal heap accesses...
						  //related to variables still being in use upon deletion.
	//delete grassFloor;
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


//**************************************************************************//
// Handle updates to the scene.  This is called regardless of which D3D		//
// API is used (we are only using Dx11).									//
//**************************************************************************//
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	if (g_b_LeftArrowDown)  thing->ry -= fElapsedTime*3.0f;
	if (g_b_RightArrowDown) thing->ry += fElapsedTime*3.0f;
	if (g_b_UpArrowDown)    thing->speed += fElapsedTime*4.0f;
	if (g_b_DownArrowDown)
	{
		//
		// Increase the speed
		//
		thing->speed  -= fElapsedTime*4.0f;

		//
		// Increate the height
		//
		if(thing->height > 0)
		{
			thing->height -= fElapsedTime*2.0f;
			thing->cameraZoom -=fElapsedTime*2.0f;
		}
	}

	if (g_b_SpaceArrowDown) 
	{
		if(thing->height > 0) thing->cameraZoom += fElapsedTime*2.0f;
		thing->height += fElapsedTime*2.0f;
	}

	//
	// If Z or X, zoom the camera in or out. 
	//
	if (g_b_xArrowDown) thing->cameraZoom += fElapsedTime*3.0f;
	if (g_b_zArrowDown) thing->cameraZoom -= fElapsedTime*3.0f;
	
	//**********************************************************************//
	// Get the thing to do its thing, as it were.							//
	//**********************************************************************//
	thing->frameTime = fElapsedTime;
	thing->DoBehaviour(); 


	//**********************************************************************//
	// And now get the tiles to do their thing.								//
	//**********************************************************************//
	int numTiles = (int) tiles.size();
	for (int i = 0; i < numTiles; i++)
	{
		Tile *tile = (Tile *) tiles[i];
		
		if (tile->IntersectsWith(thing)) 
		{
			tile->doEffect               = true;
			tile->materialDiffuseColour.x = 0;
		}
		tile->DoBehaviour();
	}
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
	//HRESULT hr;

    switch( nChar )
    {

		//******************************************************************//
		// Nigel code to rotate and move the tiger.							//
		//******************************************************************//
		case VK_LEFT:  g_b_LeftArrowDown  = bKeyDown; break;
		case VK_RIGHT: g_b_RightArrowDown = bKeyDown; break;
		case VK_UP:    g_b_UpArrowDown    = bKeyDown; break;
		case VK_DOWN:  g_b_DownArrowDown  = bKeyDown; break;
		case VK_SPACE: g_b_SpaceArrowDown = bKeyDown; break;
		case 'Z': g_b_zArrowDown = bKeyDown; break;
		case 'X': g_b_xArrowDown = bKeyDown; break;
		case VK_F1 :  g_b_f1ArrowDown = bKeyDown; break;
    }
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


//**************************************************************************//
// Create a new thing and plug the shader effect and various other stuff	//
// into it.																	//
//**************************************************************************//
FlyingThing3D *NewFlyingThing(ID3D11Device        *pRenderingDevice,
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
							   "Thing3D_Textured_PS");

	pShader->CreateConstandBuffersAndSampler(pRenderingDevice);



	FlyingThing3D *newFlyingThing = new FlyingThing3D(pRenderingDevice, pImmediateContext);
	newFlyingThing->SetVS_PS_ShaderPair(pShader);

	//
	// 'create flying thing' adds two wings to a mesh
	// although for this implementation so far, it also has the sky following it
	// may need to address this later on. 
	//
	newFlyingThing->CreateFlyingThing(pRenderingDevice, pImmediateContext, L"Media\\Tiger\\tiger.sdkmesh", 0);
	newFlyingThing->SetInitialDirection(0, 0, -1);
	newFlyingThing->SetScale(1, 1, 1);
	newFlyingThing->MoveTo(0, 0.65f, 0);
	newFlyingThing->RotateTo(0, XMConvertToRadians(180), 0);

	return newFlyingThing;
}

Thing3D *NewLand(ID3D11Device        *pRenderingDevice,
		 	      ID3D11DeviceContext *pImmediateContext,
				  char type[])
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

	//
	// Create an object based on what type has been passed to this method
	// the .bmp that is associated with the folder where the sdkmesh is located, determines the look
	//
	if(type == "sea")
	{
		// The link for the water image: h ttp://www.seodesign.us/photoshop-patterns/45-free-water-photoshop-patterns-textures/
		newLand->CreateMesh(pRenderingDevice, pImmediateContext, L"Media\\floor\\sea\\seafloor.sdkmesh", 0);
		newLand->SetInitialDirection(0, 0, -1);
		newLand->SetScale(2, 2, 2);
		newLand->MoveTo(0, 13, 0);
		newLand->RotateTo(0, XMConvertToRadians(180), 0);
	}
	else if(type == "grass")
	{
		// The link for the grass image: h ttp://naldzgraphics.net/freebies/30-free-high-resolution-grass-textures/
		newLand->CreateMesh(pRenderingDevice, pImmediateContext, L"Media\\floor\\grass\\seafloor.sdkmesh", 0);
		newLand->SetInitialDirection(0, 0, -1);
		newLand->SetScale(0.5, 0.5, 0.5);
		newLand->MoveTo(0, 1.7, 0);
		newLand->RotateTo(0, XMConvertToRadians(180), 0);
	}
	else // in case, for some reason, the above two are not entered...
	{
		newLand->CreateMesh(pRenderingDevice, pImmediateContext, L"Media\\floor\\sea\\seafloor.sdkmesh", 0);
		newLand->SetInitialDirection(0, 0, -1);
		newLand->SetScale(2, 2, 2);
		newLand->MoveTo(0, 14, 0);
		newLand->RotateTo(0, XMConvertToRadians(180), 0);
	}


	return newLand;
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

void NewCup(ID3D11Device        *pRenderingDevice,
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
							   "FlatThing3D_PS");

	pixelShader->CreateConstandBuffersAndSampler(pRenderingDevice);

	Object3D *object3D = new Object3D(pRenderingDevice, pImmediateContext);
	
	object3D->SetVS_PS_ShaderPair(pixelShader);
	object3D->SetObjMeshFromFile("Media\\cup\\cup.obj", pRenderingDevice); // pass the file that contains the object (.obj).
	object3D->Create(pRenderingDevice, pImmediateContext);
	object3D->SetScale(2, 2, 2);
	object3D->MoveTo(0, -0.3, 5); // move to an easilly seen area (testing purposes)
	object3D->RotateTo(0, 0, 0);

	objModels.push_back(object3D);
	
}

//**************************************************************************//
// Create a new road.														//
//**************************************************************************//
void NewHarbour(ID3D11Device        *pRenderingDevice,
		     ID3D11DeviceContext *pImmediateContext)
{
	HRESULT hr;

	//
	// create a texture
	//
	ID3D11ShaderResourceView *pTexture;

	hr = D3DX11CreateShaderResourceViewFromFile( pRenderingDevice, 
												 //L"Media\\floor\\sea\\seafloor.bmp", 
												 L"Media\\woodtexture.jpg",
												 NULL, NULL, 
												 &pTexture,			// This is returned.
												 NULL );

	//
	// create and allocate the shaders
	//
	Thing3DVS_PS_Pair *pShader = new Thing3DVS_PS_Pair();
	pShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_VS");

	pShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "FlatThing3D_PS");

 
	pShader->CreateConstandBuffersAndSampler(pRenderingDevice);
	pShader->SetTexture(pImmediateContext, pTexture, 1);

	int tileArray[30][30] = {
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
							};
			

	for (int x = 0; x < 30; x++)
		for (int z = 0; z < 30; z++)
			if (tileArray[x][z] == 1)
			{
				Tile *pTile = new Tile(pRenderingDevice, pImmediateContext);
				pTile->SetVS_PS_ShaderPair(pShader);
				pTile->Create(pRenderingDevice, pImmediateContext);
				pTile->SetScale(1, 1, 1);
				pTile->MoveTo((float)x*2-20, -3, (float)z*2+25);
				pTile->RotateTo(0, 0, 0);

				tiles.push_back(pTile);

			}
}


void NewBearsAndZebras(ID3D11Device        *pRenderingDevice,
		     ID3D11DeviceContext *pImmediateContext)
{
	//HRESULT hr;

	//
	// create and allocate the shaders
	//
	Thing3DVS_PS_Pair *pShader = new Thing3DVS_PS_Pair();
	pShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_VS");

	pShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "Thing3D_PS");

 
	pShader->CreateConstandBuffersAndSampler(pRenderingDevice);


 
	int animalArray[30][30] = {
								{1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,1},
								{3,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1},
								{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,1,2,1,2,1,2,1,2,1,2},
								{3,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{1,1,1,1,1,3,0,0,2,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{2,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{1,0,0,0,0,3,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1,3,1,2,1,2,1,3,1,3},
								{2,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
								{1,0,0,0,0,2,0,0,2,0,0,0,0,0,0,1,0,0,0,0,1,3,1,3,1,3,1,3,1,2},
								{2,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
								{1,1,1,3,1,3,1,3,1,3,1,2,1,2,1,2,1,2,1,2,1,3,1,3,1,2,1,2,1,1},
								{2,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{2,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
								{2,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
								{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
								{3,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
								{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
								{3,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
								{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
								{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
								{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
								{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
								{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
								{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
								{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
								{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
								{1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
							};

		
		
	for (int x = 0; x < 30; x++)
		for (int z = 0; z < 30; z++)
			{
				if (animalArray[x][z] == 2)
				{
					Thing3D *pAnimal = new Thing3D(pRenderingDevice, pImmediateContext);
					pAnimal->SetVS_PS_ShaderPair(pShader);
					pAnimal->CreateMesh(pRenderingDevice, pImmediateContext, L"Media\\lizard\\lizardSIG.sdkmesh", 0);
					pAnimal->SetScale(0.1, 0.1, 0.1);
					pAnimal->MoveTo((float)x*2-40, -3, (float)z*-25);
					pAnimal->RotateTo(0, 0, 0);

					bearsAndZebras.push_back(pAnimal);
				}
				if (animalArray[x][z] == 3)
				{
					Thing3D *pAnimal = new Thing3D(pRenderingDevice, pImmediateContext);
					pAnimal->SetVS_PS_ShaderPair(pShader);
					pAnimal->CreateMesh(pRenderingDevice, pImmediateContext, L"Media\\Zebra\\Zebra.sdkmesh", 0);
					pAnimal->SetScale(1, 1, 1);
					pAnimal->MoveTo((float)x*2-40, -3, (float)z*2-25);
					pAnimal->RotateTo(0, 0, 0);

					bearsAndZebras.push_back(pAnimal);
				}
		}

}