#include "Shapes.h"
#include"ToolBox.h" // Fpr SafeRelease Function

void Shapes::ShowSelection(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {

	ID2D1StrokeStyle* pStrokeStyle = nullptr;
	float dashes[] = { 10.0f };
	D2D1_STROKE_STYLE_PROPERTIES strokeStyleProperties = D2D1::StrokeStyleProperties(
		D2D1_CAP_STYLE_FLAT,     // Dash cap style
		D2D1_CAP_STYLE_FLAT,     // Dash cap style
		D2D1_CAP_STYLE_ROUND,     // Dash cap style
		D2D1_LINE_JOIN_MITER,    // Line join style
		10.0f,// The miter limit.
		D2D1_DASH_STYLE_CUSTOM,// The dash style.
		0.0f // The dash offset.
	);

	HRESULT hr = pFactory->CreateStrokeStyle(strokeStyleProperties, dashes, ARRAYSIZE(dashes), &pStrokeStyle);

	D2D1_RECT_F rectangle1 = D2D1::RectF(
		this->topX,
		this->topY,
		this->bottomX,
		this->bottomY
	);
	pRenderTarget->DrawRectangle(&rectangle1, pBrush, 1.0f, pStrokeStyle);

}

// LINE Functions
void ShapeLine::OnDrawShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {
	pRenderTarget->DrawLine(D2D1::Point2F(topX, topY), D2D1::Point2F(bottomX, bottomY), pBrush, 1.0f, NULL);
}

void ShapeLine::FillShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {

	if (curPBrush != nullptr) {
		pRenderTarget->DrawLine(D2D1::Point2F(topX, topY), D2D1::Point2F(bottomX, bottomY), curPBrush, 1.0f, NULL);
	}
}

void ShapeLine::SetColor(ID2D1SolidColorBrush* curPBrush_) {
	curPBrush = curPBrush_;
}

Shapes* ShapeLine::clone()  {
	return new ShapeLine(*this); // Using copy constructor
}

// RECTANGLE Functions
void ShapeRectangle::OnDrawShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {

	D2D1_RECT_F rectangle1 = D2D1::RectF(
		topX,
		topY,
		bottomX,
		bottomY
	);
	pRenderTarget->DrawRectangle(&rectangle1, pBrush, 1.0f);
}

void ShapeRectangle::FillShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {

	if (curPBrush != nullptr) {
		pRenderTarget->FillRectangle(D2D1::RectF(topX, topY, bottomX, bottomY), curPBrush);
	}
}

void ShapeRectangle::SetColor(ID2D1SolidColorBrush* curPBrush_) {
	curPBrush = curPBrush_;
}

Shapes* ShapeRectangle::clone() {
	return new ShapeRectangle(*this); // Using copy constructor
}

// TRIANGLE Functions
void ShapeTriangle::OnDrawShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {

	float width = (bottomX - topX);
	float height = (bottomY - topY);
	float x1 = topX + width;
	float y1 = topY + height;


	D2D1_POINT_2F points[] =
	{
		D2D1::Point2F(topX + (width / 2), topY),
		D2D1::Point2F(topX, topY + height),
		D2D1::Point2F(bottomX, bottomY)
	};

	// Create a Direct2D path geometry
	ID2D1PathGeometry* pGeometry = NULL;
	HRESULT hr = pFactory->CreatePathGeometry(&pGeometry);
	if (SUCCEEDED(hr))
	{
		// Open the geometry sink
		ID2D1GeometrySink* pSink = NULL;
		hr = pGeometry->Open(&pSink);
		if (SUCCEEDED(hr))
		{
			// Begin the figure
			pSink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);

			// Add the lines
			pSink->AddLine(points[1]);
			pSink->AddLine(points[2]);

			// End the figure
			pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

			// Close the geometry sink
			hr = pSink->Close();
			SafeRelease(&pSink);
		}

		// Draw the triangle
		if (SUCCEEDED(hr))
		{
			pRenderTarget->DrawGeometry(pGeometry, pBrush, 1.0f, NULL);

			// Update the curPGeometry
			curPGeometry = pGeometry;
		}

		pGeometry = nullptr;
	}
}

void ShapeTriangle::FillShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {

	//D2D1_RECT_F rect = D2D1::RectF(topX, topY, bottomX, bottomY);
	if (curPBrush != nullptr) {
		pRenderTarget->FillGeometry(curPGeometry, curPBrush);
	}
}

void ShapeTriangle::SetColor(ID2D1SolidColorBrush* curPBrush_) {
	curPBrush = curPBrush_;
}

Shapes* ShapeTriangle::clone() {
	return new ShapeTriangle(*this); // Using copy constructor
}

// ELLIPSE Functions
void ShapeEllipse::OnDrawShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {

	float width = (bottomX - topX) / 2;
	float height = (bottomY - topY) / 2;
	float x1 = topX + width;
	float y1 = topY + height;

	curEllipse = D2D1::Ellipse(
		D2D1::Point2F(x1, y1),
		width,
		height
	);

	pRenderTarget->DrawEllipse(curEllipse, pBrush, 1.0f, pStrokeStyle);

}

void ShapeEllipse::FillShape(ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, ID2D1Factory* pFactory) {

	if (curPBrush != nullptr) {
		pRenderTarget->FillEllipse(curEllipse, curPBrush);
	}
}

void ShapeEllipse::SetColor(ID2D1SolidColorBrush* curPBrush_) {
	curPBrush = curPBrush_;
}

Shapes* ShapeEllipse::clone() {
	return new ShapeEllipse(*this); // Using copy constructor
}
