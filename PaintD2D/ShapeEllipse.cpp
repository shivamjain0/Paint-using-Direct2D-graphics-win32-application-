#include "ShapeEllipse.h"
#include <d2d1.h>

//void ShapeEllipse::OnDrawShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory ) {
//
//	float width = (bottomX - topX) / 2;
//	float height = (bottomY - topY) / 2;
//	float x1 = topX + width;
//	float y1 = topY + height;
//
//	curEllipse = D2D1::Ellipse(
//		D2D1::Point2F(x1, y1),
//		width,
//		height
//	);
//
//	pRenderTarget->DrawEllipse(curEllipse, pBrush, 1.0f, pStrokeStyle);
//
//}
//
//void ShapeEllipse::FillShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {
//
//	if (curPBrush != nullptr ) {
//		pRenderTarget->FillEllipse(curEllipse, curPBrush);
//	}
//}
//
//void ShapeEllipse::SetColor(ID2D1SolidColorBrush* curPBrush_) {
//	curPBrush = curPBrush_;
//}