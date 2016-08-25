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
		const vrpn::skeleton_frame_t& get_animation_frame(std::size_t) const;

	private:
		void load(const std::string&);
		void load_metadata_from(const Json::Value&);
		void load_frame_from(const Json::Value&);

		std::size_t m_animation_length = 0;
		std::size_t m_animated_nodes = 0;
		std::vector<vrpn::skeleton_frame_t> m_anim_frames;
		vrpn::skeleton_desc_t m_animation_metadata;
	};

}