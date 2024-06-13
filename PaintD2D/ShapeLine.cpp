#include <d2d1.h>
#include "ShapeLine.h"

//void ShapeLine::OnDrawShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {
//	pRenderTarget->DrawLine(D2D1::Point2F(topX,topY), D2D1::Point2F(bottomX, bottomY), pBrush,1.0f,NULL);
//}
//
//void ShapeLine::FillShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {
//
//	//D2D1_RECT_F rect = D2D1::RectF(topX, topY, bottomX, bottomY);
//	if (curPBrush != nullptr) {
//		pRenderTarget->DrawLine(D2D1::Point2F(topX, topY), D2D1::Point2F(bottomX, bottomY), curPBrush, 1.0f, NULL);
//	}
//}
//
//void ShapeLine::SetColor(ID2D1SolidColorBrush* curPBrush_) {
//	curPBrush = curPBrush_;
//}