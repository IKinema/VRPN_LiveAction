#pragma once

#include <vector>
#include <array>

namespace FIK {
	class Quaternion;
}

namespace IKinema {

typedef std::array<double, 3> vector_t;	
typedef std::array<double, 4> quaternion_t;

struct transform_t {
	vector_t translation;
	quaternion_t rotation;
};

namespace vrpn {

struct bone_desc_t {
	int id = -1;
	int parent_id = -1;
	std::string name;
	transform_t rest;
	double figure_scale = 1.;
};

using skeleton_desc_t = std::vector<bone_desc_t>;
using transform_vector = std::vector<transform_t>;

struct rb_desc_t {
	std::string name;
};

using rigidbodies_desc_t = std::vector<rb_desc_t>;
}
}