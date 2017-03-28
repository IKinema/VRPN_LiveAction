#include "animation.hpp"
#include "json.h"

#include <fstream>

IKinema::Animation::Animation(const std::string& p_filename)
{
	load(p_filename);
}

IKinema::Animation::~Animation()
{}

void IKinema::Animation::load(const std::string& p_filename)
{
	Json::Value result_root{Json::nullValue};
	std::ifstream animation_file{p_filename};
	
	if (!animation_file.is_open())
		throw std::runtime_error{"unable to open animation file"};

	Json::Reader json_reader;
	if (!json_reader.parse(animation_file, result_root))
		throw std::runtime_error{"error parsing file"};

	load_metadata_from(result_root["metadata"]);
	if (m_animated_nodes == 0)
		throw std::runtime_error{"no animated nodes in animation"};

	const Json::Value& anim_data = result_root["animation"];

	if (!anim_data.isArray() || anim_data.size() == 0)
		throw std::runtime_error{"animation must be an non-empty array"};

	for (const auto& frame : anim_data)
		load_frame_from(frame);

	if (m_animation_length == 0)
		throw std::runtime_error{"no animation data read"};
}

const IKinema::vrpn::skeleton_desc_t& IKinema::Animation::get_skeleton_description() const
{
	return m_animation_metadata;
}

const IKinema::vrpn::transform_vector& IKinema::Animation::get_animation_frame(std::size_t p_requested_frame) const
{
	if (m_animation_length == 0)
		throw std::runtime_error{"animation is empty"};

	return m_anim_frames[p_requested_frame % m_animation_length]; // limit the returned frame to [0, m_animation_length-1]
}

void IKinema::Animation::load_metadata_from(const Json::Value& p_meta)
{
	if (!p_meta.isArray())
		throw std::runtime_error{"metadata must be an array"};

	for (const auto& node : p_meta) {
		vrpn::bone_desc_t bone_desc;
		bone_desc.id = node["id"].asInt();
		bone_desc.parent_id = node["pid"].asInt();
		bone_desc.name = node["name"].asString();
		const auto& rest_transform = node["rest"];
		
		if (!rest_transform.isArray() || rest_transform.size() != 7)
			throw std::runtime_error{"bad animation data"};

		// read translation [X, Y, Z]
		for (std::size_t i = 0; i < 3; ++i)
			bone_desc.rest.translation[i] = rest_transform[static_cast<Json::ArrayIndex>(i)].asDouble();

		// read quaternion rotation [X, Y, Z, W]
		for (std::size_t i = 0; i < 4; ++i)
			bone_desc.rest.rotation[i] = rest_transform[static_cast<Json::ArrayIndex>(i + 3)].asDouble();

		m_animation_metadata.push_back(std::move(bone_desc));
	}

	m_animated_nodes = m_animation_metadata.size();
}

void IKinema::Animation::load_frame_from(const Json::Value& p_anim_frame)
{
	if (!p_anim_frame.isArray())
		throw std::runtime_error{"animation must be an array"};

	vrpn::transform_vector new_frame;
	for (const auto& node_transform : p_anim_frame) {
		transform_t local_transform;

		if (!node_transform.isArray() || node_transform.size() != 7)
			throw std::runtime_error{"bad animation data"};

		// read translation [X, Y, Z]
		for (std::size_t i = 0; i < 3; ++i)
			local_transform.translation[i] = node_transform[static_cast<Json::ArrayIndex>(i)].asDouble();

		// read quaternion rotation [X, Y, Z, W]
		for (std::size_t i = 0; i < 4; ++i)
			local_transform.rotation[i] = node_transform[static_cast<Json::ArrayIndex>(i+3)].asDouble();

		new_frame.push_back(std::move(local_transform));
	}

	if (new_frame.size() != m_animated_nodes)
		throw std::runtime_error{"frame doesn't contain animation for all nodes"};

	m_anim_frames.push_back(std::move(new_frame));
	m_animation_length++;
}
