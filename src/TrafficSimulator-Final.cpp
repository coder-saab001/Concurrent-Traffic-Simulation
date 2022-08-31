#include <iostream>
#include <thread>
#include <vector>

#include "Vehicle.h"
#include "Street.h"
#include "Intersection.h"
#include "Graphics.h"

// Paris
void createTrafficObjects_Paris(std::vector<std::shared_ptr<Street>> &streets,
                                std::vector<std::shared_ptr<Intersection>> &intersections,
                                std::vector<std::shared_ptr<Vehicle>> &vehicles,
                                std::string &filename,
                                int nVehices) {
    // Assigning filename of corresponding city map
    filename = "../data/paris.jpg";

    // Init traffic objects
    int nIntersections = 9;
    for(size_t ni = 0; ni < nIntersections; ni++) {
        intersections.push_back(std::make_shared<Intersection>());
    }

    // Position intersections in pixel coordinates (counter-clockwise)
    intersections.at(0)->setPosition(385, 270);
    intersections.at(1)->setPosition(1240, 80);
    intersections.at(2)->setPosition(1625, 75);
    intersections.at(3)->setPosition(2110, 75);
    intersections.at(4)->setPosition(2840, 175);
    intersections.at(5)->setPosition(3070, 680);
    intersections.at(6)->setPosition(2800, 1400);
    intersections.at(7)->setPosition(400, 1100);
    intersections.at(8)->setPosition(1700, 900); // Central Plaza

    // Create Streets and connect traffic objects
    int nStreets = 8;
    for(size_t ns = 0; ns < nStreets; ns++) {
        streets.push_back(std::make_shared<Street>());
        streets.at(ns)->setInIntersation(intersections.at(ns));
        streets.at(ns)->setOutIntersation(intersections.at(8));
    }

    // Add vehicles to streets
    for(size_t nv = 0; nv < nVehices; nv++) {
        vehicles.push_back(std::make_shared<Vehicle>());
        vehicles.at(nv)->setCurrentStreet(streets.at(nv));
        vehicles.at(nv)->SetCurrentDestination(intersections.at(8));
    }
}

// NYC
void createTrafficObjects_NYC(std::vector<std::shared_ptr<Street>> &streets,
                                std::vector<std::shared_ptr<Intersection>> &intersections,
                                std::vector<std::shared_ptr<Vehicle>> &vehicles,
                                std::string &filename,
                                int nVehices) {
    // Assign filename of corresponding city map
    filename = "../data/nyc.jpg";

    // Init traffic objects
    int nIntersections = 6;
    for(size_t ni = 0; ni < nIntersections; ni++) {
        intersections.push_back(std::make_shared<Intersection>());
    }

    // Position intersections in pixel coorinates
    intersections.at(0)->setPosition(1430, 625);
    intersections.at(1)->setPosition(2575, 1260);
    intersections.at(2)->setPosition(2200, 1950);
    intersections.at(3)->setPosition(1000, 1350);
    intersections.at(4)->setPosition(400, 1000);
    intersections.at(5)->setPosition(750, 250);

    // Create streets and connect traffic objects
    int nStreets = 7;
    for(size_t ns = 0; ns < nStreets; ns++) {
        streets.push_back(std::make_shared<Street>());
    }

    streets.at(0)->setInIntersation(intersections.at(0));
    streets.at(0)->setOutIntersation(intersections.at(1));

    streets.at(1)->setInIntersation(intersections.at(1));
    streets.at(1)->setOutIntersation(intersections.at(2));

    streets.at(2)->setInIntersation(intersections.at(2));
    streets.at(2)->setOutIntersation(intersections.at(3));

    streets.at(3)->setInIntersation(intersections.at(3));
    streets.at(3)->setOutIntersation(intersections.at(4));

    streets.at(4)->setInIntersation(intersections.at(4));
    streets.at(4)->setOutIntersation(intersections.at(5));

    streets.at(5)->setInIntersation(intersections.at(5));
    streets.at(5)->setOutIntersation(intersections.at(0));

    streets.at(6)->setInIntersation(intersections.at(0));
    streets.at(6)->setOutIntersation(intersections.at(3));

    // Add vehicles to streets
    for(size_t nv = 0; nv < nVehices; nv++) {
        vehicles.push_back(std::make_shared<Vehicle>());
        vehicles.at(nv)->setCurrentStreet(streets.at(nv));
        vehicles.at(nv)->SetCurrentDestination(intersections.at(nv));
    }
}

/* Main Function */
int main() {
    // Setting up traffic objects

    // Create and connect intersections and streets
    std::vector<std::shared_ptr<Street>> streets;
    std::vector<std::shared_ptr<Intersection>> intersections;
    std::vector<std::shared_ptr<Vehicle>> vehicles;
    std::string backgroundImg;
    int nVehicles = 6;
    createTrafficObjects_NYC(streets, intersections, vehicles, backgroundImg, nVehicles);

    // Simulate Traffic Objects

    // Simulate Intersection
    std::for_each(intersections.begin(), intersections.end(), [](std::shared_ptr<Intersection> &i) {
        i->simulate();
    });

    // Simulate Vehicles
    std::for_each(vehicles.begin(), vehicles.end(), [](std::shared_ptr<Vehicle> &v) {
        v->simulate();
    });

    // Launching Visualization

    // Adding all objects to common vector
    std::vector<std::shared_ptr<TrafficObject>> trafficObjects;
    std::for_each(intersections.begin(), intersections.end(), [&trafficObjects](std::shared_ptr<Intersection> &intersection) {
        std::shared_ptr<TrafficObject> trafficObject = std::dynamic_pointer_cast<TrafficObject>(intersection);
        trafficObjects.push_back(trafficObject);
    });

    std::for_each(vehicles.begin(), vehicles.end(), [&trafficObjects](std::shared_ptr<Vehicle> &vehicles) {
        std::shared_ptr<TrafficObject> trafficObject = std::dynamic_pointer_cast<TrafficObject>(vehicles);
        trafficObjects.push_back(trafficObject);
    });

    // Draw all objects in vector
    Graphics *graphics = new Graphics();
    graphics->setBgFilename(backgroundImg);
    graphics->setTrafficObject(trafficObjects);
    graphics->simulate();

    return 0;
}