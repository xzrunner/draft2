#include "draft2/DrawLineUtility.h"

#include <SM_Calc.h>

#include <wx/utils.h>

namespace draft2
{

bool DrawLineUtility::IsStraightOpen(const std::vector<sm::vec2>& lines)
{
	return !lines.empty() && wxGetKeyState(WXK_SHIFT);
}

sm::vec2 DrawLineUtility::FixPosTo8DirStraight(const sm::vec2& last, const sm::vec2& curr)
{
	float nearest;
	sm::vec2 fixed = curr;

	const float dx = fabs(curr.x - last.x);
	nearest = dx;
	fixed.Set(last.x, curr.y);

	const float dy = fabs(curr.y - last.y);
	if (dy < nearest)
	{
		nearest = dy;
		fixed.Set(curr.x, last.y);
	}

	sm::vec2 other(last.x + 1, last.y - 1);
	const float dxyDown = sm::dis_pos_to_seg(curr, last, other);
	if (dxyDown < nearest)
	{
		nearest = dxyDown;
		sm::get_foot_of_perpendicular(last, other, curr, &fixed);
	}

	other.Set(last.x + 1, last.y + 1);
	const float dxyUp = sm::dis_pos_to_seg(curr, last, other);
	if (dxyUp < nearest)
	{
		nearest = dxyUp;
		sm::get_foot_of_perpendicular(last, other, curr, &fixed);
	}

	return fixed;
}

}