#pragma once

#include "draft2/ShapeCapture.h"

#include <ee0/EditOP.h>

namespace draft2
{

class EditView;

class SelectShapeOP : public ee0::EditOP
{
public:
	SelectShapeOP(const std::shared_ptr<pt0::Camera>& cam, EditView& view,
		float capture_threshold);

	virtual bool OnMouseLeftDown(int x, int y) override;
	virtual bool OnMouseRightDown(int x, int y) override;
	virtual bool OnMouseMove(int x, int y) override;

	virtual bool OnDraw() const override;
	virtual bool Clear() override;

private:
	void SelectByPos(int x, int y);
	ShapeCapture::NodeRef QueryByPos(int x, int y) const;

protected:
	EditView& m_view;

	float m_capture_threshold = 0;

	ShapeCapture::NodeRef m_active;
	ShapeCapture::NodeRef m_hot;

}; // SelectShapeOP

}