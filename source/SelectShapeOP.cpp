#include "drawing2/SelectShapeOP.h"
#include "drawing2/RenderStyle.h"

#include <ee0/CameraHelper.h>
#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>
#include <ee0/MsgHelper.h>

#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>
#include <painting2/OrthoCamera.h>
#include <geoshape/Polyline.h>

namespace dw2
{

SelectShapeOP::SelectShapeOP(const std::shared_ptr<pt0::Camera>& cam, const ee0::SubjectMgrPtr& sub_mgr, const ee0::SceneNodeContainer& nodes, float capture_threshold)
	: ee0::EditOP(cam)
	, m_sub_mgr(sub_mgr)
	, m_nodes(nodes)
	, m_capture_threshold(capture_threshold)
{
}

bool SelectShapeOP::OnMouseLeftDown(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftDown(x, y)) {
		return true;
	}

	SelectByPos(x, y);

	return false;
}

bool SelectShapeOP::OnMouseRightDown(int x, int y)
{
	if (ee0::EditOP::OnMouseRightDown(x, y)) {
		return true;
	}

	SelectByPos(x, y);

	return false;
}

bool SelectShapeOP::OnMouseMove(int x, int y)
{
	if (ee0::EditOP::OnMouseMove(x, y)) {
		return true;
	}

	m_hot = QueryByPos(x, y);

	return false;
}

bool SelectShapeOP::OnDraw() const
{
	if (ee0::EditOP::OnDraw()) {
		return true;
	}
	if (!m_active.shape && !m_hot.shape) {
		return false;
	}

	tess::Painter pt;

	float cam_scale = std::dynamic_pointer_cast<pt2::OrthoCamera>(m_camera)->GetScale();
	if (m_active.shape) {
		pt2::RenderSystem::DrawShape(pt, *m_active.shape, COL_ACTIVE_SHAPE, cam_scale);
		if (m_active.shape->get_type() == rttr::type::get<gs::Polyline>()) {
			const float radius = NODE_RADIUS * cam_scale;
			auto& verts = std::static_pointer_cast<gs::Polyline>(m_active.shape)->GetVertices();
			for (auto& v : verts) {
				pt.AddCircleFilled(v, radius, COL_ACTIVE_NODE);
			}
		}
	}
	if (m_hot.shape) {
		pt2::RenderSystem::DrawShape(pt, *m_hot.shape, COL_HOT_SHAPE, cam_scale);
	}

	pt2::RenderSystem::DrawPainter(pt);

	return false;
}

bool SelectShapeOP::Clear()
{
	if (ee0::EditOP::Clear()) {
		return true;
	}

	m_active.Reset();
	m_hot.Reset();

	return false;
}

void SelectShapeOP::SelectByPos(int x, int y)
{
	m_active.Reset();
	m_hot.Reset();
	m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);

	auto captured = QueryByPos(x, y);
	// add to selection set
	if (captured.obj && captured.shape)
	{
		auto nwp = n0::NodeWithPos(captured.obj, captured.obj, 0);
		ee0::MsgHelper::InsertSelection(*m_sub_mgr, { nwp });
	}
	m_active = captured;
	m_hot    = captured;
}

ShapeCapture::NodeRef SelectShapeOP::QueryByPos(int x, int y) const
{
	float cam_scale = std::dynamic_pointer_cast<pt2::OrthoCamera>(m_camera)->GetScale();
	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	return ShapeCapture::Capture(m_nodes, m_capture_threshold * cam_scale, pos);
}

}