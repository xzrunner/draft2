#include "drawing2/CommonEditView.h"

#include <ee0/SceneNodeContainer.h>
#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>
#include <ee0/MsgHelper.h>

#include <node0/SceneNode.h>
#include <node2/CompShape.h>
#include <ns/NodeFactory.h>

namespace dw2
{

CommonEditView::CommonEditView(const ee0::SceneNodeContainer& nodes, const ee0::SubjectMgrPtr& sub_mgr)
	: m_nodes(nodes)
	, m_sub_mgr(sub_mgr)
{
}

void CommonEditView::Traverse(std::function<bool(const std::shared_ptr<gs::Shape2D>& shape)> func) const
{
	m_nodes.Traverse([&](const ee0::GameObj& obj)->bool
	{
		if (!obj->HasSharedComp<n2::CompShape>()) {
			return true;
		}
		auto& cshape = obj->GetSharedComp<n2::CompShape>();
		auto& shape = cshape.GetShape();
		return func(shape);
	});
}

void CommonEditView::Insert(const std::shared_ptr<gs::Shape2D>& shape)
{
	auto obj = ns::NodeFactory::Create2D();
	obj->AddSharedComp<n2::CompShape>(shape);
	ee0::MsgHelper::InsertNode(*m_sub_mgr, obj, true);
}

void CommonEditView::Delete(const std::shared_ptr<gs::Shape2D>& shape)
{
	n0::SceneNodePtr node = QueryNodeByShape(shape);
	if (node)
	{
		ee0::MsgHelper::DeleteNode(*m_sub_mgr, node);
		m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
	}
}

void CommonEditView::Clear()
{
	m_sub_mgr->NotifyObservers(ee0::MSG_SCENE_NODE_CLEAR);
}

void CommonEditView::AddSelected(const std::shared_ptr<gs::Shape2D>& shape)
{
	n0::SceneNodePtr node = QueryNodeByShape(shape);
	if (node)
	{
		auto nwp = n0::NodeWithPos(node, node, 0);
		ee0::MsgHelper::InsertSelection(*m_sub_mgr, { nwp });
	}
}

void CommonEditView::ClearSelectionSet()
{
	m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
}

void CommonEditView::SetCanvasDirty()
{
	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

n0::SceneNodePtr CommonEditView::QueryNodeByShape(const std::shared_ptr<gs::Shape2D>& shape) const
{
	n0::SceneNodePtr node = nullptr;
	m_nodes.Traverse([&](const ee0::GameObj& obj)->bool
	{
		if (!obj->HasSharedComp<n2::CompShape>()) {
			return true;
		}

		auto& cshape = obj->GetSharedComp<n2::CompShape>();
		if (cshape.GetShape() == shape) {
			node = obj;
			return false;
		} else {
			return true;
		}
	});
	return node;
}

void CommonEditView::ShapeChanged(const std::shared_ptr<gs::Shape2D>& shape)
{
}

}