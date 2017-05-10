// Copyright © 2017, IKinema Ltd. All rights reserved.
// IKinema VRPN SDK, source distribution
// Your use and or redistribution of this software in source and / or binary form, with or without modification, is subject to: 
// 1. your ongoing acceptance of and compliance with the terms and conditions of the IKinema License Agreement; and 
// 2. your inclusion of this notice in any version of this software that you use  or redistribute.
// A copy of the IKinema License Agreement is available by contacting IKinema Ltd., https://www.ikinema.com, support@ikinema.com

#pragma once
#include "types.hpp"

namespace Json {
class Value;
}

namespace IKinema {
	
	class Animation {
	public:
		explicit Animation(const std::string&);
		~Animation();

		const vrpn::skeleton_desc_t& get_skeleton_description() const;
		const vrpn::transform_vector& get_animation_frame(std::size_t) const;

	private:
		void load(const std::string&);
		void load_metadata_from(const Json::Value&);
		void load_frame_from(const Json::Value&);

		std::size_t m_animation_length = 0;
		std::size_t m_animated_nodes = 0;
		std::vector<vrpn::transform_vector> m_anim_frames;
		vrpn::skeleton_desc_t m_animation_metadata;
	};

}