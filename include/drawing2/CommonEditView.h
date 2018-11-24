#pragma once

#include "drawing2/EditView.h"

#include <ee0/typedef.h>

#include <node0/typedef.h>

namespace ee0 { class SceneNodeContainer; }

namespace dw2
{

class CommonEditView : public EditView
{
public:
	CommonEditView(const ee0::SceneNodeContainer& nodes,
		const ee0::SubjectMgrPtr& sub_mgr);

	// node
	virtual void Traverse(std::function<bool(const std::shared_ptr<gs::Shape>&)> func) const override;
	virtual void Insert(const std::shared_ptr<gs::Shape>& shape) override;
	virtual void Delete(const std::shared_ptr<gs::Shape>& shape) override;
	virtual void Clear() override;

	// selection
	virtual void AddSelected(const std::shared_ptr<gs::Shape>& shape) override;
	virtual void ClearSelectionSet() override;

	// canvas
	virtual void SetCanvasDirty() override;

	// shape
	virtual void ShapeChanged(const std::shared_ptr<gs::Shape>& shape) override;

private:
	n0::SceneNodePtr QueryNodeByShape(const std::shared_ptr<gs::Shape>& shape) const;

private:
	const ee0::SceneNodeContainer& m_nodes;

	ee0::SubjectMgrPtr m_sub_mgr;

}; // CommonEditView

}