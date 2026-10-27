
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

#include <memory>

class RobotSubscriber : public rclcpp::Node
{
public:
    RobotSubscriber()
    : Node("robot_subscriber")
    {
        subscription_ =
            this->create_subscription<std_msgs::msg::String>(
                "robot_status",
                10,
                [this](std_msgs::msg::String::SharedPtr message)
                {
                    RCLCPP_INFO(
                        this->get_logger(),
                        "Received: '%s'",
                        message->data.c_str());
                });
        
    }

private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<RobotSubscriber>();

    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}
