#pragma once

#include <memory>
#include <functional>

namespace gs { class Shape2D; }

namespace dw2
{

class EditView
{
public:
	virtual ~EditView() {}

	// nodes
	virtual void Traverse(std::function<bool(const std::shared_ptr<gs::Shape2D>&)> func) const = 0;
	virtual void Insert(const std::shared_ptr<gs::Shape2D>& shape) = 0;
	virtual void Delete(const std::shared_ptr<gs::Shape2D>& shape) = 0;
	virtual void Clear() = 0;

	// selection
	virtual void AddSelected(const std::shared_ptr<gs::Shape2D>& shape) = 0;
	virtual void ClearSelectionSet() = 0;

	// canvas
	virtual void SetCanvasDirty() = 0;

	// shape
	virtual void ShapeChanged(const std::shared_ptr<gs::Shape2D>& shape) = 0;

}; // EditView

}