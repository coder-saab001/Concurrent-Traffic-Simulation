#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "Graphics.h"
#include "Intersection.h"

void Graphics::simulate() {
    this->loadBackgroundImg();
    while(true) {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // update graphics
        this->drawTrafficObject();
    }
}

void Graphics::loadBackgroundImg() {
    // Create Window
    _windowName = "Concurrency Traffic Simulation";
    cv::namedWindow(_windowName, cv::WINDOW_NORMAL);

    // load image and create a copy to be used for semi-transparent overlay
    cv::Mat background = cv::imread(_bgFilename);
    _images.push_back(background);            // First element is the original background
    _images.push_back(background.clone());    // Second element will be the transparent overlay
    _images.push_back(background.clone());    // Third element will be the result image for display 
}

void Graphics::drawTrafficObject() {
    // Reset images
    _images.at(1) = _images.at(0).clone();
    _images.at(2) = _images.at(0).clone();

    /// Create overlay from all traffic objects
    for(auto it : _trafficObjects) {
        double posx, posy;
        it->getPosition(posx, posy);

        if(it->getType() == ObjectType::objectIntersection) {
            // Cast object type from TrafficObject to Intersection
            std::shared_ptr<Intersection> intersection = std::dynamic_pointer_cast<Intersection>(it);

            // Set color according to traffic light and draw intersection as a circle
            cv::Scalar trafficLightColor = intersection->trafficLightIsGreen() == true ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
            cv::circle(_images.at(1), cv::Point2d(posx, posy), 25, trafficLightColor, -1);
        }
        else if(it->getType() == ObjectType::objectVehicle) {
            cv::RNG rng(it->getID());
            int b = rng.uniform(0, 255);
            int g = rng.uniform(0, 255);
            int r = sqrt(255*255 - g*g - r*r); // ensure that length of color vector is always 255
            cv::Scalar vehicleColor = cv::Scalar(b, g, r);
            cv::circle(_images.at(1), cv::Point2d(posx, posy), 50, vehicleColor, -1);
        }
    }

    float opacity = 0.85;
    cv::addWeighted(_images.at(1), opacity, _images.at(0), 1.0 - opacity, 0, _images.at(2));

    // Display background and overlay image
    cv::Mat ReduceImageSize;
    cv::Size f;
    cv::resize(_images.at(2), ReduceImageSize, cv::Size(), 0.4, 0.4);
    cv::imshow(_windowName, ReduceImageSize);
    cv::waitKey(33);
}