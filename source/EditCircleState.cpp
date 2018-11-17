#include "drawing2/EditCircleState.h"
#include "drawing2/RenderStyle.h"

#include <ee0/CameraHelper.h>
#include <ee0/MsgHelper.h>

#include <SM_Calc.h>
#include <geoshape/Circle.h>
#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node2/CompShape.h>
#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>
#include <painting2/OrthoCamera.h>

namespace dw2
{

EditCircleState::EditCircleState(const std::shared_ptr<pt0::Camera>& camera,
	                             const ee0::SubjectMgrPtr& sub_mgr,
	                             std::function<ShapeCapture::NodeRef()> get_selected)
	: ee0::EditOpState(camera)
	, m_sub_mgr(sub_mgr)
	, m_get_selected(get_selected)
{
	Clear();
}

bool EditCircleState::OnMousePress(int x, int y)
{
	m_first_pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	m_selected = m_get_selected();
	return true;
}

bool EditCircleState::OnMouseRelease(int x, int y)
{
	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	if (m_selected.shape && m_selected.shape->get_type() == rttr::type::get<gs::Circle>()) {
		UpdateCirclePos(pos);
	} else if (m_first_pos.IsValid() && m_first_pos != pos) {
		m_curr_pos = pos;

		auto obj = ns::NodeFactory::Create();
		const float r = sm::dis_pos_to_pos(m_first_pos, m_curr_pos);
		auto shape = std::make_shared<gs::Circle>(m_first_pos, r);
		obj->AddUniqueComp<n2::CompShape>(shape);
		ee0::MsgHelper::InsertNode(*m_sub_mgr, obj, true);
	}

	Clear();

	return true;
}

bool EditCircleState::OnMouseDrag(int x, int y)
{
	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	if (m_selected.shape && m_selected.shape->get_type() == rttr::type::get<gs::Circle>()) {
		UpdateCirclePos(pos);
		return true;
	} else if (m_first_pos.IsValid()) {
		m_curr_pos = pos;
		return true;
	} else {
		return false;
	}
}

bool EditCircleState::OnDraw() const
{
	if (m_first_pos.IsValid() && m_curr_pos.IsValid())
	{
		tess::Painter pt;
		const float cam_scale = std::dynamic_pointer_cast<pt2::OrthoCamera>(m_camera)->GetScale();
		const float radius = sm::dis_pos_to_pos(m_first_pos, m_curr_pos);
		pt.AddCircle(m_first_pos, radius, COL_ACTIVE_SHAPE, cam_scale, static_cast<uint32_t>(radius * 0.5f));
		pt2::RenderSystem::DrawPainter(pt);
	}

	return false;
}

bool EditCircleState::Clear()
{
	m_first_pos.MakeInvalid();
	m_curr_pos.MakeInvalid();
	m_selected.Reset();
	return false;
}

void EditCircleState::UpdateCirclePos(const sm::vec2& pos)
{
	if (!m_selected.shape) {
		return;
	}

	auto circle = std::static_pointer_cast<gs::Circle>(m_selected.shape);
	if (m_selected.type == ShapeCapture::NodeRef::Type::CTRL_NODE) {
		circle->SetRadius(sm::dis_pos_to_pos(circle->GetCenter(), pos));
	} else {
		circle->SetCenter(pos);
	}
}

}