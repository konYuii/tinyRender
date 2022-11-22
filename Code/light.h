#pragma once

#include "global.h"

namespace render {

	struct DirectLight {
		Eigen::Vector3f intensity;
		Eigen::Vector3f direction;
	};
	struct PointLight {
		Eigen::Vector3f position;
		Eigen::Vector3f intensity;
		float radius;
	};
}