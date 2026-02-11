#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include <memory>
#include "std_msgs/msg/float32.hpp"
#include <chrono>
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/bool.hpp"
#include "custom_interface/srv/tresh_exam.hpp"

class DistanceNode : public rclcpp::Node {
    public:
    DistanceNode() : Node("distance_node")
    {
        RCLCPP_INFO(this-> get_logger(),"NODE CREATED");
        subscriber1_ = this->create_subscription<turtlesim::msg::Pose>
        ("/turtle1/pose", 10, std::bind(&DistanceNode::gettin_pose1, this, std::placeholders::_1));
        subscriber2_ = this->create_subscription<turtlesim::msg::Pose>
        ("/turtle2/pose", 10, std::bind(&DistanceNode::gettin_pose2, this, std::placeholders::_1));
        pub = this->create_publisher<std_msgs::msg::Float32>("/distance", 10);
        timer_ = this->create_wall_timer(std::chrono::milliseconds(10), std::bind(&DistanceNode::distance_callback,this));
        turtle1_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel",10);
        turtle2_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel",10);
        stop_pub_ = this->create_publisher<std_msgs::msg::Bool>("/stop_cmd", 10);
         tresh_service = this->create_service<custom_interface::srv::TreshExam>(
            "set_treshold",
            //lamba
            [this](const std::shared_ptr<custom_interface::srv::TreshExam::Request> request, 
                std::shared_ptr<custom_interface::srv::TreshExam::Response> response){
                if(request->value <= 0.0){
                    response->ack = false; 
                    return;
                }
                if(request->sign == "increase"){
                    this->tresh = this->tresh + request->value;
                }else this->tresh = this->tresh - (request->value);
                
                RCLCPP_INFO(this->get_logger(), "Treshold updated to: %f", this->tresh);
                response->ack = true; //aknowledgmentl
            }
        );

    }
    private:
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscriber1_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscriber2_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr pub;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr turtle1_vel_pub_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr turtle2_vel_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr stop_pub_;


    //new part
    rclcpp::Service<custom_interface::srv::TreshExam>::SharedPtr tresh_service;


    rclcpp::TimerBase::SharedPtr timer_;
    turtlesim::msg::Pose pose1;
    turtlesim::msg::Pose pose2;
    double  distance = 1000;
    double bound_max = 10.0;
    double bound_min = 1.0;
    double tresh = 0.5;

    bool boundaries(const turtlesim::msg::Pose pose ){
        return (pose.x < bound_min || pose.x > bound_max ||
                pose.y < bound_min || pose.y > bound_max );
    }
    void gettin_pose1(const turtlesim::msg::Pose::SharedPtr pose){
        pose1.x = pose -> x;
        pose1.y = pose -> y;
    }
    void gettin_pose2(const turtlesim::msg::Pose::SharedPtr pose){
        pose2.x = pose -> x;
        pose2.y = pose -> y;
    }
    void compute_distance(){
        distance = std::sqrt(std::pow(pose1.x - pose2.x, 2) +
                     std::pow(pose1.y - pose2.y, 2));
        RCLCPP_INFO(this->get_logger(), "DISTANCE COMPUTED: %f", distance);
    }
    void stop(int turtle) {
        auto stop_msg = geometry_msgs::msg::Twist();
        stop_msg.linear.x = 0.0;
        stop_msg.linear.y = 0.0;



        if (turtle == 1) {
            turtle1_vel_pub_->publish(stop_msg);


        } else {
            turtle2_vel_pub_->publish(stop_msg);
        }

    }

    void distance_callback(){
        compute_distance();
        std_msgs::msg::Bool stop_msg;
        if(distance >0)
        {
            std_msgs::msg::Float32 msg;
            msg.data = distance;
            pub->publish(msg);
        }
        if (distance < tresh) {
                RCLCPP_INFO(this->get_logger(), "Turtles too close! Distance: %.2f", distance);
                stop_msg.data = true;
                stop_pub_->publish(stop_msg);
                stop(1);
                stop(2);

        }
        else if(boundaries(pose1)){
            stop_msg.data = true;
            stop_pub_->publish(stop_msg);
            stop(1);
        }
        else if(boundaries(pose2)){
            stop_msg.data = true;
            stop_pub_->publish(stop_msg);
            stop(2);
        }
        else{
            stop_msg.data = false;
            stop_pub_->publish(stop_msg);
            
        }
    
    }


};


int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DistanceNode>());
    rclcpp::shutdown();
    return 0;

}