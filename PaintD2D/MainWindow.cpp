
#include"BaseWindow.h"
#include "resource.h"
#include"MainWindow.h"
#include<tchar.h>

#include <commctrl.h>

#define ShowToolBox 25
#define open_existing_project 26
#define New_Project 27
#define Clear_Screen 28
#define Close_Project 29
#define ID_HELP 30

#define rectangle 1
#define line 2
#define ellipse 3
#define triangle 4
#define select 5
#define paint 6
#define deleteShape 7

void PrintFloatDebugString(float value) {
	char buffer[64];
	sprintf_s(buffer, "%f", value);
	OutputDebugStringA(buffer);
}

class DPIScale
{
	static float scaleX;
	static float scaleY;

public:
	static void Initialize(ID2D1Factory *pFactory)
	{
		FLOAT dpiX, dpiY;
		pFactory->GetDesktopDpi(&dpiX, &dpiY);
		scaleX = dpiX / 96.0f;
		scaleY = dpiY / 96.0f;
	}

	template <typename T>
	static float PixelsToDipsX(T x)
	{
		return static_cast<float>(x) / scaleX;
	}

	template <typename T>
	static float PixelsToDipsY(T y)
	{
		return static_cast<float>(y) / scaleY;
	}
};

float DPIScale::scaleX = 1.0f;
float DPIScale::scaleY = 1.0f;

HRESULT MainWindow::CreateGraphicsResources()
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
			const D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);
		}
	}
	return hr;
}

void MainWindow::DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBrush);
}

void MainWindow::OnPaint()
{
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);

		pRenderTarget->BeginDraw();

		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		for (int i = 0; i < shapes.size(); i++) {
			shapes[i]->OnDrawShape(pRenderTarget, pBrush, pFactory);
			shapes[i]->FillShape(pRenderTarget, pBrush, pFactory);
		}
		//----------------------------------------------------------------------------------------------------------------

		// Show selection for selected shape
		if (toolBoxObj.selectedShapePtr != NULL)
			(toolBoxObj.selectedShapePtr)->ShowSelection(pRenderTarget, pBrush, pFactory);

		if (toolBoxObj.shapeToDraw != NULL)
			toolBoxObj.shapeToDraw->OnDrawShape(pRenderTarget, pBrush, pFactory);

		//----------------------------------------------------------------------------------------------------------------

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);
	}
}

void MainWindow::Resize()
{
	if (pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		pRenderTarget->Resize(size);
	}
}

void MainWindow::SetProrityOfSelectedShape(int j) {

	Shapes *selectedShape = shapes[j];

	// Do not iterate it for last index - Update last index after the loop ends
	for (int i = j; i < shapes.size() - 1; i++)
		shapes[i] = shapes[i + 1];

	shapes[shapes.size() - 1] = selectedShape;
}

// Edited
void MainWindow::ShowSelectedShape(float topX, float topY) {

	float selectedShapePt1 = topX;
	float selectedShapePt2 = topY;

	toolBoxObj.selectedShapePtr = NULL;

	for (int i = (int)(shapes.size()) - 1; i >= 0; i--) {

		float minX = shapes[i]->topX,
			minY = shapes[i]->topY,
			maxX = shapes[i]->bottomX,
			maxY = shapes[i]->bottomY;

		if (minX > maxX) {
			minX = shapes[i]->bottomX,
				maxX = shapes[i]->topX;
		}

		if (minY > maxY) {
			minY = shapes[i]->bottomY,
				maxY = shapes[i]->topY;
		}

		if (selectedShapePt1 >= minX && selectedShapePt1 <= maxX && selectedShapePt2 >= minY && selectedShapePt2 <= maxY) {

			if (pRenderTarget != NULL && pBrush != NULL) {

				toolBoxObj.isShapeSelected = TRUE;
				this->toolBoxObj.selectedShapePtr = shapes[i];

				// Storing the points pos during selection ( used for drag/resize )
				xOnSelection = topX, yOnSelection = topY;

				// Setting Prority of Selected Shape
				SetProrityOfSelectedShape(i);

				selectedShapeIndex = i;

				OutputDebugString(L"X - ");
				PrintFloatDebugString(xOnSelection);
				OutputDebugString(L"  Y - ");
				PrintFloatDebugString(yOnSelection);
				OutputDebugString(L"\n");
				break;
			}
			else {
				toolBoxObj.isShapeSelected = FALSE;
				toolBoxObj.selectedShapePtr = NULL;
			}
		}
	}
}

