# IKinema Sample VRPN server for streaming data into IKinema LiveAction 
IKinema 2016

The application consists of two main components:
- IKinema::vrpn::SkeletonTracker - a custom VRPN tracker that servers a single skeleton to the client implemented in LiveAction
- simple JSON animation file reader for demostration purposes

---
## Depends on:
* [VRPN](https://github.com/vrpn/vrpn)
* [GSL](https://github.com/Microsoft/GSL)
* [JsonCpp](https://github.com/open-source-parsers/jsoncpp)