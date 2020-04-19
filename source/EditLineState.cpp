#include "draft2/EditLineState.h"
#include "draft2/RenderStyle.h"

#include <ee0/CameraHelper.h>

#include <SM_Calc.h>
#include <tessellation/Painter.h>
#include <geoshape/Line2D.h>
#include <unirender2/RenderState.h>
#include <painting2/OrthoCamera.h>
#include <painting2/RenderSystem.h>

namespace draft2
{

EditLineState::EditLineState(const std::shared_ptr<pt0::Camera>& camera, EditView& view,
	                           std::function<ShapeCapture::NodeRef()> get_selected)
	: ee0::EditOpState(camera)
	, m_view(view)
	, m_get_selected(get_selected)
{
    Clear();
}

bool EditLineState::OnMousePress(int x, int y)
{
	m_first_pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
    m_curr_pos = m_first_pos;

    m_selected = m_get_selected();
    if (m_selected.shape &&
        m_selected.shape->get_type() == rttr::type::get<gs::Line2D>() &&
        m_selected.type == ShapeCapture::NodeRef::Type::CTRL_NODE)
    {
        auto line = std::static_pointer_cast<gs::Line2D>(m_selected.shape);

        float min_dis = std::numeric_limits<float>::max();
        float dis = sm::dis_pos_to_pos(m_selected.pos, line->GetStart());
        if (dis < min_dis) {
            min_dis = dis;
            m_selected_node_type = CtrlNodeType::Start;
        }
        dis = sm::dis_pos_to_pos(m_selected.pos, line->GetEnd());
        if (dis < min_dis) {
            min_dis = dis;
            m_selected_node_type = CtrlNodeType::End;
        }
    }

	return true;
}

bool EditLineState::OnMouseRelease(int x, int y)
{
	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	auto selected = m_get_selected().shape;
	if (selected && selected->get_type() == rttr::type::get<gs::Line2D>()) {
        UpdateLinePos(pos);
        m_curr_pos = pos;
	} else if (m_first_pos.IsValid()) {
        m_curr_pos = pos;
        m_view.Insert(std::make_shared<gs::Line2D>(m_first_pos, m_curr_pos));
    }

    Clear();

	return true;
}

bool EditLineState::OnMouseDrag(int x, int y)
{
	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	if (m_selected.shape && m_selected.shape->get_type() == rttr::type::get<gs::Line2D>())
    {
		UpdateLinePos(pos);
        m_curr_pos = pos;
		return true;
	}
    else if (m_first_pos.IsValid())
    {
		m_curr_pos = pos;
		return true;
	}
    else
    {
		return false;
	}
}

bool EditLineState::OnDraw(const ur2::Device& dev, ur2::Context& ctx) const
{
	if (m_first_pos.IsValid() &&
        m_curr_pos.IsValid() &&
        m_selected.type == ShapeCapture::NodeRef::Type::NONE)
	{
        tess::Painter pt;

        const float cam_scale = std::dynamic_pointer_cast<pt2::OrthoCamera>(m_camera)->GetScale();
        const float line_width = LINE_WIDTH * cam_scale;
        const float r = NODE_RADIUS * cam_scale;

        pt.AddLine(m_first_pos, m_curr_pos, COL_ACTIVE_SHAPE, line_width);

        pt.AddCircleFilled(m_first_pos, r, COL_ACTIVE_NODE);
        pt.AddCircleFilled(m_curr_pos, r, COL_ACTIVE_NODE);

        ur2::RenderState rs;
		pt2::RenderSystem::DrawPainter(dev, ctx, rs, pt);
	}

	return false;
}

bool EditLineState::Clear()
{
    m_first_pos.MakeInvalid();
    m_curr_pos.MakeInvalid();

    m_selected.Reset();
    m_selected_node_type = CtrlNodeType::Unknown;

    return false;
}

void EditLineState::UpdateLinePos(const sm::vec2& pos)
{
    if (!m_selected.shape) {
        return;
    }

    if (m_selected.type == ShapeCapture::NodeRef::Type::CTRL_NODE)
    {
        if (m_selected_node_type != CtrlNodeType::Unknown)
        {
            auto line = std::static_pointer_cast<gs::Line2D>(m_selected.shape);
            switch (m_selected_node_type)
            {
            case CtrlNodeType::Start:
                line->SetStart(pos);
                break;
            case CtrlNodeType::End:
                line->SetEnd(pos);
                break;
            default:
                assert(0);
            }
            m_view.ShapeChanged(line);
        }
    }
    else
    {
        auto line = std::static_pointer_cast<gs::Line2D>(m_selected.shape);
        assert(m_selected.pos.IsValid());
        auto offset = pos - m_curr_pos;
        line->SetStart(line->GetStart() + offset);
        line->SetEnd(line->GetEnd() + offset);
        m_view.ShapeChanged(line);
    }
}

}