// Provides ROS 2 node, publisher, timer, and logging APIs.
#include "rclcpp/rclcpp.hpp"

// Provides the Float32 message type used to publish battery percentages.
#include "std_msgs/msg/float32.hpp"

// Provides the 1s duration literal used for the timer period.
#include <chrono>

// Provides std::max to clamp the simulated battery value at zero.
#include <algorithm>

// Provides std::make_shared for creating a shared-ownership node instance.
#include <memory>

// Enables chrono duration literals such as 1s instead of constructing a duration.
using namespace std::chrono_literals;

// A ROS 2 node that publishes a simulated battery percentage periodically.
class HealthPublisher : public rclcpp::Node
{
public:
    // Construct the node and initialize the simulated battery to 100 percent.
    HealthPublisher()
    // Register the node under the ROS graph name "health_publisher".
    // Initialize battery_percentage_ before callbacks can use it.
    : Node("health_publisher"), battery_percentage_(100.0f)
    {
        // Create a Float32 publisher on the battery percentage topic.
        // Queue depth 10 allows a limited backlog under the default QoS.
        publisher_ =
            this->create_publisher<std_msgs::msg::Float32>(
                "robot_health/battery_percentage", 10);

        // Start a wall-clock timer that invokes publish_health once per
        // second; storing it as a member keeps it active with the node.
        timer_ = this->create_wall_timer(
            1s,
            // Bind the member callback to this HealthPublisher instance.
            std::bind(&HealthPublisher::publish_health, this));

        // Log that the node finished setting up its publisher and timer.
        RCLCPP_INFO(get_logger(), "Robot health publisher started");
    }

private:
    // Publish the current simulated battery reading, then decrease the
    // internal reading for the next timer tick. Returns no value; its effects
    // are publishing, logging, and updating battery_percentage_.
    void publish_health()
    {
        // Create a fresh Float32 message for this publication.
        auto message = std_msgs::msg::Float32();

        // Copy the current simulated level into the outgoing message payload.
        message.data = battery_percentage_;

        // Send the message to subscribers on the battery percentage topic.
        publisher_->publish(message);

        // Report the value that was just published; %.1f formats one decimal.
        RCLCPP_INFO(get_logger(), "Battery: %.1f%%", message.data);

        // Reduce the simulated level only while it is positive, so it remains
        // at zero instead of continuing into negative percentages.
        if (battery_percentage_ > 0.0f) {
            // Subtract one percentage point and clamp the result to at least
            // zero. std::max prevents floating-point state from going below 0.
            battery_percentage_ =
                std::max(0.0f, battery_percentage_ - 1.0f);
        }
    }

    // Persistent simulated percentage; the callback updates it each second.
    float battery_percentage_;

    // Retain the publisher so the callback can publish throughout node life.
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr publisher_;

    // Retain the timer so periodic callback execution remains active.
    rclcpp::TimerBase::SharedPtr timer_;
};

// Program entry point; argc/argv contain command-line arguments from the OS.
int main(int argc, char * argv[])
{
    // Initialize ROS 2 and process ROS-specific command-line arguments.
    rclcpp::init(argc, argv);

    // Construct the node with shared ownership, as expected by spin().
    auto node = std::make_shared<HealthPublisher>();

    // Process timer callbacks and other ROS events; blocks until shutdown.
    rclcpp::spin(node);

    // Release ROS 2 resources after the event loop ends.
    rclcpp::shutdown();

    // Return success to the operating system.
    return 0;
}
