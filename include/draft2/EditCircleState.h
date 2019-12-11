#pragma once

#include "draft2/ShapeCapture.h"

#include <ee0/EditOpState.h>

#include <SM_Vector.h>

#include <functional>

namespace gs { class Shape2D; }

namespace draft2
{

class EditView;

class EditCircleState : public ee0::EditOpState
{
public:
	EditCircleState(const std::shared_ptr<pt0::Camera>& camera, EditView& view,
		std::function<ShapeCapture::NodeRef()> get_selected);

	virtual bool OnMousePress(int x, int y) override;
	virtual bool OnMouseRelease(int x, int y) override;
	virtual bool OnMouseDrag(int x, int y) override;

	virtual bool OnDraw() const override;
	virtual bool Clear() override;

private:
	void UpdateCirclePos(const sm::vec2& pos);

private:
	EditView& m_view;

	std::function<ShapeCapture::NodeRef()> m_get_selected;

	sm::vec2 m_first_pos;
	sm::vec2 m_curr_pos;

	ShapeCapture::NodeRef m_selected;

}; // EditCircleState

}