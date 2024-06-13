#pragma once
#include <windows.h>  // Including core functionalities such as Window management, Message handling, inlcude basic Data types etc
#include <Windowsx.h> // Handling common tasks such as retrieving mouse coordinates and keyboard inputs
#include <d2d1.h>
#include <list>
#include <memory>

#pragma comment(lib, "d2d1")

#include"BaseWindow.h"
#include "resource.h"
#include"ToolBox.h"
#include "Shapes.h" 


class MainWindow : public BaseWindow<MainWindow>
{

	HCURSOR                 hCursor;
	HCURSOR					hresizeCursorWE;
	HCURSOR					hresizeCursorNS;
	HCURSOR					hresizeCursorNWSE;
	HCURSOR					hresizeCursorNESW;
	HCURSOR					hresizeCursorSIZEALL;

	ID2D1Factory            *pFactory;
	ID2D1HwndRenderTarget   *pRenderTarget;
	ID2D1SolidColorBrush    *pBrush;
	BOOL                    onDrawing;
	vector<Shapes*>         shapes;
	float                   xOnSelection, yOnSelection;
	ToolBox                 toolBoxObj;
	HMENU                   viewSubMenu;
	Shapes					*cutSelectedShape;
	Shapes					*copySelectedShape;
	int						selectedShapeIndex;
	BOOL					toPaste; // To check if we do have any shape to paste or not


	HRESULT CreateGraphicsResources();
	void    DiscardGraphicsResources();
	void    OnPaint();
	void    Resize();
	void    OnLButtonDown(int pixelX, int pixelY, DWORD flags);
	void    OnLButtonUp(int pixelX, int pixelY, DWORD flags);
	void    OnMouseMove(int pixelX, int pixelY, DWORD flags);
	void    ShowSelectedShape(float topX, float topY);
	void    ToMoveSelectedShape(int pixelX, int pixelY, DWORD flags);
	void    ResizeSelectedShape(int pixelX, int pixelY, DWORD flags);
	void    SetProrityOfSelectedShape(int);
	void    DeleteShape(float, float);
	void    addMenu(HWND);

public:


	MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL)
	{
		onDrawing = FALSE;

		hCursor = LoadCursor(NULL, IDC_ARROW);
		hresizeCursorWE = LoadCursor(NULL, IDC_SIZEWE);
		hresizeCursorNS = LoadCursor(NULL, IDC_SIZENS);
		hresizeCursorNWSE = LoadCursor(NULL, IDC_SIZENWSE);
		hresizeCursorNESW = LoadCursor(NULL, IDC_SIZENESW);
		hresizeCursorSIZEALL = LoadCursor(NULL, IDC_SIZEALL);
	}

	PCWSTR  ClassName() const { return L"Circle Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

};

