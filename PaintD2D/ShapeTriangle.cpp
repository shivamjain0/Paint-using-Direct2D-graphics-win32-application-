//#include "ShapeTriangle.h"
//#include"ToolBox.h"
//#include"Shapes.h"
//#pragma comment(lib, "d2d1")
//
////template <class T>
////inline void SafeRelease(T** ppT)
////{
////	if (*ppT)
////	{
////		(*ppT)->Release();
////		*ppT = nullptr;
////	}
////}
//
//void ShapeTriangle::OnDrawShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {
//
//	float width = (bottomX - topX);
//	float height = (bottomY - topY);
//	float x1 = topX + width;
//	float y1 = topY + height;
//
//
//	D2D1_POINT_2F points[] =
//	{
//		D2D1::Point2F(topX + (width / 2), topY),
//		D2D1::Point2F(topX, topY + height),
//		D2D1::Point2F(bottomX, bottomY)
//	};
//
//	// Create a Direct2D path geometry
//	ID2D1PathGeometry* pGeometry = NULL;
//	HRESULT hr = pFactory->CreatePathGeometry(&pGeometry);
//	if (SUCCEEDED(hr))
//	{
//		// Open the geometry sink
//		ID2D1GeometrySink* pSink = NULL;
//		hr = pGeometry->Open(&pSink);
//		if (SUCCEEDED(hr))
//		{
//			// Begin the figure
//			pSink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
//
//			// Add the lines
//			pSink->AddLine(points[1]);
//			pSink->AddLine(points[2]);
//
//			// End the figure
//			pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
//
//			// Close the geometry sink
//			hr = pSink->Close();
//			SafeRelease(&pSink);
//		}
//
//		// Draw the triangle
//		if (SUCCEEDED(hr))
//		{
//			pRenderTarget->DrawGeometry(pGeometry, pBrush, 1.0f, NULL);
//			
//			// Update the curPGeometry
//			curPGeometry = pGeometry;
//		}
//
//		pGeometry = nullptr;
//	}
//}
//
//void ShapeTriangle::FillShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {
//
//	//D2D1_RECT_F rect = D2D1::RectF(topX, topY, bottomX, bottomY);
//	if (curPBrush != nullptr) {
//		pRenderTarget->FillGeometry(curPGeometry, curPBrush);
//	}
//}
//
//void ShapeTriangle::SetColor(ID2D1SolidColorBrush* curPBrush_) {
//	curPBrush = curPBrush_;
//}