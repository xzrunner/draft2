#include "drawing2/EditPointState.h"

#include <ee0/CameraHelper.h>
#include <ee0/MsgHelper.h>

#include <geoshape/Point2D.h>
#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node2/CompShape.h>

namespace dw2
{

EditPointState::EditPointState(const std::shared_ptr<pt0::Camera>& camera,
	                           const ee0::SubjectMgrPtr& sub_mgr,
	                           std::function<ShapeCapture::NodeRef()> get_selected)
	: ee0::EditOpState(camera)
	, m_sub_mgr(sub_mgr)
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
		auto obj = ns::NodeFactory::Create();
		auto shape = std::make_shared<gs::Point2D>(pos);
		obj->AddUniqueComp<n2::CompShape>(shape);
		ee0::MsgHelper::InsertNode(*m_sub_mgr, obj, true);
	}

	return true;
}

bool EditPointState::OnMouseDrag(int x, int y)
{
	auto selected = m_get_selected().shape;
	if (selected && selected->get_type() == rttr::type::get<gs::Point2D>()) {
		auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
		std::static_pointer_cast<gs::Point2D>(selected)->SetPos(pos);
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