#pragma once

#include "global.h"

namespace render {
	enum class LightType{
		PointLight,DirectLight
	};

	struct Light {
		
		LightType type;
		Eigen::Vector3f position;
		Eigen::Vector3f intensity;

		Eigen::Vector3f direction;
		float radius;
	};
}