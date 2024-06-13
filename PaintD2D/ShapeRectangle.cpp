//#include "ShapeRectangle.h"
#include"Shapes.h"
//void ShapeRectangle::OnDrawShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {
//	
//	D2D1_RECT_F rectangle1 = D2D1::RectF(
//		topX,
//		topY,
//		bottomX,
//		bottomY
//	);
//	pRenderTarget->DrawRectangle(&rectangle1, pBrush, 1.0f);
//}
//
//void ShapeRectangle::FillShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {
//
//	if (curPBrush != nullptr) {
//		pRenderTarget->FillRectangle(D2D1::RectF(topX, topY, bottomX, bottomY), curPBrush);
//	}
//}
//
//void ShapeRectangle::SetColor(ID2D1SolidColorBrush* curPBrush_) {
//	curPBrush = curPBrush_;
//}