
// Provides ROS 2 node, publisher, timer, and logging APIs.
#include "rclcpp/rclcpp.hpp"

// Provides the String message type sent on the robot status topic.
#include "std_msgs/msg/string.hpp"

// Provides std::chrono::seconds for expressing the timer period.
#include <chrono>

// Provides std::bind for adapting a member function to the timer callback.
#include <functional>

// Provides std::make_shared for creating a shared-ownership node instance.
#include <memory>

// A ROS 2 node that publishes a configurable status message periodically.
class RobotPublisher : public rclcpp::Node
{
public:
    // Construct the node and set up its parameter, publisher, and timer.
    RobotPublisher()
    // Give this node the ROS graph name "robot_publisher".
    : Node("robot_publisher")
    {
        // Declare a string parameter and provide the default used when no
        // startup override supplies a different value.
        this->declare_parameter<std::string>(
              "status_message", "Robot is operational");

        // Create a String publisher for "robot_status"; queue depth 10
        // allows up to ten messages to be queued under the default QoS.
        publisher_ = this->create_publisher<std_msgs::msg::String>(
            "robot_status", 10);

        // Schedule the callback every second of wall-clock time. Retaining
        // the timer in a member keeps it active for the node's lifetime.
        timer_ = this->create_wall_timer(
            std::chrono::seconds(1),
            // Bind the callback to this object because it is a member
            // function and needs a RobotPublisher instance to run on.
            std::bind(&RobotPublisher::publish_message, this));
    }

private:
    // Build, log, and publish one status message each time the timer fires.
    // The function returns no value; its effects are ROS logging and publish.
    void publish_message()
    {
        // Create a new, empty ROS String message for this timer tick.
        auto message = std_msgs::msg::String();

        // Read the parameter at publication time so later parameter updates
        // can affect future messages, then store its string in the payload.
        message.data = this->get_parameter("status_message").as_string();

        // Log the value being sent. c_str() supplies the null-terminated
        // character sequence required by the "%s" format specifier.
        RCLCPP_INFO(
            this->get_logger(), "Publishing: '%s'",
            message.data.c_str()
                   );

        // Send the completed message on "robot_status"; this has a ROS
        // communication side effect and does not produce a value here.
        publisher_->publish(message);
    }

    // Keep the publisher available to the timer callback for sending messages.
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;

    // Keep the timer alive and associated with this node.
    rclcpp::TimerBase::SharedPtr timer_;
};

// Program entry point; argc/argv carry command-line arguments from the OS.
int main(int argc, char * argv[])
{
    // Initialize ROS 2 and process ROS-specific command-line arguments.
    rclcpp::init(argc, argv);

    // Construct the node under shared ownership, as required by spin().
    auto node = std::make_shared<RobotPublisher>();

    // Process ROS events and invoke callbacks; normally blocks until shutdown.
    rclcpp::spin(node);

    // Release ROS 2 resources after spinning ends.
    rclcpp::shutdown();

    // Report normal completion to the operating system.
    return 0;
}
