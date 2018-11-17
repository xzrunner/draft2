#include "drawing2/EditPolylineOP.h"
#include "drawing2/DrawLineUtility.h"
#include "drawing2/RenderStyle.h"

#include <ee0/CameraHelper.h>
#include <ee0/MessageID.h>
#include <ee0/SubjectMgr.h>
#include <ee0/MsgHelper.h>

#include <SM_Calc.h>
#include <tessellation/Painter.h>
#include <painting2/OrthoCamera.h>
#include <painting2/RenderSystem.h>
#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node2/CompShape.h>
#include <geoshape/Polyline.h>
#include <geoshape/Polygon.h>

namespace dw2
{

EditPolylineOP::EditPolylineOP(const std::shared_ptr<pt0::Camera>& cam,
	                           const ee0::SubjectMgrPtr& sub_mgr,
	                           std::function<ShapeCapture::NodeRef()> get_selected,
	                           bool is_closed)
	: ee0::EditOP(cam)
	, m_sub_mgr(sub_mgr)
	, m_get_selected(get_selected)
	, m_is_closed(is_closed)
{
	m_curr_pos.MakeInvalid();
}

bool EditPolylineOP::OnMouseLeftDown(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftDown(x, y)) {
		return true;
	}

	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);

	m_selected = m_get_selected();
	// interrupt
	if (m_selected.shape &&
		m_selected.shape->get_type() == rttr::type::get<gs::Polyline>() &&
		m_selected.type == ShapeCapture::NodeRef::Type::SHAPE)
	{
		auto pl = std::static_pointer_cast<gs::Polyline>(m_selected.shape);
		auto verts = pl->GetVertices();
		int nearest = -1;
		float dis = sm::dis_pos_to_polyline(pos, verts, &nearest);
		assert(nearest >= 0 && nearest < static_cast<int>(verts.size()));
		verts.insert(verts.begin() + nearest + 1, pos);
		pl->SetVertices(verts);
		m_selected.type = ShapeCapture::NodeRef::Type::CTRL_NODE;
		m_selected.pos = pos;
	}

	if (m_selected.shape &&
		m_selected.shape->get_type() == rttr::type::get<gs::Polyline>() &&
		m_selected.type == ShapeCapture::NodeRef::Type::CTRL_NODE)
	{
		m_selected_ctrl_node = -1;
		auto& verts = std::static_pointer_cast<gs::Polyline>(m_selected.shape)->GetVertices();
		for (int i = 0, n = verts.size(); i < n; ++i) {
			if (verts[i] == m_selected.pos) {
				m_selected_ctrl_node = i;
			}
		}
	}
	else
	{
		m_selected_ctrl_node = -1;
		if (DrawLineUtility::IsStraightOpen(m_polyline)) {
			pos = DrawLineUtility::FixPosTo8DirStraight(m_polyline.back(), pos);
		}
		m_polyline.push_back(pos);
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}

	return false;
}

bool EditPolylineOP::OnMouseRightDown(int x, int y)
{
	if (ee0::EditOP::OnMouseRightDown(x, y)) {
		return true;
	}

	m_selected = m_get_selected();
	// delete selected ctrl node
	if (m_selected.shape &&
		m_selected.shape->get_type() == rttr::type::get<gs::Polyline>() &&
		m_selected.type == ShapeCapture::NodeRef::Type::CTRL_NODE)
	{
		auto pl = std::static_pointer_cast<gs::Polyline>(m_selected.shape);
		auto verts = pl->GetVertices();
		for (auto itr = verts.begin(); itr != verts.end(); ++itr) {
			if (*itr == m_selected.pos) {
				verts.erase(itr);
				break;
			}
		}
		pl->SetVertices(verts);
	}
	// delete last node
	else if (!m_polyline.empty())
	{
		m_polyline.pop_back();
		if (m_polyline.empty()) {
			m_curr_pos.MakeInvalid();
		}
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}

	return false;
}

bool EditPolylineOP::OnMouseMove(int x, int y)
{
	if (ee0::EditOP::OnMouseMove(x, y)) {
		return true;
	}

	if (m_polyline.empty()) return false;

	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	if (DrawLineUtility::IsStraightOpen(m_polyline)) {
		pos = DrawLineUtility::FixPosTo8DirStraight(m_polyline.back(), pos);
	}
	m_curr_pos = pos;
	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);

	return false;
}

bool EditPolylineOP::OnMouseDrag(int x, int y)
{
	if (ee0::EditOP::OnMouseDrag(x, y)) {
		return true;
	}

	if (m_selected_ctrl_node >= 0)
	{
		assert(m_selected.shape
			&& m_selected.shape->get_type() == rttr::type::get<gs::Polyline>()
			&& m_selected.type == ShapeCapture::NodeRef::Type::CTRL_NODE);
		auto poly = std::static_pointer_cast<gs::Polyline>(m_selected.shape);
		auto verts = poly->GetVertices();
		assert(m_selected_ctrl_node < static_cast<int>(verts.size()));
		auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
		verts[m_selected_ctrl_node] = pos;
		poly->SetVertices(verts);
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}

	return false;
}

bool EditPolylineOP::OnMouseLeftDClick(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftDClick(x, y)) {
		return true;
	}

	auto obj = ns::NodeFactory::Create();
	std::shared_ptr<gs::Shape> shape = nullptr;
	if (m_is_closed) {
		shape = std::make_shared<gs::Polygon>(m_polyline);
	} else {
		shape = std::make_shared<gs::Polyline>(m_polyline);
	}
	obj->AddUniqueComp<n2::CompShape>(shape);
	ee0::MsgHelper::InsertNode(*m_sub_mgr, obj, false);
	m_polyline.clear();
	m_curr_pos.MakeInvalid();

	return false;
}

bool EditPolylineOP::OnDraw() const
{
	if (ee0::EditOP::OnDraw()) {
		return true;
	}
	if (m_polyline.empty()) {
		return false;
	}

	float cam_scale = 1.0f;
	if (m_camera->TypeID() == pt0::GetCamTypeID<pt2::OrthoCamera>()) {
		cam_scale = std::dynamic_pointer_cast<pt2::OrthoCamera>(m_camera)->GetScale();
	}

	tess::Painter pt;

	// lines
	const float line_width = LINE_WIDTH * cam_scale;
	pt.AddPolyline(m_polyline.data(), m_polyline.size(), COL_ACTIVE_SHAPE, line_width);
	if (m_curr_pos.IsValid()) {
		pt.AddLine(m_polyline.back(), m_curr_pos, COL_ACTIVE_SHAPE, line_width);
	}

	// points
	const float radius = NODE_RADIUS * cam_scale;
	for (auto& p : m_polyline) {
		pt.AddCircleFilled(p, radius, COL_ACTIVE_NODE);
	}

	pt2::RenderSystem::DrawPainter(pt);

	return false;
}

bool EditPolylineOP::Clear()
{
	if (ee0::EditOP::Clear()) {
		return true;
	}

	m_polyline.clear();
	m_curr_pos.MakeInvalid();

	m_selected.Reset();
	m_selected_ctrl_node = -1;

	return false;
}

bool EditPolylineOP::ShouldFixPos() const
{
	return DrawLineUtility::IsStraightOpen(m_polyline);
}

}