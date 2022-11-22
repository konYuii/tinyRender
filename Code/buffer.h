#pragma once
#include "global.h"

namespace render {
	struct DepthBuffer {
		std::vector<float> data;
		size_t width;
		size_t height;

		DepthBuffer(size_t w, size_t h)
		{
			width = w;
			height = h;
			data.resize(w * h);
			for (size_t i = 0; i < data.size(); i++)
			{
				data[i] = std::numeric_limits<float>::max();
			}
		}
	};

}