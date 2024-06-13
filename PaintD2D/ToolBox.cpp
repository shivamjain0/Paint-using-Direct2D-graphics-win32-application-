#include "ToolBox.h"


void PrintFloatDebugString_(float value) {
	char buffer[64];
	sprintf_s(buffer, "%f", value);
	OutputDebugStringA(buffer);
}

class DPIScale_
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

float DPIScale_::scaleX = 1.0f;
float DPIScale_::scaleY = 1.0f;

int ToolBox::createToolBox(int nCmdShow) {

	SetPositionOfToolBar();
	if (!Create(L"ToolBox", WS_EX_TOOLWINDOW | WS_CAPTION , 0, x, y, width, height, m_parentHWND))
	{
		return 0;
	}

	// SW_SHOW - Activates the window and displays it in its current size and position.
	ShowWindow(Window(), SW_SHOW);
	UpdateWindow(Window());

	return 0;
}

void ToolBox::setHWND(HWND parentHWND) {
	m_parentHWND = parentHWND;
}

void ToolBox::SetPositionOfToolBar() {

	RECT mainWindow;
	GetWindowRect(m_parentHWND, &mainWindow);

	//int mainWindowHeight = mainWindow.bottom - mainWindow.top;

	x = mainWindow.left,
		y = mainWindow.top + 50,
		width = 40,
		height = 410;
}

ID2D1Bitmap* ToolBox::LoadImage_(ID2D1RenderTarget* pRenderTarget, LPCWSTR filename) {

	auto it = imageCache.find(filename);
	if (it != imageCache.end()) { // If found
		return it->second; 
	}

	// If image not found, load it  
	ID2D1Bitmap* pBitmap = CreateBitmapFromFile(pRenderTarget, filename);

	// Save/Cache the loaded image
	if (pBitmap) {
		imageCache[filename] = pBitmap;
	}

	return pBitmap;
}

void ToolBox::DrawImage(ID2D1RenderTarget* pRenderTarget, ID2D1Bitmap* pBitmap, float x, float y) {
	if (pBitmap) {
		// Get the size of the bitmap
		D2D1_SIZE_F size = pBitmap->GetSize();

		// Define the destination rectangle (position and size)
		D2D1_RECT_F destRect = D2D1::RectF(x, y, x + 70, y + 40);

		// Draw the bitmap onto the render target
		pRenderTarget->DrawBitmap(pBitmap, destRect);
	}
}

ID2D1Bitmap* ToolBox::CreateBitmapFromFile(ID2D1RenderTarget* pRenderTarget, LPCWSTR filename) {
	IWICImagingFactory* pWICFactory = nullptr;
	IWICBitmapDecoder* pDecoder = nullptr;
	IWICBitmapFrameDecode* pFrame = nullptr;
	IWICFormatConverter* pConverter = nullptr;
	ID2D1Bitmap* pBitmap = nullptr;

	// Initialize WIC
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pWICFactory);
	if (SUCCEEDED(hr)) {
		// Create decoder for the image file
		hr = pWICFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);
		// DWORD -  Specifies the desired access to the file
		if (SUCCEEDED(hr)) {
			// Get the first frame of the image
			hr = pDecoder->GetFrame(0, &pFrame);
			if (SUCCEEDED(hr)) {
				// Create a WIC format converter
				hr = pWICFactory->CreateFormatConverter(&pConverter);
				if (SUCCEEDED(hr)) {
					// Initialize the format converter
					hr = pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);
					if (SUCCEEDED(hr)) {
						// Create a Direct2D bitmap
						hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, nullptr, &pBitmap);
					}
				}
			}
		}
	}

	SafeRelease(&pWICFactory);
	SafeRelease(&pDecoder);
	SafeRelease(&pFrame);
	SafeRelease(&pConverter);

	return pBitmap;
}

