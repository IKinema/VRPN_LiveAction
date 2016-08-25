#include "vrpn_server.hpp"
#include "animation.hpp"

#include <memory>
#include <iostream>
#include <thread>
#include <string>

int main()
{
	try {
		const int server_port = 3883;
		const std::string subject_name = "Test_Skeleton";

		std::cout << "reading animation file demo.json....\n";
		IKinema::Animation demo_anim{"demo.json"};

		std::cout << "creating server on port " << server_port << " ....\n";
		auto connection = vrpn_create_server_connection(3883);

		std::cout << "adding subject " << subject_name << " to server....\n";
		auto tracker = std::make_unique<IKinema::vrpn::SkeletonTracker>(connection, subject_name);

		// the SkeletonTracker needs to know about its skeleton before being able to stream it
		tracker->build_metadata(demo_anim.get_skeleton_description());

		std::cout << "streaming animation....\n";

		bool should_exit = false;
		std::thread server_thread([&]() {
			std::size_t frame_time = 0;
			auto wait_until = std::chrono::system_clock::now();
			while(!should_exit) {
				auto&& animation_frame = demo_anim.get_animation_frame(frame_time++); // read animation from somewhere
				tracker->send_frame(animation_frame); // use the tracker to send the frame to the connection
				
				// serve frames at  30 fps
				// using sleep_until would cancel out the delay introduced by send_frame
				wait_until += std::chrono::milliseconds(33);
				std::this_thread::sleep_until(wait_until); 
			}
		});


		std::cout << "press enter to exit....\n";
		std::cin.get(); // wait for user input
		should_exit = true;

		server_thread.join(); // wait for the thread to finish
		std::cout << "stopping server....\n";
	}
	catch (const std::exception& e) {
		std::cerr << "error: " << e.what() << std::endl;
	}

	return 0;
}