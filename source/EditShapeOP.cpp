#include "drawing2/EditShapeOP.h"

#include "drawing2/EditPointState.h"
#include "drawing2/EditRectState.h"
#include "drawing2/EditCircleState.h"
#include "drawing2/EditPolylineOP.h"

#include <geoshape/Point2D.h>
#include <geoshape/Rect.h>
#include <geoshape/Circle.h>
#include <geoshape/Polyline2D.h>
#include <geoshape/Polygon2D.h>

#include <wx/defs.h>

namespace dw2
{

EditShapeOP::EditShapeOP(const std::shared_ptr<pt0::Camera>& cam, EditView& view,
	                     float capture_threshold, uint32_t shape_type)
	: SelectShapeOP(cam, view, capture_threshold)

	, m_view(view)
{
	ChangeEditState(shape_type, nullptr);
}

bool EditShapeOP::OnKeyDown(int key_code)
{
	if (SelectShapeOP::OnKeyDown(key_code)) {
		return true;
	}
	if (m_proxy_op) {
		return m_proxy_op->OnKeyDown(key_code);
	}

	if (key_code == WXK_DELETE)
	{
		if (m_active.shape) {
			m_view.Delete(m_active.shape);
			Clear();
		}
	}

	return false;
}

bool EditShapeOP::OnMouseLeftDown(int x, int y)
{
	if (SelectShapeOP::OnMouseLeftDown(x, y)) {
		return true;
	}
	if (m_proxy_op) {
		return m_proxy_op->OnMouseLeftDown(x, y);
	}

	if (m_active.shape) {
		ChangeEditState(m_active.shape->get_type().get_id(), m_active.shape);
	}

	if (m_op_state) {
		m_op_state->OnMousePress(x, y);
	}

	return false;
}

bool EditShapeOP::OnMouseLeftUp(int x, int y)
{
	if (SelectShapeOP::OnMouseLeftUp(x, y)) {
		return true;
	}
	if (m_proxy_op) {
		return m_proxy_op->OnMouseLeftUp(x, y);
	}

	if (m_op_state) {
		m_op_state->OnMouseRelease(x, y);
	}

	return false;
}

bool EditShapeOP::OnMouseRightDown(int x, int y)
{
	if (SelectShapeOP::OnMouseRightDown(x, y)) {
		return true;
	}
	if (m_proxy_op) {
		return m_proxy_op->OnMouseRightDown(x, y);
	}

	if (m_active.shape) {
		m_view.Delete(m_active.shape);
		Clear();
	}

	//if (m_op_state) {
	//	m_op_state->OnMousePress(x, y);
	//}

	return false;
}

bool EditShapeOP::OnMouseMove(int x, int y)
{
	if (SelectShapeOP::OnMouseMove(x, y)) {
		return true;
	}
	if (m_proxy_op) {
		return m_proxy_op->OnMouseMove(x, y);
	}

	if (m_op_state) {
		m_op_state->OnMouseMove(x, y);
	}

	return false;
}

bool EditShapeOP::OnMouseDrag(int x, int y)
{
	if (SelectShapeOP::OnMouseDrag(x, y)) {
		return true;
	}
	if (m_proxy_op) {
		return m_proxy_op->OnMouseDrag(x, y);
	}

	if (m_op_state) {
		m_op_state->OnMouseDrag(x, y);
	}

	return false;
}

bool EditShapeOP::OnMouseLeftDClick(int x, int y)
{
	if (SelectShapeOP::OnMouseLeftDClick(x, y)) {
		return true;
	}
	if (m_proxy_op) {
		return m_proxy_op->OnMouseLeftDClick(x, y);
	}

	return false;
}

bool EditShapeOP::OnDraw() const
{
	if (SelectShapeOP::OnDraw()) {
		return true;
	}
	if (m_proxy_op) {
		return m_proxy_op->OnDraw();
	}

	if (m_op_state) {
		m_op_state->OnDraw();
	}

	return false;
}

bool EditShapeOP::Clear()
{
	if (SelectShapeOP::Clear()) {
		return true;
	}
	if (m_proxy_op) {
		return m_proxy_op->Clear();
	}

	if (m_op_state) {
		m_op_state->Clear();
	}

	return false;
}

void EditShapeOP::ChangeEditState(uint32_t shape_type, std::shared_ptr<gs::Shape2D> selected)
{
	if (shape_type == m_shape_type) {
		return;
	}

	m_shape_type = shape_type;

	auto get_selected = [&]() { return m_active; };
	if (shape_type == rttr::type::get<gs::Point2D>().get_id()) {
		ChangeEditOpState(std::make_shared<EditPointState>(m_camera, m_view, get_selected));
	} else if (shape_type == rttr::type::get<gs::Rect>().get_id()) {
		ChangeEditOpState(std::make_shared<EditRectState>(m_camera, m_view, get_selected));
	} else if (shape_type == rttr::type::get<gs::Circle>().get_id()) {
		ChangeEditOpState(std::make_shared<EditCircleState>(m_camera, m_view, get_selected));
	} else if (shape_type == rttr::type::get<gs::Polyline2D>().get_id()) {
		ChangeEditOpState(nullptr);
		m_proxy_op = std::make_shared<EditPolylineOP>(m_camera, m_view, get_selected, false);
	} else if (shape_type == rttr::type::get<gs::Polygon2D>().get_id()) {
		ChangeEditOpState(nullptr);
		m_proxy_op = std::make_shared<EditPolylineOP>(m_camera, m_view, get_selected, true);
	}

	if (shape_type != rttr::type::get<gs::Polyline2D>().get_id() &&
		shape_type != rttr::type::get<gs::Polygon2D>().get_id()) {
		m_proxy_op.reset();
	}
}

}