void MainWindow::DeleteShape(float curXPos, float curYPos) {

	for (int i = 0; i < (int)(shapes.size()); i++) {

		float minX = shapes[i]->topX,
			minY = shapes[i]->topY,
			maxX = shapes[i]->bottomX,
			maxY = shapes[i]->bottomY;

		if (minX > maxX) {
			minX = shapes[i]->bottomX,
				maxX = shapes[i]->topX;
		}

		if (minY > maxY) {
			minY = shapes[i]->bottomY,
				maxY = shapes[i]->topY;
		}

		if (curXPos >= minX && curXPos <= maxX && curYPos >= minY && curYPos <= maxY) {

			if (pRenderTarget != NULL && pBrush != NULL) {

				auto it = shapes.begin();
				shapes.erase(it + i);
				break;
			}
		}
	}
}

void MainWindow::OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{

	float topX = DPIScale::PixelsToDipsX(pixelX);
	float topY = DPIScale::PixelsToDipsY(pixelY);

	if (toolBoxObj.selectedShapeId == rectangle)
		toolBoxObj.shapeToDraw = new ShapeRectangle();

	else if (toolBoxObj.selectedShapeId == line)
		toolBoxObj.shapeToDraw = new ShapeLine();

	else if (toolBoxObj.selectedShapeId == ellipse)
		toolBoxObj.shapeToDraw = new ShapeEllipse();

	else if (toolBoxObj.selectedShapeId == triangle)
		toolBoxObj.shapeToDraw = new ShapeTriangle();

	// Case for selection of any shape
	else if (toolBoxObj.selectedShapeId == select) {

		onDrawing = FALSE;
		//toolBoxObj.isShapeSelected = TRUE;
		ShowSelectedShape(topX, topY);
	}

	// To fill color in the Figure
	else if (toolBoxObj.selectedShapeId == paint) {
		if (toolBoxObj.fillColor && toolBoxObj.selectedShapePtr != NULL) {

			ID2D1SolidColorBrush* pBrush1;
			pRenderTarget->CreateSolidColorBrush(toolBoxObj.curColor, &pBrush1);

			toolBoxObj.selectedShapePtr->SetColor(pBrush1);
		}
	}

	else if (toolBoxObj.selectedShapeId == deleteShape) {
		DeleteShape(topX, topY);
		InvalidateRect(m_hwnd, NULL, FALSE);
	}

	if (toolBoxObj.shapeToDraw != NULL) {

		toolBoxObj.shapeToDraw->topX = topX;
		toolBoxObj.shapeToDraw->topY = topY;
		toolBoxObj.shapeToDraw->bottomX = topX;
		toolBoxObj.shapeToDraw->bottomY = topY;

		toolBoxObj.selectedShapePtr = toolBoxObj.shapeToDraw;
	}
}

void MainWindow::OnLButtonUp(int pixelX, int pixelY, DWORD flags) {

	if (toolBoxObj.shapeToDraw != NULL && (toolBoxObj.shapeToDraw->topX != toolBoxObj.shapeToDraw->bottomX &&
		toolBoxObj.shapeToDraw->topY != toolBoxObj.shapeToDraw->bottomY))
	{
		shapes.push_back(toolBoxObj.shapeToDraw);
	}

	if (onDrawing) {
		toolBoxObj.selectedShapePtr = NULL;
	}

	toolBoxObj.selectedShapePtr = NULL;
	toolBoxObj.isShapeSelected = FALSE;
	ReleaseCapture();
}

