#pragma once

#include <vrpn_Types.h>
#include <vector>

namespace IKinema {

struct vector_t {
	vrpn_float64 data[3] = {0, 0, 0}; // x, y, z
};

struct quat_t {
	vrpn_float64 data[4] = {0, 0, 0, 1}; // x, y, z, w
};

struct transform_t {
	vector_t translation;
	quat_t rotation;
};

namespace vrpn {

struct bone_desc_t {
	int id = -1;
	int parent_id = -1;
	std::string name;
	transform_t rest;
};

using skeleton_desc_t = std::vector<bone_desc_t>;
using skeleton_frame_t = std::vector<transform_t>;

}
}