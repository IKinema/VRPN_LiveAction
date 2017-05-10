// Copyright © 2017, IKinema Ltd. All rights reserved.
// IKinema VRPN SDK, source distribution
// Your use and or redistribution of this software in source and / or binary form, with or without modification, is subject to: 
// 1. your ongoing acceptance of and compliance with the terms and conditions of the IKinema License Agreement; and 
// 2. your inclusion of this notice in any version of this software that you use  or redistribute.
// A copy of the IKinema License Agreement is available by contacting IKinema Ltd., https://www.ikinema.com, support@ikinema.com

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