void ToolBox::OnPaint_()
{
	HRESULT hr = CreateGraphicsResources_();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(m_hwnd, &ps);

		pRenderTarget->BeginDraw();

		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		//----------------------------------------------------------------------------------------------------------------
		
		float margin = 0;
		for (int i = 0; i < shapesIcons.size(); i++) {

			pBitmap = LoadImage_(pRenderTarget, shapesIcons[i]);
			if (pBitmap) {
				// In RemoveShape icon y is 10.0f more downwards, To make UI look more precise
				if (i == shapesIcons.size() - 1) DrawImage(pRenderTarget, pBitmap, 25.0f, (40.0f*i) + margin + 5.0f);
				else DrawImage(pRenderTarget, pBitmap, 25.0f, margin +(40.0f*i));
			}
		}

		if (curMousePos.y < rectShownOnHover.top || curMousePos.y > rectShownOnHover.bottom) {

			for (int i = 0; i < 9; i++) {

				float margin = 0.0f;
				if (curMousePos.x > 5.0f && curMousePos.x < 115.0f &&
					curMousePos.y >(40.0f*i) && curMousePos.y < 40.0f + (40.0f*i) + margin) {

					if (i == 8) margin = 5.0f; // Last image is at more margin
					if (pRenderTarget != NULL) {

						rectShownOnHover = D2D1::RectF(
							5.0f, (40.0f*i) + margin,
							115.0f, 40.0f + (40.0f*i) + margin
						);

						pRenderTarget->DrawRectangle(&rectShownOnHover, pBrush, 1.0f);
						pRenderTarget->CreateSolidColorBrush(colorOnHover, &pBrush);
						pRenderTarget->FillRectangle(rectShownOnHover, pBrush);
					}
				}
			}
		}
		else if (curMousePos.y > rectShownOnHover.top && curMousePos.y < rectShownOnHover.bottom) {
			
			pRenderTarget->DrawRectangle(&rectShownOnHover, pBrush, 1.0f);
			pRenderTarget->CreateSolidColorBrush(colorOnHover, &pBrush);
			pRenderTarget->FillRectangle(rectShownOnHover, pBrush);
		}

		// To show the selected image 
		if (selectedShapeId != -1) {
			pRenderTarget->DrawRectangle(&curRect, pBrush, 1.0f);
			pRenderTarget->CreateSolidColorBrush(colorOnSelection, &pBrush);
			pRenderTarget->FillRectangle(&curRect, pBrush);
		}
		
		//----------------------------------------------------------------------------------------------------------------

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources_();
		}

		EndPaint(m_hwnd, &ps);
	}
}

void ToolBox::SetCurRect(int caseSelected) {
	caseSelected -= 10;
	float margin = 0.0f;
	if (caseSelected == 8) margin = 10.0f;
	curRect = { (float)5, (float)(40.0*caseSelected), (float)115, (float)(40.0 + (40.0*caseSelected) + margin) };
}

void  ToolBox::MouseHandling(int caseSelected) {

	selectedShapeId = -1;
	fillColor = FALSE;
	if (caseSelected != Paint || caseSelected != RemoveShapeColor || caseSelected != Delete)
	{
		selectedShapePtr = NULL;
		isShapeSelected = FALSE;
	}

	switch (caseSelected)
	{
	case RemoveShapeColor:

		selectedShapeId = paint;
		fillColor = TRUE;
		curColor = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);

		break;

	case Delete:
		selectedShapeId = deleteShape;
		break;

	case Paint:
		selectedShapeId = paint;
		fillColor = TRUE;
		curColor = color;

		break;

	case drawRectangle:
		selectedShapeId = rectangle;
		break;

	case drawLine:
		selectedShapeId = line;
		break;

	case drawEllipse:
		selectedShapeId = ellipse;
		break;

	case drawTriangle:
		selectedShapeId = triangle;
		break;

	case Select:
		shapeToDraw = NULL;
		selectedShapeId = select;
		break;

	case Choose_Palette:

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

			// Div by 255 - as rbg takes color values between 0-1
			color = D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f);
		}

		break;
	}
	SetCurRect(caseSelected);
}

LRESULT ToolBox::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	float curX = DPIScale_::PixelsToDipsX(GET_X_LPARAM(lParam));
	float curY = DPIScale_::PixelsToDipsY(GET_Y_LPARAM(lParam));
	int caseSelected = -1;

	switch (uMsg)
	{

		case WM_CREATE:
			
			if (FAILED(D2D1CreateFactory(
				D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory))) {
				return -1;  // Fail CreateWindowEx.
			}

			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = Window();
			break;

		case WM_CLOSE:
			ShowWindow(Window(), SW_HIDE);
			return 0;

		case WM_DESTROY:
			DiscardGraphicsResources_();
			SafeRelease(&pFactory);
			PostQuitMessage(0);
			break;

		case WM_PAINT:
			OnPaint_();
			ShowWindow(Window(), SW_SHOW);
			break;

		case WM_SIZE:
			break;

		case WM_LBUTTONDOWN:

			for (int i = 0; i < 9; i++) {
				float margin = 0.0f;
				if (i == 9) margin = 10.0f;
				if (curX >= 25.0f && curX <= 95.0f && curY > (40.0f*i) && curY <= 40.0f + (40.0f*i) + margin) {
					caseSelected = selectedCases[i];
					// casting double to float
					curRect = { (float)5, (float)(40.0*i), (float)115, (float)(40.0 + (40.0*i) + margin) };
				}
			}
			MouseHandling(caseSelected);
			break;

		case WM_MOUSEMOVE:
			TrackMouseEvent(&tme);
			break;

		case WM_SETCURSOR:
			GetCursorPos(&curMousePos);
			ScreenToClient(Window(), &curMousePos);
			OnPaint_();
			break;
		case WM_LBUTTONUP:
			break;

		case WM_MOUSELEAVE:
			curMousePos.x = (LONG)0.0f;
			curMousePos.y = (LONG)0.0f;
			InvalidateRect(Window(), NULL, FALSE);
			break;
		case WM_COMMAND:
			break;
	}
	return DefWindowProc(Window(), uMsg, wParam, lParam);
}