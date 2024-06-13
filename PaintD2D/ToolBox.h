#pragma once
#include <wincodec.h> // Include the Windows Imaging Component header
#include <windows.h>
#include <Windowsx.h>
#include<map>
#include"Resource.h"
#include <d2d1.h>
#include"Shapes.h"
#include"BaseWindow.h"

#define drawRectangle 10
#define drawLine 11
#define drawEllipse 12
#define drawTriangle 13
#define Paint 14
#define Choose_Palette 15
#define Select 16
#define Delete 17
#define RemoveShapeColor 18

#define rectangle 1
#define line 2
#define ellipse 3
#define triangle 4
#define select 5
#define paint 6
#define deleteShape 7

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

class ToolBox : public BaseWindow<ToolBox> {

public:

	ToolBox() : shapeToDraw(NULL), selectedShapePtr(NULL) {
		isShapeSelected = FALSE;
		fillColor = FALSE;
		selectedShapeId = -1;

		for( int i = 0; i<9; i++ ) 
			selectedCases[i]  = 10+i;

		curRect = { 0,0,0,0 };
		colorOnHover = D2D1::ColorF(0.0f, 1.0f, 1.0f, 0.5); // Light blue for hovering
		colorOnSelection = D2D1::ColorF(0.0f, 0.6f, 0.9f, 0.5); // blue for selection;
	}

	PCWSTR  ClassName() const { return L"ToolBox"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	int createToolBox(int nCmdShow);

	void setHWND(HWND parentHWND);

	HWND GetHwnd() { return Window(); }

	//void Get_HandleWM_COMMAND(Shapes* shapeToDraw, Shapes* selectedShapePtr, 
	//BOOL &isShapeSelected, BOOL &fillColor,D2D1_COLOR_F &color, int &selectedShapeId);

	void SetPositionOfToolBar();

	ID2D1Bitmap* LoadImage_(ID2D1RenderTarget* pRenderTarget, LPCWSTR filename);

	void DrawImage(ID2D1RenderTarget* pRenderTarget, ID2D1Bitmap* pBitmap, float x, float y);

	ID2D1Bitmap* CreateBitmapFromFile(ID2D1RenderTarget* pRenderTarget, LPCWSTR filename);

	void OnPaint_();

	HRESULT CreateGraphicsResources_()
	{
		HRESULT hr = S_OK;
		if (pRenderTarget == NULL)
		{
			RECT rc;
			GetClientRect(m_hwnd, &rc);

			D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

			hr = pFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(m_hwnd, size),
				&pRenderTarget);

			if (SUCCEEDED(hr))
			{
				const D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0); // Transparent color
				hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);
			}
		}
		return hr;
	}

	void DiscardGraphicsResources_()
	{
		SafeRelease(&pRenderTarget);
		SafeRelease(&pBrush);
	}

	void MouseHandling(int);
	void SetCurRect(int);

	int x, y, width, height;
	HWND m_parentHWND;

	Shapes *shapeToDraw;
	Shapes *selectedShapePtr;
	BOOL isShapeSelected;
	BOOL fillColor;
	D2D1_COLOR_F color;
	D2D1_COLOR_F curColor;
	int selectedShapeId;
	int selectedCases[9];
	vector<LPCWSTR> shapesIcons = { L"rectangle.bmp" ,L"line-ico.ico" ,L"ellipse 1.BMP" , L"triangle-ico.ico",
								L"paint-bucket.png" , L"brush-ico.ico", L"select.ico" , L"dustbin-ico 1.ico", L"RemoveColor.png" };

	map<LPCWSTR, ID2D1Bitmap*> imageCache;

	POINT curMousePos;
	D2D1_RECT_F rectShownOnHover;
	D2D1_RECT_F curRect; // Update the curRect to show color for selection in case of keyboard key press
	D2D1_COLOR_F colorOnHover;
	D2D1_COLOR_F colorOnSelection;

	ID2D1HwndRenderTarget   *pRenderTarget;
	ID2D1Bitmap*             pBitmap;
	ID2D1SolidColorBrush    *pBrush;
	ID2D1Factory            *pFactory;

	TRACKMOUSEEVENT tme; /* Set mouse event dwflag to TM_LEAVE and trackMouseEvent is called in WM_MOUSEMOVE
						    Now, WM_MOUSELEAVE msg is received whenever mouse is leaving the window */
};

