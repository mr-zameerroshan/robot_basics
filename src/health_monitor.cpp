// Provides ROS 2 node, subscription, logging, and event-loop APIs.
#include "rclcpp/rclcpp.hpp"

// Provides the Float32 message type used for battery percentages.
#include "std_msgs/msg/float32.hpp"

// Provides std::make_shared for creating the node with shared ownership.
#include <memory>

// A ROS 2 node that observes battery messages and reports threshold changes.
class HealthMonitor : public rclcpp::Node
{
public:
    // Construct the monitor and initialize its low-battery tracking state.
    HealthMonitor()
    // Register the ROS node under this name and start in normal state;
    // false means no low-battery warning hasnt been issued yet.
    : Node("health_monitor"), low_battery_(false)
    {
        // Declare a configurable threshold with a default of 20 percent.
        // ROS parameter overrides can provide a different threshold at startup.
        this->declare_parameter<double>("low_battery_threshold", 20.0);

        // Subscribe to the battery percentage topic. The queue depth of 10
        // lets ROS retain a limited number of messages if callbacks lag.
        subscription_ =
            this->create_subscription<std_msgs::msg::Float32>(
                "robot_health/battery_percentage",
                10,
                // Capture this node so the callback can invoke its method.
                // ROS supplies each received message through this shared pointer.
                [this](std_msgs::msg::Float32::SharedPtr message)
                {
                    // Forward the numeric payload to the comparison and
                    // state-transition logic; this call returns no value.
                    check_battery(message->data);
                });

        // Announce successful construction; logging has no effect on node state.
        RCLCPP_INFO(get_logger(), "Robot health monitor started");
    }

private:
    // Compare one received battery reading with the configured threshold.
    // The function returns no value; it logs status and updates alert state.
    void check_battery(float battery)
    {
        // Read the current threshold for each reading so an updated parameter
        // can affect later comparisons. The ROS parameter is represented here
        // as a double, while the incoming message value is a float.
        const double threshold =
            this->get_parameter("low_battery_threshold").as_double();

        // Detect the transition from normal to low battery. The state check
        // prevents repeating the warning for every message while still low.
        if (battery <= threshold && !low_battery_) {
            // Report the low reading with one digit after the decimal point.
            RCLCPP_WARN(
                get_logger(), "LOW BATTERY: %.1f%% remaining", battery);

            // Remember that the warning condition is active so subsequent
            // readings can be handled as continuing low-battery state.
            low_battery_ = true;
        // Detect recovery above the threshold, but only if a low warning was
        // previously active; this makes the log describe a real state change.
        } else if (battery > threshold && low_battery_) {
            // Report that this reading has crossed back into the normal range.
            RCLCPP_INFO(
                get_logger(), "Battery returned to normal: %.1f%%", battery);

            // Clear the active-warning state so a future drop can warn again.
            low_battery_ = false;
        // For normal readings, log the value only when no low warning is active.
        // While low, the first branch already warned and avoids repeated logs.
        } else if (!low_battery_) {
            RCLCPP_INFO(
                get_logger(), "Battery level normal: %.1f%%", battery);
        }
    }

    // Tracks whether the most recently handled state is low battery; this
    // boolean is what suppresses duplicate warnings until recovery occurs.
    bool low_battery_;

    // Retain the subscription so it stays registered and can receive messages
    // for the lifetime of the monitor node.
    rclcpp::Subscription<
        std_msgs::msg::Float32>::SharedPtr subscription_;
};

// Program entry point; argc/argv contain command-line arguments from the OS.
int main(int argc, char * argv[])
{
    // Initialize ROS 2 and process ROS-specific command-line arguments.
    rclcpp::init(argc, argv);

    // Construct the node using shared ownership, which spin() expects.
    auto node = std::make_shared<HealthMonitor>();

    // Process incoming ROS messages and callbacks; normally blocks until shutdown.
    rclcpp::spin(node);

    // Clean up ROS 2 resources once spinning ends.
    rclcpp::shutdown();

    // Tell the operating system the program completed normally.
    return 0;
}