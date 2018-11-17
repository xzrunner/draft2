#pragma once

#include <node0/typedef.h>

#include <SM_Vector.h>

#include <memory>

namespace gs { class Shape; }
namespace ee0 { class SceneNodeContainer; }

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

		n0::SceneNodePtr obj = nullptr;

		std::shared_ptr<gs::Shape> shape = nullptr;
		sm::vec2 pos;

		NodeRef()
			: type(Type::NONE)
			, shape(nullptr)
		{
			pos.MakeInvalid();
		}

		bool operator != (const NodeRef& node) const {
			return type  != node.type
				|| obj   != node.obj
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
	static NodeRef Capture(const ee0::SceneNodeContainer& nodes, float threshold, const sm::vec2& pos);

private:
	static NodeRef CapturePoint(const std::shared_ptr<gs::Shape>& shape, float threshold, const sm::vec2& pos);
	static NodeRef CaptureRect(const std::shared_ptr<gs::Shape>& shape, float threshold, const sm::vec2& pos);
	static NodeRef CaptureCircle(const std::shared_ptr<gs::Shape>& shape, float threshold, const sm::vec2& pos);
	static NodeRef CapturePolyline(const std::shared_ptr<gs::Shape>& shape, float threshold, const sm::vec2& pos);

}; // ShapeCapture

}