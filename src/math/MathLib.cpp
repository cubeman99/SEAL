#include "MathLib.h"
#include <math.h>
#include <limits>


//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const float Math::PI			= (float) (4.0 * atan(1.0));
const float Math::TWO_PI		= Math::PI * 2.0f;
const float Math::HALF_PI		= Math::PI * 0.5f;
const float Math::INV_PI		= 1.0f / Math::PI;
const float Math::INV_TWO_PI	= 1.0f / Math::TWO_PI;
const float Math::DEG_TO_RAD	= Math::PI / 180.0f;
const float Math::RAD_TO_DEG	= 180.0f / Math::PI;
const float Math::LOG_2			= log(2.0f);
const float Math::LOG_10		= log(10.0f);
const float Math::INV_LOG_2		= 1.0f / Math::LOG_2;
const float Math::INV_LOG_10	= 1.0f / Math::LOG_10;
