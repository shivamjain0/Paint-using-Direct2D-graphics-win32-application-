#pragma once
#include <d2d1.h>
#include<vector>
#include "resource.h"
#pragma comment(lib, "d2d1")
using namespace std;

class Shapes
{
public:

	float topX;
	float topY;
	float bottomX;
	float bottomY;

	ID2D1StrokeStyle* pStrokeStyle = nullptr;
	ID2D1SolidColorBrush* curPBrush = nullptr;

	Shapes() {
		topX ;
		topY;
		bottomX;
		bottomY;
	}

	virtual void OnDrawShape(ID2D1HwndRenderTarget *pRenderTarget,ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory = nullptr) = 0;
	
	void ShowSelection(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory);

	virtual void FillShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) = 0;

	virtual void SetColor(ID2D1SolidColorBrush* curPBrush_) = 0;

	virtual Shapes* clone() = 0;
};

class ShapeLine : public Shapes
{
public:
	virtual void OnDrawShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory = nullptr);
	void FillShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory);
	void SetColor(ID2D1SolidColorBrush* curPBrush_);
	Shapes* clone();
};

class ShapeTriangle :public Shapes
{
	ID2D1PathGeometry* curPGeometry = NULL;

	virtual void OnDrawShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory = nullptr);
	void FillShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory);
	void SetColor(ID2D1SolidColorBrush* curPBrush_);
	Shapes* clone();
};

class ShapeRectangle :public Shapes
{
public:

	ShapeRectangle() {}

	virtual void OnDrawShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory = nullptr);
	void FillShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory);
	void SetColor(ID2D1SolidColorBrush* curPBrush_);
	Shapes* clone();
};

class ShapeEllipse : public Shapes
{

	D2D1_ELLIPSE curEllipse;

	void OnDrawShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory = nullptr);
	void FillShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory);
	void SetColor(ID2D1SolidColorBrush* curPBrush_);
	Shapes* clone();
};