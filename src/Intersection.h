#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <vector>
#include <future>
#include <mutex>
#include <memory>
#include "TrafficObject.h"
#include "TrafficLight.h"

// Forward declaration to avoid cycle
class Street;
class Vehicle;

// Auxiliary class to queue and dequeue watiting vehicles in a thread-safe manner
class WaitingVehicles {
public:
    // Getter/Setter
    int getSize();

    // Typical behaviour method
    void pushBack(std::shared_ptr<Vehicle> vehicle, std::promise<void> &&promise);
    void permitEntryToFirstInQueue();

private:
    std::vector<std::shared_ptr<Vehicle>> _vehicles;  // List of all vehicles waiting to enter this intersection
    std::vector<std::promise<void>> _promises; // List of associated promises
    std::mutex _mutex;
};

class Intersection : public TrafficObject {
public:
    // Constructor/Destructor
    Intersection();

    // Getters/Setters
    void setIsBlocked(bool isBlocked);

    // Typical behaviour methods
    void addVehicleToQueue(std::shared_ptr<Vehicle> vehicle);
    void addStreet(std::shared_ptr<Street> street);
    std::vector<std::shared_ptr<Street>> queryStreets(std::shared_ptr<Street> incoming); // return pointer to current list of all outgoing streets
    void simulate();
    void vehicleHasLeft(std::shared_ptr<Vehicle> vehicle);
    bool trafficLightIsGreen();
private:
    // Typical Behaviour methods
    void processVehicleQueue();

    // private members
    std::vector<std::shared_ptr<Street>> _streets; // List of all streets connected to this intersection
    WaitingVehicles _waitingVehicles; // List of all vehicles and their associated promises waiting to enter the intersection
    bool _isBlocked;  // Flag indicating whether the intersection is blocked by vehicle
    TrafficLight _trafficLight;
};

#endif