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
