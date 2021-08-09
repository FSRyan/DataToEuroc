#include <iostream>
#include <fstream>
#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/highgui.hpp>

std::ofstream fg;
void imu_callback(const sensor_msgs::ImuConstPtr &imu_msg)
{
    long long int t = (imu_msg->header.stamp.toSec()-187602.0)*1e9; //直接获取的时间戳cam和IMU相差比较大，因此做个粗略的对齐，以免后面无法标定

    fg << t << "," << imu_msg->angular_velocity.x << "," << imu_msg->angular_velocity.y << "," << imu_msg->angular_velocity.z << "," << imu_msg->linear_acceleration.x << "," << imu_msg->linear_acceleration.y << "," << imu_msg->linear_acceleration.z << std::endl;

}

void img_callback(const sensor_msgs::ImageConstPtr &img_msg)
{
    long long int t = img_msg->header.stamp.toSec()*1e9;
    cv_bridge::CvImageConstPtr ptr;
    if(img_msg->encoding == "8UC1")
    {
        sensor_msgs::Image img;
        img.header = img_msg->header;
        img.height = img_msg->height;
        img.width = img_msg->width;
        img.is_bigendian = img_msg->is_bigendian;
        img.step = img_msg->step;
        img.data = img_msg->data;
        img.encoding = "mono8";
        ptr = cv_bridge::toCvCopy(img, sensor_msgs::image_encodings::MONO8);
    }
    else
        ptr = cv_bridge::toCvCopy(img_msg, sensor_msgs::image_encodings::MONO8);

    cv::Mat img_raw = ptr->image;
    std::string img_dir = "/home/fs/Downloads/dataset/android_euroc/cam0/"+ std::to_string(t) +".png";
    cv::imwrite(img_dir, img_raw);
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "date_to_euroc");
    ros::NodeHandle n("~");
    ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Info);

    fg.open("/home/fs/Downloads/dataset/android_euroc/imu0.csv");
    fg << std::fixed;
    fg << "timestamp" << "," << "omega_x" << "," << "omega_y" << "," << "omega_z" << "," << "alpha_x" << "," << "alpha_y" << "," << "alpha_z" << std::endl;

    ros::Subscriber sub_imu = n.subscribe("/android/imu", 2000, imu_callback, ros::TransportHints().tcpNoDelay());
    ros::Subscriber sub_img = n.subscribe("/cam0/image_raw", 200, img_callback);

    ros::spin();
}
