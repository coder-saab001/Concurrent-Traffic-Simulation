#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include "TrafficLight.h"

class Graphics {
public:
    void setBgFilename(std::string filename) { _bgFilename = filename; }
    void setTrafficObject(std::vector<std::shared_ptr<TrafficObject>> &TrafficObjects) { _trafficObjects = TrafficObjects; };

    void simulate();

private:
    void loadBackgroundImg();
    void drawTrafficObject();

    // member variable
    std::vector<std::shared_ptr<TrafficObject>> _trafficObjects;
    std::string _bgFilename;
    std::string _windowName;
    std::vector<cv::Mat> _images;
};

#endif