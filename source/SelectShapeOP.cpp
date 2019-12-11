#include "draft2/SelectShapeOP.h"
#include "draft2/RenderStyle.h"
#include "draft2/EditView.h"

#include <ee0/CameraHelper.h>

#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>
#include <painting2/OrthoCamera.h>
#include <geoshape/Line2D.h>
#include <geoshape/Polyline2D.h>

namespace draft2
{

SelectShapeOP::SelectShapeOP(const std::shared_ptr<pt0::Camera>& cam,
	                         EditView& view, float capture_threshold)
	: ee0::EditOP(cam)
	, m_view(view)
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
	if (m_active.shape)
    {
		pt2::RenderSystem::DrawShape(pt, *m_active.shape, COL_ACTIVE_SHAPE, cam_scale);

        const float radius = NODE_RADIUS * cam_scale;
        auto type = m_active.shape->get_type();
        if (type == rttr::type::get<gs::Line2D>())
        {
            auto line = std::static_pointer_cast<gs::Line2D>(m_active.shape);
            pt.AddCircleFilled(line->GetStart(), radius, COL_ACTIVE_NODE);
            pt.AddCircleFilled(line->GetEnd(),   radius, COL_ACTIVE_NODE);
        }
		else if (type == rttr::type::get<gs::Polyline2D>())
        {
			auto& verts = std::static_pointer_cast<gs::Polyline2D>(m_active.shape)->GetVertices();
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
	m_view.ClearSelectionSet();

	auto captured = QueryByPos(x, y);
	// add to selection set
	if (captured.shape) {
		m_view.AddSelected(captured.shape);
	}

	m_active = captured;
	m_hot    = captured;
}

ShapeCapture::NodeRef SelectShapeOP::QueryByPos(int x, int y) const
{
	float cam_scale = std::dynamic_pointer_cast<pt2::OrthoCamera>(m_camera)->GetScale();
	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	return ShapeCapture::Capture(m_view, m_capture_threshold * cam_scale, pos);
}

}