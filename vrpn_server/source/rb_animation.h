#pragma once
#include "types.hpp"

namespace IKinema {

	class rb_animation {
	public:
		rb_animation();
		const vrpn::rigidbodies_desc_t& get_description() const { return m_desc; }
		const vrpn::transform_vector& get_animation_frame(std::size_t);

	private:
		const std::size_t m_animation_length = 100;
		vrpn::rigidbodies_desc_t m_desc;
		vrpn::transform_vector m_anim;
	};

}