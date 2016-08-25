#include "vrpn_server.hpp"
#include "json.h"

#include <gsl/gsl>

#include <iostream>
#include <sstream>
#include <array>
#include <limits>

namespace IKinema {
namespace vrpn {

namespace detail {

void VRPN_CALLBACK text_message_sink(void* userdata, const vrpn_TEXTCB info)
{
	if (userdata == nullptr)
		return; // can't act on the SkeletonTracker if the pointer is invalid

	auto tracker = static_cast<SkeletonTracker*>(userdata);

	std::string message{info.message, sizeof(info.message) / sizeof(char)};
	// check if the client has requested metadata (hierarchy & rest pose)
	if (message.find("request_metadata") != std::string::npos) {
        tracker->enqueue_metadata();
    }
}

}

SkeletonTracker::SkeletonTracker(vrpn_Connection* connection, const std::string& name)
    : vrpn_Tracker_Server(name.c_str(), connection), vrpn_Text_Receiver(name.c_str(), connection)
{
	vrpn_Text_Receiver::register_message_handler(this, &detail::text_message_sink);
}

SkeletonTracker::~SkeletonTracker()
{
	vrpn_Text_Receiver::unregister_message_handler(this, &detail::text_message_sink);
}

void SkeletonTracker::mainloop()
{
	vrpn_Tracker_Server::mainloop();
	vrpn_Text_Receiver::mainloop();
	server_mainloop();
}

void SkeletonTracker::enqueue_metadata()
{
	constexpr ptrdiff_t chunk_size = 1000;

	// due to the limit of 1k bytes per message
	// split the metadata JSON string in chunks of 1000 characters
	// and send them one by one

	// prepare a time stamp
	timeval current_time;
	vrpn_gettimeofday(&current_time, nullptr);


	// we use the gsl span to view only the current chunk part of the metadata string
	const auto metadata_span = gsl::ensure_z(m_stream_metadata.c_str());
	auto meta_length = metadata_span.length();

	auto chunk_count = (meta_length / chunk_size) + 1;
	if (chunk_count > std::numeric_limits<char>::max())
		throw std::runtime_error{"too many chunks"};

    std::array<char, chunk_size + 3> buffer = {}; // 3 bytes extra = chunk_id, chunk_count & string terminator
    ptrdiff_t offset = 0;                            // start offset of current chunk
    for (uint8_t chunk_id = 1; chunk_id <= chunk_count; ++chunk_id, offset += chunk_size) {
        buffer[0] = chunk_id;
        buffer[1] = static_cast<char>(chunk_count);

        // bound copy operation
        std::size_t chunk_length = (offset + chunk_size) > meta_length ? meta_length - offset : chunk_size;
        auto current_span = metadata_span.subspan(offset, chunk_length);
        memcpy_s(&buffer[2], chunk_size, &current_span[0], chunk_length);
        buffer[chunk_length + 2] = '\0'; // don't forget C-string null terminator

        send_text_message(buffer.data(), current_time); // enqueue message to be sent
    }
}

void SkeletonTracker::build_metadata(const skeleton_desc_t& p_skeleton)
{
	// convert the skeleton static pose & hierarchy to JSON and store it in a string

    Json::Value bones{Json::arrayValue};
	assert(std::numeric_limits<Json::ArrayIndex>::max() > p_skeleton.size() && "too many bones for our JSON library");
    bones.resize(static_cast<Json::ArrayIndex>(p_skeleton.size()));
	auto max_id = bones.size();

	assert(max_id > 0 && "we need at least one bone");
	assert(p_skeleton.size() == bones.size() && "bones didn't init right");

    for (decltype(max_id) id = 0; id < max_id; ++id) {
        const auto& bone_input = p_skeleton[id];
        
		Json::Value rest_t{Json::objectValue};
		rest_t["X"] = bone_input.rest.translation.data[0];
		rest_t["Y"] = bone_input.rest.translation.data[1];
		rest_t["Z"] = bone_input.rest.translation.data[2];
		
		Json::Value rest_r{Json::objectValue};
		rest_r["X"] = bone_input.rest.rotation.data[0];
		rest_r["Y"] = bone_input.rest.rotation.data[1];
		rest_r["Z"] = bone_input.rest.rotation.data[2];
		rest_r["W"] = bone_input.rest.rotation.data[3];

		Json::Value rest{Json::objectValue};
		rest["Translation"] = std::move(rest_t);
        rest["Rotation"] = std::move(rest_t);

		auto& bone_output = bones[id];
		bone_output["Name"] = bone_input.name;
		bone_output["ID"] = bone_input.id;
		bone_output["PID"] = bone_input.parent_id;
		bone_output["Rest"] = std::move(rest);
    }
    
    Json::Value root{Json::objectValue};
    root["Bones"] = std::move(bones);

    // construct JSON serializer for the most compact code possible
    Json::StreamWriterBuilder json_writer_builder;
    json_writer_builder["precision"] = 5; // set floating point precision
    json_writer_builder["indentation"] = ""; // minimize use of empty spaces 
    json_writer_builder["commentStyle"] = "None"; // same as above
    auto json_writer = json_writer_builder.newStreamWriter();

    // serialize and store the structure as string
    std::stringstream out_stream;
	if (json_writer->write(root, &out_stream) != 0)
		throw std::runtime_error{"Error preparing stream metadata\n"};

    m_stream_metadata = out_stream.str();
}

void SkeletonTracker::send_frame(const skeleton_frame_t& p_frame)
{
    // prepare a time stamp
    timeval current_time;
    vrpn_gettimeofday(&current_time, nullptr);

    std::array<char, 200> buffer = {}; // buffer to hold the data for a single bone

    auto max_id = p_frame.size();
	if (max_id > std::numeric_limits<decltype(d_sensor)>::max())
		throw std::runtime_error{"too many bones in the frame"};

    for (decltype(max_id) id = 0; id < max_id; ++id) {
        const auto& bone_data = p_frame[id];
		buffer.fill('\0'); // clear buffer memory

		// use the same bone ID as the one sent with the metadata
        d_sensor = static_cast<decltype(d_sensor)>(id);

		std::copy_n(bone_data.translation.data, 3, pos);
		std::copy_n(bone_data.rotation.data, 4, d_quat);

        auto len = encode_to(buffer.data());
        if (d_connection &&
            d_connection->pack_message(len, current_time, position_m_id, d_sender_id, buffer.data(),
                                       vrpn_CONNECTION_LOW_LATENCY)) {
            std::cerr << "Unable to write message - dropped\n";
        }
    }

	// send the data to the network layer (also receives incoming messages)
    mainloop();
}

}
}