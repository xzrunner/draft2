#include "draft2/EditPointState.h"

#include <ee0/CameraHelper.h>

#include <geoshape/Point2D.h>

namespace draft2
{

EditPointState::EditPointState(const std::shared_ptr<pt0::Camera>& camera, EditView& view,
	                           std::function<ShapeCapture::NodeRef()> get_selected)
	: ee0::EditOpState(camera)
	, m_view(view)
	, m_get_selected(get_selected)
{
	m_first_pos.MakeInvalid();
}

bool EditPointState::OnMousePress(int x, int y)
{
	m_first_pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);

	return false;
}

bool EditPointState::OnMouseRelease(int x, int y)
{
	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	auto selected = m_get_selected().shape;
	if (selected && selected->get_type() == rttr::type::get<gs::Point2D>()) {
		std::static_pointer_cast<gs::Point2D>(selected)->SetPos(pos);
	} else if (m_first_pos.IsValid()) {
		m_view.Insert(std::make_shared<gs::Point2D>(pos));
	}

	return true;
}

bool EditPointState::OnMouseDrag(int x, int y)
{
	auto selected = m_get_selected().shape;
	if (selected && selected->get_type() == rttr::type::get<gs::Point2D>()) {
		auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
		std::static_pointer_cast<gs::Point2D>(selected)->SetPos(pos);
		m_view.ShapeChanged(selected);
		return true;
	} else {
		return false;
	}
}

bool EditPointState::Clear()
{
	m_first_pos.MakeInvalid();

	return false;
}

}