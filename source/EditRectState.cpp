#include "drawing2/EditRectState.h"
#include "drawing2/RenderStyle.h"

#include <ee0/CameraHelper.h>
#include <ee0/MsgHelper.h>

#include <SM_Calc.h>
#include <geoshape/Rect.h>
#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node2/CompShape.h>
#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>
#include <painting2/OrthoCamera.h>

namespace dw2
{

EditRectState::EditRectState(const std::shared_ptr<pt0::Camera>& camera,
	                         const ee0::SubjectMgrPtr& sub_mgr,
	                         std::function<ShapeCapture::NodeRef()> get_selected)
	: ee0::EditOpState(camera)
	, m_sub_mgr(sub_mgr)
	, m_get_selected(get_selected)
{
	Clear();
}

bool EditRectState::OnMousePress(int x, int y)
{
	m_first_pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);

	m_selected = m_get_selected();
	if (m_selected.shape &&
		m_selected.shape->get_type() == rttr::type::get<gs::Rect>() &&
		m_selected.type == ShapeCapture::NodeRef::Type::CTRL_NODE)
	{
		auto rect = std::static_pointer_cast<gs::Rect>(m_selected.shape);
		auto& r = rect->GetRect();

		float min_dis = std::numeric_limits<float>::max();
		float dis = sm::dis_pos_to_pos(m_selected.pos, { r.xmin, r.ymin });
		if (dis < min_dis) {
			min_dis = dis;
			m_selected_node_type = CtrlNodeType::XMIN_YMIN;
		}
		dis = sm::dis_pos_to_pos(m_selected.pos, { r.xmax, r.ymin });
		if (dis < min_dis) {
			min_dis = dis;
			m_selected_node_type = CtrlNodeType::XMAX_YMIN;
		}
		dis = sm::dis_pos_to_pos(m_selected.pos, { r.xmax, r.ymax });
		if (dis < min_dis) {
			min_dis = dis;
			m_selected_node_type = CtrlNodeType::XMAX_YMAX;
		}
		dis = sm::dis_pos_to_pos(m_selected.pos, { r.xmin, r.ymax });
		if (dis < min_dis) {
			min_dis = dis;
			m_selected_node_type = CtrlNodeType::XMIN_YMAX;
		}
	}

	return true;
}

bool EditRectState::OnMouseRelease(int x, int y)
{
	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	if (m_selected.shape && m_selected.shape->get_type() == rttr::type::get<gs::Rect>()) {
		UpdateRectPos(pos);
	} else if (m_first_pos.IsValid() && m_first_pos != pos) {
		m_curr_pos = pos;

		auto obj = ns::NodeFactory::Create();
		auto shape = std::make_shared<gs::Rect>(sm::rect(m_first_pos, m_curr_pos));
		obj->AddUniqueComp<n2::CompShape>(shape);
		ee0::MsgHelper::InsertNode(*m_sub_mgr, obj, true);
	}

	Clear();

	return true;
}

bool EditRectState::OnMouseDrag(int x, int y)
{
	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	if (m_selected.shape && m_selected.shape->get_type() == rttr::type::get<gs::Rect>()) {
		UpdateRectPos(pos);
		return true;
	} else if (m_first_pos.IsValid()) {
		m_curr_pos = pos;
		return true;
	} else {
		return false;
	}
}

bool EditRectState::OnDraw() const
{
	if (m_first_pos.IsValid() && m_curr_pos.IsValid())
	{
		tess::Painter pt;
		float cam_scale = std::dynamic_pointer_cast<pt2::OrthoCamera>(m_camera)->GetScale();
		pt.AddRect(m_first_pos, m_curr_pos, COL_ACTIVE_SHAPE, cam_scale);
		pt2::RenderSystem::DrawPainter(pt);
	}

	return false;
}

bool EditRectState::Clear()
{
	m_first_pos.MakeInvalid();
	m_curr_pos.MakeInvalid();
	m_selected.Reset();
	m_selected_node_type = CtrlNodeType::UNKNOWN;
	return false;
}

void EditRectState::UpdateRectPos(const sm::vec2& pos)
{
	if (!m_selected.shape) {
		return;
	}

	if (m_selected.type == ShapeCapture::NodeRef::Type::CTRL_NODE)
	{
		if (m_selected_node_type != CtrlNodeType::UNKNOWN)
		{
			auto rect = std::static_pointer_cast<gs::Rect>(m_selected.shape);
			auto new_r = rect->GetRect();
			switch (m_selected_node_type)
			{
			case CtrlNodeType::XMIN_YMIN:
				new_r.xmin = pos.x;
				new_r.ymin = pos.y;
				break;
			case CtrlNodeType::XMAX_YMIN:
				new_r.xmax = pos.x;
				new_r.ymin = pos.y;
				break;
			case CtrlNodeType::XMAX_YMAX:
				new_r.xmax = pos.x;
				new_r.ymax = pos.y;
				break;
			case CtrlNodeType::XMIN_YMAX:
				new_r.xmin = pos.x;
				new_r.ymax = pos.y;
				break;
			}
			rect->SetRect(new_r);
		}
	}
	else
	{
		auto rect = std::static_pointer_cast<gs::Rect>(m_selected.shape);
		auto r = rect->GetRect();
		r.Translate(pos - r.Center());
		rect->SetRect(r);
	}
}

}