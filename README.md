# ROS 2 C++ Publisher and Subscriber

A beginner ROS 2 Jazzy project demonstrating inter-node communication using
C++, publishers, subscribers, timers, and runtime parameters.

## Features

- Publishes robot status messages every second.
- Subscribes to the robot status topic and logs received messages.
- Uses a configurable `status_message` parameter.
- Implements nodes using `rclcpp`.
- Uses `std_msgs/msg/String` for message communication.

## Requirements

- Ubuntu 24.04
- ROS 2 Jazzy
- C++ compiler and CMake
- `colcon` build tools

## Project Structure

```text
robot_basics/
├── CMakeLists.txt
├── package.xml
├── README.md
├── .gitignore
└── src/
    ├── publisher.cpp
    └── subscriber.cpp

```