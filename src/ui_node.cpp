#include "rclcpp/rclcpp.hpp"
#include <iostream>
#include <memory>
#include <string>
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/bool.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <chrono>


class UINode : public rclcpp::Node {

    public:
    UINode() : Node("ui_node"){
        RCLCPP_INFO(this->get_logger(), "Velocity Input Node has been started.");
        publisher1_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        publisher2_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10);
        stop_sub = this->create_subscription<std_msgs::msg::Bool>("/stop_cmd",10, std::bind(&UINode::stopCallBack, this, std::placeholders::_1));
        stopping = false;

    }
    void run(){
        while(rclcpp::ok())
        {
            getting_input();
            sendVelocity(std::stof(turtle));
        }
    }

    private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher1_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher2_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr stop_sub;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::TimerBase::SharedPtr goback_timer;
    std::string turtle;
    std::string linear_vel;
    std::string angular_vel;
    bool stopping;

    void stopCallBack(const std_msgs::msg::Bool::SharedPtr stop_msg){
        stopping = stop_msg->data;
        RCLCPP_INFO(this->get_logger(), "Stopping message Recieved: value:%d", stop_msg->data);
    }
    void goBack(const rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub, float linear_vel ){
        geometry_msgs::msg::Twist back_msg;
        if(linear_vel < 0) back_msg.linear.x = 2;
        if(linear_vel > 0) back_msg.linear.x = -2;

        rclcpp::Rate rate(10);
        auto start = this->now();
        while ((this->now()-start).seconds() < 1.0)
        {
            rclcpp::spin_some(this->get_node_base_interface());
            RCLCPP_INFO(this->get_logger(), "GOING BACK: x= %f", back_msg.linear.x);
            pub->publish(back_msg);
            rate.sleep();
        }
        back_msg.linear.x = 0.0;
        back_msg.angular.z = 0.0;
        pub->publish(back_msg);
        RCLCPP_INFO(this->get_logger(), "Go back completed");
        
    
        
    }

    void sendVelocity(int turtle_choice){
        geometry_msgs::msg::Twist twist;
        twist.linear.x = std::stof(linear_vel);
        twist.angular.z = std::stof(angular_vel);

        auto publisher = (turtle_choice == 1) ? publisher1_ : publisher2_;
    
        auto start_time = this->now();
        rclcpp::Rate rate(10);

        bool collision = false;
        
        while ((this->now() - start_time).seconds() < 1.0) {
            rclcpp::spin_some(this->get_node_base_interface()); //permette agli altri sub e pub di fare le loro task
            if(!stopping){
                publisher->publish(twist);
                RCLCPP_INFO(this->get_logger(), "Moving lx = %f, ax = %f", twist.linear.x, twist.linear.y);
            }
            else{
                collision = true;
                RCLCPP_WARN(this->get_logger(), "Collision detected!");
                break;
            }
            rate.sleep();
        }

         // Ferma prima di tutto
        twist.linear.x = 0.0;
        twist.angular.z = 0.0;
        publisher->publish(twist);

        // IMPORTANTE: Processa ancora messaggi dopo aver fermato
        for(int i = 0; i < 5; i++) {
            rclcpp::spin_some(this->get_node_base_interface());
            rclcpp::sleep_for(std::chrono::milliseconds(10));
        }

        if(collision){
            //go back a little bit
            goBack(publisher, stof(linear_vel));
        }

    }

    
    void getting_input(){
    
        std::cout << "Choose turtle: " << std::endl;
        std::cin >> turtle;
        std::cout << "Choose linear vel: " << std::endl; 
        std::cin >> linear_vel;
        std::cout << "Choose angular vel: " << std::endl; 
        std::cin >> angular_vel;
    };


};

int main(int argc, char * argv[]){

    rclcpp::init(argc, argv);
    std::shared_ptr<UINode> node = std::make_shared<UINode>();
    node ->run();
    rclcpp::shutdown();
    return 0;


}