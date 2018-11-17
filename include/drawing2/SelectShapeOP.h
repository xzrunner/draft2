#pragma once

#include "drawing2/ShapeCapture.h"

#include <ee0/EditOP.h>
#include <ee0/typedef.h>

namespace ee0 { class SceneNodeContainer; }

namespace dw2
{

class SelectShapeOP : public ee0::EditOP
{
public:
	SelectShapeOP(const std::shared_ptr<pt0::Camera>& cam, const ee0::SubjectMgrPtr& sub_mgr,
		const ee0::SceneNodeContainer& nodes, float capture_threshold);

	virtual bool OnMouseLeftDown(int x, int y) override;
	virtual bool OnMouseRightDown(int x, int y) override;
	virtual bool OnMouseMove(int x, int y) override;

	virtual bool OnDraw() const override;
	virtual bool Clear() override;

private:
	void SelectByPos(int x, int y);
	ShapeCapture::NodeRef QueryByPos(int x, int y) const;

protected:
	ee0::SubjectMgrPtr m_sub_mgr;

	const ee0::SceneNodeContainer& m_nodes;

	float m_capture_threshold = 0;

	ShapeCapture::NodeRef m_active;
	ShapeCapture::NodeRef m_hot;

}; // SelectShapeOP

}