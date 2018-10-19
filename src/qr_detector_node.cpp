// headers in this package
#include <qr_detector/detector.h>

// headers for ros
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>

static ros::Publisher tagPublisher;
static qr_detector::Detector detector;

void imageCb(const sensor_msgs::ImageConstPtr& image)
{
    cv_bridge::CvImageConstPtr cv_image;

    try
    {
        cv_image = cv_bridge::toCvShare(image, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    auto tags = detector.detect(cv_image->image, 10);
    for (auto& tag : tags)
    {
        tagPublisher.publish(tag.message);
    }
    return;
}

int main(int argc, char *argv[]) {
    ros::init(argc, argv, "qr_detector_node");
    ros::NodeHandle nh;
    tagPublisher = nh.advertise<std_msgs::String>(ros::this_node::getName()+"/tag",10);
    image_transport::ImageTransport it(nh);
    image_transport::Subscriber imgSubscriber = it.subscribe("/image_raw", 1, imageCb);
    ros::spin();
    return 0;
}