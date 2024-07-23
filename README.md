# libreach

libreach is a C++ library designed to interface with [Reach Robotics](https://reachrobotics.com/)
devices. Get started with libreach by installing the project or by exploring
the implemented [examples](https://github.com/Robotic-Decision-Making-Lab/libreach/tree/main/examples).

> :warning: This project is not affiliated with or maintained by Reach Robotics.
> Please refer to the [Reach Robotics SDK](https://github.com/Reach-Robotics/reach_robotics_sdk/tree/master)
> for all official software.

## Dependencies

libreach depends on [Boost](https://www.boost.org/). Please verify that Boost
has been installed prior to installing this project.

## Installation

To install libreach, first build the library using CMake

```bash
cmake -S . -B build && \
cmake --build build
```

Then, install the generated binary tree

```bash
cmake --install build
```

> :warning: For ROS 2 installations, please refer to the installation instructions
> provided on the [ros2](https://github.com/Robotic-Decision-Making-Lab/libreach/tree/ros2)
> branch.

## Getting help

If you have questions regarding usage of libreach or regarding contributing to
this project, please ask a question on our [Discussions](https://github.com/Robotic-Decision-Making-Lab/libreach/discussions)
board.

## License

libreach is released under the Reach Robotics [software license](https://github.com/Reach-Robotics/reach_robotics_sdk/blob/master/LICENSE.txt).
