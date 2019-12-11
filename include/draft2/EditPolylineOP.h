#pragma once

#include "draft2/ShapeCapture.h"

#include <ee0/EditOP.h>

#include <SM_Vector.h>

#include <vector>
#include <functional>

namespace draft2
{

class EditPolylineOP : public ee0::EditOP
{
public:
	EditPolylineOP(const std::shared_ptr<pt0::Camera>& cam, EditView& view,
		std::function<ShapeCapture::NodeRef()> get_selected, bool is_closed);

	virtual bool OnMouseLeftDown(int x, int y) override;
	virtual bool OnMouseRightDown(int x, int y) override;
	virtual bool OnMouseMove(int x, int y) override;
	virtual bool OnMouseDrag(int x, int y) override;
	virtual bool OnMouseLeftDClick(int x, int y) override;

	virtual bool OnDraw() const override;
	virtual bool Clear() override;

	void SetLoop(bool is_loop) {
		m_is_closed = is_loop;
	}

private:
	bool ShouldFixPos() const;

protected:
	mutable std::vector<sm::vec2> m_polyline;
	sm::vec2 m_curr_pos;

private:
	EditView& m_view;

	std::function<ShapeCapture::NodeRef()> m_get_selected;

	bool m_is_closed;

	ShapeCapture::NodeRef m_selected;
	int m_selected_ctrl_node = -1;

}; // EditPolylineOP

}