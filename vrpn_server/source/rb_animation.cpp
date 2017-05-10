// Copyright © 2017, IKinema Ltd. All rights reserved.
// IKinema VRPN SDK, source distribution
// Your use and or redistribution of this software in source and / or binary form, with or without modification, is subject to: 
// 1. your ongoing acceptance of and compliance with the terms and conditions of the IKinema License Agreement; and 
// 2. your inclusion of this notice in any version of this software that you use  or redistribute.
// A copy of the IKinema License Agreement is available by contacting IKinema Ltd., https://www.ikinema.com, support@ikinema.com

#include "rb_animation.h"
#include <cmath>

IKinema::rb_animation::rb_animation() : m_desc{ {"sin"}, {"cos"} }, m_anim{{}, {}} {}

const IKinema::vrpn::transform_vector& IKinema::rb_animation::get_animation_frame(std::size_t p_frame)
{
	double delta = static_cast<double>(p_frame % m_animation_length) / m_animation_length;
	m_anim[0].translation = vector_t{delta, std::sin(delta * 6.28), 0};
	m_anim[1].translation = vector_t{ std::cos(delta * 6.28), 0, delta };
	return m_anim;
}
