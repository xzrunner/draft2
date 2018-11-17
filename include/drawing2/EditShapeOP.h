#pragma once

#include "drawing2/SelectShapeOP.h"

namespace gs { class Shape; }

namespace dw2
{

class EditShapeOP : public SelectShapeOP
{
public:
	EditShapeOP(const std::shared_ptr<pt0::Camera>& cam, const ee0::SubjectMgrPtr& sub_mgr,
		const ee0::SceneNodeContainer& nodes, float capture_threshold, uint32_t shape_type);

	virtual bool OnKeyDown(int key_code) override;

	virtual bool OnMouseLeftDown(int x, int y) override;
	virtual bool OnMouseLeftUp(int x, int y) override;
	virtual bool OnMouseRightDown(int x, int y) override;
	virtual bool OnMouseMove(int x, int y) override;
	virtual bool OnMouseDrag(int x, int y) override;
	virtual bool OnMouseLeftDClick(int x, int y) override;

	virtual bool OnDraw() const override;
	virtual bool Clear() override;

	uint32_t GetEditShapeType() const { return m_shape_type; }
	void ChangeEditState(uint32_t shape_type, std::shared_ptr<gs::Shape> selected = nullptr);

private:
	uint32_t m_shape_type = 0;

	ee0::EditOPPtr m_proxy_op = nullptr;

}; // EditShapeOP

}