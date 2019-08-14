#pragma once

#include "drawing2/EditView.h"

#include <node0/typedef.h>

#include <SM_Vector.h>

#include <memory>
#include <vector>

namespace gs { class Shape2D; }

namespace dw2
{

class ShapeCapture
{
public:
	struct NodeRef
	{
		enum class Type
		{
			NONE = 0,
			SHAPE,
			CTRL_NODE,
			OUTSIDE,
		} type = Type::NONE;

		std::shared_ptr<gs::Shape2D> shape = nullptr;
		sm::vec2 pos;

		NodeRef()
			: type(Type::NONE)
			, shape(nullptr)
		{
			pos.MakeInvalid();
		}

		bool operator != (const NodeRef& node) const {
			return type  != node.type
				|| shape != node.shape
				|| pos   != node.pos;
		}

		void Reset()
		{
			type = Type::NONE;
			shape = nullptr;
			pos.MakeInvalid();
		}
	};

public:
	static NodeRef Capture(const EditView& view, float threshold, const sm::vec2& pos);

private:
	static NodeRef CapturePoint(const std::shared_ptr<gs::Shape2D>& shape, float threshold, const sm::vec2& pos);
	static NodeRef CaptureRect(const std::shared_ptr<gs::Shape2D>& shape, float threshold, const sm::vec2& pos);
	static NodeRef CaptureCircle(const std::shared_ptr<gs::Shape2D>& shape, float threshold, const sm::vec2& pos);
	static NodeRef CapturePoly(const std::shared_ptr<gs::Shape2D>& shape, const std::vector<sm::vec2>& verts, float threshold, const sm::vec2& pos);

}; // ShapeCapture

}