
// Provides ROS 2 node, subscription, logging, and event-loop APIs.
#include "rclcpp/rclcpp.hpp"

// Provides the String message type this node receives.
#include "std_msgs/msg/string.hpp"

// Provides std::make_shared for creating a shared-ownership node instance.
#include <memory>

// A ROS 2 node that listens for status strings and logs each received message.
class RobotSubscriber : public rclcpp::Node
{
public:
    // Construct the node and register its subscription.
    RobotSubscriber()
    // Give this node the ROS graph name "robot_subscriber".
    : Node("robot_subscriber")
    {
        // Subscribe to String messages on "robot_status". Queue depth 10
        // allows a limited number of messages to wait if processing lags.
        subscription_ =
            this->create_subscription<std_msgs::msg::String>(
                "robot_status",
                10,
                // Capture this node so the callback can access its logger.
                // ROS supplies each received message as a shared pointer.
                [this](std_msgs::msg::String::SharedPtr message)
                {
                    // Log the received payload; c_str() provides the
                    // null-terminated string required by the "%s" formatter.
                    RCLCPP_INFO(
                        this->get_logger(),
                        "Received: '%s'",
                        message->data.c_str());
                });
    }

private:
    // Retain the subscription so it stays registered and can receive messages
    // for the lifetime of this node.
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

// Program entry point; argc/argv contain command-line arguments from the OS.
int main(int argc, char * argv[])
{
    // Initialize ROS 2 and process ROS-specific command-line arguments.
    rclcpp::init(argc, argv);

    // Construct the node with shared ownership, which spin() expects.
    auto node = std::make_shared<RobotSubscriber>();

    // Process incoming ROS messages and callbacks; normally blocks until shutdown.
    rclcpp::spin(node);

    // Clean up ROS 2 resources once the event loop ends.
    rclcpp::shutdown();

    // Return success to the operating system.
    return 0;
}