void MainWindow::OnMouseMove(int pixelX, int pixelY, DWORD flags) {

	float bottomX = DPIScale::PixelsToDipsX(pixelX);
	float bottomY = DPIScale::PixelsToDipsY(pixelY);

	toolBoxObj.shapeToDraw->bottomX = bottomX;
	toolBoxObj.shapeToDraw->bottomY = bottomY;

	// So that cursor not goes out of the window when we are drawing any shape
	if (toolBoxObj.shapeToDraw != NULL) {

		float xPosMove = bottomX - xOnSelection,
			yPosMove = bottomY - yOnSelection;

		float topX = toolBoxObj.shapeToDraw->topX + xPosMove,
			topY = toolBoxObj.shapeToDraw->topY + yPosMove,
			bottomX = toolBoxObj.shapeToDraw->bottomX + xPosMove,
			bottomY = toolBoxObj.shapeToDraw->bottomY + yPosMove;

		POINT cursorPos;
		GetCursorPos(&cursorPos);
		// Figure and cursor should not out of the window
		RECT windowRect;
		GetClientRect(m_hwnd, &windowRect);

		if (topX < windowRect.left || topY < windowRect.top || bottomX > windowRect.right || bottomY > windowRect.bottom) {
			xOnSelection = bottomX;
			yOnSelection = bottomY;

			// Diff of 1 to stop cursor to do resizing when reached at corners/sides of the figure
			POINT topLeft = { windowRect.left + 1, windowRect.top + 1 };
			POINT bottomRight = { windowRect.right - 1, windowRect.bottom - 1 };
			ClientToScreen(m_hwnd, &topLeft);
			ClientToScreen(m_hwnd, &bottomRight);

			RECT clipRect = { topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };

			// Set the cursor's clipping region to the window area
			ClipCursor(&clipRect);
		}
	}
	
	InvalidateRect(m_hwnd, NULL, FALSE);
}

void MainWindow::ToMoveSelectedShape(int pixelX, int pixelY, DWORD flags) {

	float curX = DPIScale::PixelsToDipsX(pixelX);
	float curY = DPIScale::PixelsToDipsY(pixelY);

	float xPosMove = curX - xOnSelection,
		yPosMove = curY - yOnSelection;

	float topX = toolBoxObj.selectedShapePtr->topX + xPosMove,
		topY = toolBoxObj.selectedShapePtr->topY + yPosMove,
		bottomX = toolBoxObj.selectedShapePtr->bottomX + xPosMove,
		bottomY = toolBoxObj.selectedShapePtr->bottomY + yPosMove;


	// Figure and cursor should not out of the window
	RECT windowRect;
	GetClientRect(m_hwnd, &windowRect);
	if (topX < windowRect.left || topY < windowRect.top || bottomX > windowRect.right || bottomY > windowRect.bottom) {

		xOnSelection = curX;
		yOnSelection = curY;
		 
		// Diff of 1 to stop cursor to do resizing when reached at corners/sides of the figure
		POINT topLeft = { windowRect.left+1, windowRect.top+1 };
		POINT bottomRight = { windowRect.right-1, windowRect.bottom-1 };
		ClientToScreen(m_hwnd, &topLeft);
		ClientToScreen(m_hwnd, &bottomRight);

		RECT clipRect = { topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };

		// Set the cursor's clipping region to the window area
		ClipCursor(&clipRect);
		InvalidateRect(m_hwnd, NULL, FALSE);
		return;
	}

	toolBoxObj.selectedShapePtr->topX += xPosMove;
	toolBoxObj.selectedShapePtr->topY += yPosMove;
	toolBoxObj.selectedShapePtr->bottomX += xPosMove;
	toolBoxObj.selectedShapePtr->bottomY += yPosMove;

	xOnSelection = curX;
	yOnSelection = curY;

	InvalidateRect(m_hwnd, NULL, FALSE);
}

