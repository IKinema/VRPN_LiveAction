#pragma once

#include "types.hpp"
#include <vrpn_Tracker.h>
#include <vrpn_Text.h>

namespace IKinema {
namespace vrpn {

namespace detail {
void VRPN_CALLBACK text_message_sink(void*, const vrpn_TEXTCB);
}

class SkeletonTracker : public vrpn_Tracker_Server, vrpn_Text_Receiver {
public:
	using rb_count = size_t;

	//  The constructor takes the streamed subject name and the connection to be used
	explicit SkeletonTracker(vrpn_Connection*, const std::string& = "Skeleton");
	~SkeletonTracker() override;

	// the frame to be sent must have the same data order as the sent metadata
	void send_frame(const transform_vector&, const transform_vector&);

	// updates the skeleton description (static pose) used by this tracker
	void build_metadata(const skeleton_desc_t&, const rigidbodies_desc_t&);

protected:
	void mainloop() override;

	// responsible for sending metadata on request from the client
	friend void VRPN_CALLBACK detail::text_message_sink(void*, const vrpn_TEXTCB);
	void enqueue_metadata();

private:
	std::string m_stream_metadata;
	std::size_t m_stream_bone_count = 0;
	std::size_t m_stream_rb_count = 0;
};

}
}