
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

#include <chrono>
#include <functional>
#include <memory>

class RobotPublisher : public rclcpp::Node
{
public:
    RobotPublisher()
    : Node("robot_publisher")
    {   
        this->declare_parameter<std::string>(
              "status_message", "Robot is operational");

        publisher_ = this->create_publisher<std_msgs::msg::String>(
            "robot_status", 10);

        timer_ = this->create_wall_timer(
            std::chrono::seconds(1),
            std::bind(&RobotPublisher::publish_message, this));
    }

private:
    void publish_message()
    {
        auto message = std_msgs::msg::String();
        message.data = this->get_parameter("status_message").as_string();
        RCLCPP_INFO(
            this->get_logger(), "Publishing: '%s'",
            message.data.c_str());

        publisher_->publish(message);
    }

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<RobotPublisher>();

    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}