void MainWindow::ResizeSelectedShape(int pixelX, int pixelY, DWORD flags) {

	float updatedX = DPIScale::PixelsToDipsX(pixelX);
	float updatedY = DPIScale::PixelsToDipsY(pixelY);

	// Left side of rectangle
	if (xOnSelection == toolBoxObj.selectedShapePtr->topX) {

		float xPosDiff = updatedX - xOnSelection;
		toolBoxObj.selectedShapePtr->topX += xPosDiff;

		xOnSelection = updatedX;
	}
	// Right side of rectangle
	if (xOnSelection == toolBoxObj.selectedShapePtr->bottomX) {

		float xPosDiff = updatedX - xOnSelection;
		toolBoxObj.selectedShapePtr->bottomX += xPosDiff;

		xOnSelection = updatedX;
	}
	// Top side of rectangle
	if (yOnSelection == toolBoxObj.selectedShapePtr->topY) {

		float yPosDiff = updatedY - yOnSelection;
		toolBoxObj.selectedShapePtr->topY += yPosDiff;

		yOnSelection = updatedY;
	}
	// Bottom side of rectangle
	if (yOnSelection == toolBoxObj.selectedShapePtr->bottomY) {

		float yPosDiff = updatedY - yOnSelection;
		toolBoxObj.selectedShapePtr->bottomY += yPosDiff;

		yOnSelection = updatedY;
	}

	// When opposite sides interchanged, then interchaning their values

	if (toolBoxObj.selectedShapePtr->bottomX < toolBoxObj.selectedShapePtr->topX) {
		float temp = toolBoxObj.selectedShapePtr->bottomX;
		toolBoxObj.selectedShapePtr->bottomX = toolBoxObj.selectedShapePtr->topX;
		toolBoxObj.selectedShapePtr->topX = temp;
	}
	if (toolBoxObj.selectedShapePtr->bottomY < toolBoxObj.selectedShapePtr->topY) {
		float temp = toolBoxObj.selectedShapePtr->bottomY;
		toolBoxObj.selectedShapePtr->bottomY = toolBoxObj.selectedShapePtr->topY;
		toolBoxObj.selectedShapePtr->topY = temp;
	}
}

