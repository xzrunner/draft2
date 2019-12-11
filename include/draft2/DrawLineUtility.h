#pragma once

#include <vector>

#include <SM_Vector.h>

namespace draft2
{

class DrawLineUtility
{
public:

	static bool IsStraightOpen(const std::vector<sm::vec2>& lines);

	static sm::vec2 FixPosTo8DirStraight(const sm::vec2& last, const sm::vec2& curr);

}; // DrawLineUtility

}