void MainWindow::addMenu(HWND m_hwnd) {

	HMENU hMenu = CreateMenu();

	HMENU hSubMenu = CreatePopupMenu(); // Create submenu for "File" menu
	viewSubMenu = CreatePopupMenu(); // Create submenu for "View" menu
	HMENU helpSubMenu = CreatePopupMenu(); // Create submenu for "HELP" menu

	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"File");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)viewSubMenu, L"View");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)helpSubMenu, L"Help");

	AppendMenu(hSubMenu, MF_STRING, New_Project, L"New Project");
	AppendMenu(hSubMenu, MF_STRING, open_existing_project, L"Open");
	AppendMenu(hSubMenu, MF_STRING, Clear_Screen, L"Clear Screen");
	AppendMenu(hSubMenu, MF_STRING, Close_Project, L"Exit");

	AppendMenu(viewSubMenu, MF_STRING, ShowToolBox, L"Close ToolBox");
	AppendMenu(helpSubMenu, MF_STRING, ID_HELP, L"Keyboard Shortcuts");

	SetMenu(m_hwnd, hMenu);

	// Add tooltips to the menu items
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	ZeroMemory(&mii, sizeof(mii));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_ID | MIIM_TYPE;
	mii.fType = MFT_STRING;

	// Set tooltips for File submenu items
	mii.wID = New_Project;
	mii.dwTypeData = (LPWSTR)L"Create a New Project";
	SetMenuItemInfo(hSubMenu, New_Project, FALSE, &mii);

	mii.wID = open_existing_project;
	mii.dwTypeData = (LPWSTR)L"Open an Existing Project";
	SetMenuItemInfo(hSubMenu, open_existing_project, FALSE, &mii);

	mii.wID = Clear_Screen;
	mii.dwTypeData = (LPWSTR)L"Clear the Screen";
	SetMenuItemInfo(hSubMenu, Clear_Screen, FALSE, &mii);

	mii.wID = Close_Project;
	mii.dwTypeData = (LPWSTR)L"Exit the Application";
	SetMenuItemInfo(hSubMenu, Close_Project, FALSE, &mii);

	// Set tooltips for View submenu items
	mii.wID = ShowToolBox;
	mii.dwTypeData = (LPWSTR)L"Close the ToolBox";
	SetMenuItemInfo(viewSubMenu, ShowToolBox, FALSE, &mii);

	// Set tooltips for Help submenu items
	mii.wID = ID_HELP;
	mii.dwTypeData = (LPWSTR)L"View Keyboard Shortcuts";
	SetMenuItemInfo(helpSubMenu, ID_HELP, FALSE, &mii);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	MainWindow win;

	if (!win.Create(L"Paint Application in Win32 (Direct2D)", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);
	UpdateWindow(win.Window());

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

//BOOL isNMousePress = FALSE;

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	static HWND hwndTT = NULL;
	static HWND hwndStatus = NULL;
	static RECT rcClient;
	static TOOLINFO ti = {};
	INT widths[] = { 120, 240, 360, -1 };

	switch (uMsg)
	{
	case WM_CREATE:

		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory))) {
			return -1;  // Fail CreateWindowEx.
		}
		DPIScale::Initialize(pFactory);

		toolBoxObj.setHWND(Window());
		toolBoxObj.createToolBox(SW_SHOW);

		addMenu(m_hwnd);

		break;

	case WM_DESTROY:
		DiscardGraphicsResources();
		SafeRelease(&pFactory);
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		OnPaint();
		break;

	case WM_CONTEXTMENU: // A context menu, also known as a popup menu or a right - click menu
	{
		if (toolBoxObj.selectedShapePtr != NULL || toPaste) {

			// Get the mouse position
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(m_hwnd, &pt);

			if (toPaste || (pt.x >= toolBoxObj.selectedShapePtr->topX && pt.x <= toolBoxObj.selectedShapePtr->bottomX &&
				            pt.y >= toolBoxObj.selectedShapePtr->topY && pt.y <= toolBoxObj.selectedShapePtr->bottomY)) {

				HMENU hMenu = CreatePopupMenu();
				AppendMenu(hMenu, MF_STRING, ID_CUT, L"Cut");
				AppendMenu(hMenu, MF_STRING, ID_COPY, L"Copy");
				AppendMenu(hMenu, MF_STRING, ID_PASTE, L"Paste");
				AppendMenu(hMenu, MF_STRING, ID_FILL_COLOR, L"Fill Color");
				AppendMenu(hMenu, MF_STRING, ID_CHOOSE_COLOR, L"Choose Color");


				GetCursorPos(&pt);
				// Show the context menu
				TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, m_hwnd, NULL);

				// Cleanup
				DestroyMenu(hMenu);
			}
		}
	}
	break;

	case WM_SIZE:

		Resize();
		InvalidateRect(m_hwnd, NULL, FALSE);
		break;
		/*
	case WM_NCLBUTTONDOWN:
		isNMousePress = TRUE;
		break;

	case WM_NCLBUTTONUP:
		isNMousePress = FALSE;

		if (toolBoxObj.GetHwnd() != nullptr) {

			toolBoxObj.SetPositionOfToolBar();
			// If window pos is changed, then it will repositon the ToolBox position
			//SetWindowPos(toolBoxObj.GetHwnd(), NULL, toolBoxObj.x, toolBoxObj.y, toolBoxObj.width, toolBoxObj.height, SWP_NOSIZE | SWP_NOZORDER);

			if (!IsWindowVisible(toolBoxObj.GetHwnd())) {
				ShowWindow(toolBoxObj.GetHwnd(), SW_SHOW);
				ModifyMenu(viewSubMenu, 0, MF_BYPOSITION | MF_STRING, ShowToolBox, L"Close ToolBox"); // Change text of the menu item
			}
		}
		else {
			toolBoxObj.setHWND(Window());
			toolBoxObj.createToolBox(SW_SHOW);
		}


		break;

	case WM_NCMOUSEMOVE:
		if (isNMousePress) {
			if (toolBoxObj.GetHwnd() != nullptr) {
				if (IsWindowVisible(toolBoxObj.GetHwnd())) {
					ShowWindow(toolBoxObj.GetHwnd(), SW_HIDE);
					ModifyMenu(viewSubMenu, 0, MF_BYPOSITION | MF_STRING, ShowToolBox, L"Show ToolBox"); // Change text of the menu item
				}
			}
			else {
				toolBoxObj.setHWND(Window());
				toolBoxObj.createToolBox(SW_SHOW);
			}
		}
		break;
*/

	case WM_GETMINMAXINFO:
	{
		int RECT_WIDTH = 1000, RECT_HEIGHT = 500;
		MINMAXINFO* minMaxInfo = reinterpret_cast<MINMAXINFO*>(lParam);

		minMaxInfo->ptMinTrackSize.x = RECT_WIDTH;
		minMaxInfo->ptMinTrackSize.y = RECT_HEIGHT;

		return 0;
	}

	case WM_LBUTTONDOWN:

		// While drawing , figures to not go outside the window
		// Set Ondrawing TRUE
		if (toolBoxObj.selectedShapeId == ellipse || toolBoxObj.selectedShapeId == line || toolBoxObj.selectedShapeId == rectangle
			|| toolBoxObj.selectedShapeId == triangle) {

			xOnSelection = (float)GET_X_LPARAM(lParam);
			yOnSelection = (float)GET_Y_LPARAM(lParam);

			onDrawing = TRUE;
		}


		// While filling color - Make selection also
		if (toolBoxObj.selectedShapeId == paint) {

			toolBoxObj.selectedShapeId = select;
			OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
			toolBoxObj.selectedShapeId = paint;
			toolBoxObj.isShapeSelected = FALSE;
		}

		OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		InvalidateRect(m_hwnd, NULL, FALSE);
		break;

	case WM_LBUTTONUP:

		// To let cursor move anywhere (clipped only to particular area when we try to take my figure out of our window)
		ClipCursor(nullptr); 

		if (onDrawing) {
			OnLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
			InvalidateRect(m_hwnd, NULL, FALSE);
		}

		if (toolBoxObj.isShapeSelected) {
			// On selection - while mouse is up, it has to show its selection
			ShowSelectedShape((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
		}

		if (!toolBoxObj.fillColor)
			toolBoxObj.isShapeSelected = FALSE;
		onDrawing = FALSE;

		// -----------------------------------------------------------------------------------------------------------------------------
		toolBoxObj.shapeToDraw = NULL;
		// -----------------------------------------------------------------------------------------------------------------------------

		InvalidateRect(m_hwnd, NULL, FALSE);
		break;

	case WM_MOUSEMOVE:

		if (toolBoxObj.shapeToDraw != NULL && onDrawing)
			OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);

		if (toolBoxObj.isShapeSelected == TRUE) {

			if (xOnSelection == toolBoxObj.selectedShapePtr->topX || xOnSelection == toolBoxObj.selectedShapePtr->bottomX ||
				yOnSelection == toolBoxObj.selectedShapePtr->topY || yOnSelection == toolBoxObj.selectedShapePtr->bottomY)
				ResizeSelectedShape(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
			else
				ToMoveSelectedShape(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);

			InvalidateRect(m_hwnd, NULL, FALSE);
		}
		break;

	case WM_SETCURSOR:

		// Cursor change only in case of selection/resize 
		// If there is a point object - Consider it as null

		if (toolBoxObj.selectedShapePtr != NULL && toolBoxObj.fillColor == FALSE &&
			((toolBoxObj.selectedShapePtr->topX != toolBoxObj.selectedShapePtr->bottomX) &&
			(toolBoxObj.selectedShapePtr->topY != toolBoxObj.selectedShapePtr->bottomY))) {

			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(m_hwnd, &pt);

			// Check if the mouse is within the region of the image
			RECT selectedShapeRect = { (LONG)toolBoxObj.selectedShapePtr->topX, (LONG)toolBoxObj.selectedShapePtr->topY, (LONG)toolBoxObj.selectedShapePtr->bottomX, (LONG)toolBoxObj.selectedShapePtr->bottomY };
			int height = selectedShapeRect.bottom - selectedShapeRect.top,
				width = selectedShapeRect.right - selectedShapeRect.left;

			if ((pt.x == selectedShapeRect.left && pt.y == selectedShapeRect.top) ||
				(pt.x == selectedShapeRect.right && pt.y == selectedShapeRect.bottom)) {
				SetCursor(hresizeCursorNWSE);
			}
			else if ((pt.x == selectedShapeRect.right && pt.y == selectedShapeRect.top) ||
				(pt.x == selectedShapeRect.left && pt.y == selectedShapeRect.bottom)) {
				SetCursor(hresizeCursorNESW);
			}
			else if (pt.x == selectedShapeRect.left || pt.x == selectedShapeRect.right) { SetCursor(hresizeCursorWE); }

			// The double-pointed resize arrow pointing up and down.
			else if (pt.y == selectedShapeRect.top || pt.y == selectedShapeRect.bottom) { SetCursor(hresizeCursorNS); }

			// Point is inside the rectangle
			else if (PtInRect(&selectedShapeRect, pt) || (pt.x > selectedShapeRect.left && pt.x < selectedShapeRect.right
				&& pt.y < selectedShapeRect.top && pt.y > selectedShapeRect.bottom))
			{
				SetCursor(hresizeCursorSIZEALL);
			}
			else
			{
				SetCursor(hCursor);
			}

			return TRUE;
		}
		else SetCursor(hCursor);

		break;

	case WM_KEYDOWN:
		switch (LOWORD(wParam)) {

		case 0x4C: // L
			toolBoxObj.MouseHandling(drawLine);
			break;

		case 0x52: // R
			if (GetKeyState(VK_CONTROL) < 0) toolBoxObj.MouseHandling(RemoveShapeColor);
			else toolBoxObj.MouseHandling(drawRectangle);
			break;

		case 0x45: // E
			toolBoxObj.MouseHandling(drawEllipse);
			break;

		case 0x54: // T
			toolBoxObj.MouseHandling(drawTriangle);
			break;

		case 0x50: // P
			if (GetKeyState(VK_CONTROL) < 0) toolBoxObj.MouseHandling(Choose_Palette);
			else toolBoxObj.MouseHandling(Paint);
			break;

		case 0x53: // S
			toolBoxObj.MouseHandling(Select);
			break;

		case 0x2E: // DEL key
			toolBoxObj.MouseHandling(Delete);
			break;
		}
		toolBoxObj.OnPaint_();
		break;

	case WM_COMMAND:

		toolBoxObj.selectedShapeId = -1;
		toolBoxObj.fillColor = FALSE;

		switch (LOWORD(wParam))
		{

		case ShowToolBox:

			if (toolBoxObj.GetHwnd() != nullptr) {


				// If window pos is changed, then it will repositon the ToolBox position
				if (!IsWindowVisible(toolBoxObj.GetHwnd())) {

					toolBoxObj.SetPositionOfToolBar();
					SetWindowPos(toolBoxObj.GetHwnd(), NULL, toolBoxObj.x, toolBoxObj.y, toolBoxObj.width, toolBoxObj.height, SWP_NOSIZE | SWP_NOZORDER);

					ShowWindow(toolBoxObj.GetHwnd(), SW_SHOW);
					ModifyMenu(viewSubMenu, 0, MF_BYPOSITION | MF_STRING, ShowToolBox, L"Close ToolBox"); // Change text of the menu item
				}
				else {
					ShowWindow(toolBoxObj.GetHwnd(), SW_HIDE);
					ModifyMenu(viewSubMenu, 0, MF_BYPOSITION | MF_STRING, ShowToolBox, L"Show ToolBox"); // Change text of the menu item
				}
			}
			else {
				toolBoxObj.setHWND(Window());
				toolBoxObj.createToolBox(SW_SHOW);
			}

			break;

		case Clear_Screen:
			/*toolBoxObj.isShapeSelected = FALSE;
			toolBoxObj.fillColor = FALSE;
			toolBoxObj.selectedShapeId = -1;
			toolBoxObj.shapeToDraw = NULL;*/
			toolBoxObj.selectedShapePtr = NULL;
			shapes.clear();
			break;

		case Close_Project:
			DiscardGraphicsResources();
			SafeRelease(&pFactory);
			PostQuitMessage(0);
			break;

		case ID_HELP:

			MessageBox(m_hwnd,          // Handler - The window over which messagebox is show
				_T("KEYBOARD SHORTCUTS\n\n\n"
					"PRESS    TO SELECT\n\n"
					"L   ->     LINE \n"
					"R   ->     RECTANGLE \n"
					"E   ->     ELLIPSE \n"
					"P   ->     PAINT BUCKET \n"
					"S   ->     SELECT \n\n"
					"DEL         ->     DELETE FIGURE \n"
					"CTRL+P   ->     COLOR PALLETE\n"
					"CTRL+R   ->     REMOVE COLOR "),// Text in the messagebox
				_T("Controls"),       // Title of the message box
				MB_OK | MB_ICONINFORMATION); 

			break;

		case ID_CUT:
			if (toolBoxObj.selectedShapePtr) {

				cutSelectedShape = toolBoxObj.selectedShapePtr;
				vector<Shapes*>::iterator it = shapes.begin();
				shapes.erase(it + selectedShapeIndex); // Its value if updated in ShowSelectedShape() method
				toolBoxObj.selectedShapePtr = NULL;

				copySelectedShape = NULL;
				toPaste = TRUE;
			}
			else {
				if (cutSelectedShape != NULL)
					shapes.push_back(cutSelectedShape);  
				toPaste = FALSE;
			}
			break;

		case ID_COPY:
			if (toolBoxObj.selectedShapePtr) {

				copySelectedShape = (*(toolBoxObj.selectedShapePtr)).clone();
				cutSelectedShape = NULL;
				toPaste = TRUE;
			}
			else {
				if (cutSelectedShape != NULL)
					shapes.push_back(cutSelectedShape);
				toPaste = FALSE;
			}
			break;

		case ID_PASTE:

			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(m_hwnd, &pt);
			if (copySelectedShape != NULL) {

				float height = copySelectedShape->bottomY - copySelectedShape->topY,
					width = copySelectedShape->bottomX - copySelectedShape->topX;
				
				copySelectedShape->topX = (float)pt.x;
				copySelectedShape->topY = (float)pt.y;
				copySelectedShape->bottomX = pt.x + width;
				copySelectedShape->bottomY = pt.y + height;

				// If on Paste - figure is going out of the client screem
				RECT rect;
				GetClientRect(m_hwnd, &rect);
				if (rect.bottom < copySelectedShape->bottomY) {
					copySelectedShape->topY = (float)rect.bottom - height;
					copySelectedShape->bottomY = (float)rect.bottom;
				}
				if (rect.right < copySelectedShape->bottomX) {
					copySelectedShape->topX = (float)rect.right - width;
					copySelectedShape->bottomX = (float)rect.right;
				}
				
				shapes.push_back((*copySelectedShape).clone());
			}
			else if (cutSelectedShape != NULL) {

				float height = cutSelectedShape->bottomY - cutSelectedShape->topY,
					width = cutSelectedShape->bottomX - cutSelectedShape->topX;
				
				cutSelectedShape->topX = (float)pt.x;
				cutSelectedShape->topY = (float)pt.y;
				cutSelectedShape->bottomX = pt.x + width;
				cutSelectedShape->bottomY = pt.y + height;

				// If on Paste - figure is going out of the client screem
				RECT rect;
				GetClientRect(m_hwnd, &rect);
				if (rect.bottom < cutSelectedShape->bottomY) {
					cutSelectedShape->topY = (float)rect.bottom - height;
					cutSelectedShape->bottomY = (float)rect.bottom;
				}
				if (rect.right < cutSelectedShape->bottomX) {
					cutSelectedShape->topX = (float)rect.right-width;
					cutSelectedShape->bottomX = (float)rect.right;
				}

				shapes.push_back(cutSelectedShape);
				cutSelectedShape = NULL;

				toPaste = FALSE;
			}
			break;

		case ID_FILL_COLOR:
			if (toolBoxObj.selectedShapePtr) {

				// Fill Color
				ID2D1SolidColorBrush* pBrush1;
				pRenderTarget->CreateSolidColorBrush(toolBoxObj.curColor, &pBrush1);

				toolBoxObj.selectedShapePtr->SetColor(pBrush1);
			}
			break;

		case ID_CHOOSE_COLOR:
			if (toolBoxObj.selectedShapePtr) {

				CHOOSECOLOR cc = { 0 };
				cc.lStructSize = sizeof(CHOOSECOLOR);
				cc.hwndOwner = m_hwnd;
				cc.lpCustColors = new COLORREF[16];
				cc.Flags = CC_FULLOPEN | CC_RGBINIT;

				// On pressing OK button - this condition becomes true
				if (ChooseColor(&cc)) {
					float r = GetRValue(cc.rgbResult);
					float g = GetGValue(cc.rgbResult);
					float b = GetBValue(cc.rgbResult);

					// D2D1 color format (0.0-1.0) 
					toolBoxObj.curColor = D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f);
				}

				// Fill Color
				ID2D1SolidColorBrush* pBrush1;
				pRenderTarget->CreateSolidColorBrush(toolBoxObj.curColor, &pBrush1);

				toolBoxObj.selectedShapePtr->SetColor(pBrush1);

			}
			break;
		}
		InvalidateRect(m_hwnd, NULL, FALSE);
		break;
	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}