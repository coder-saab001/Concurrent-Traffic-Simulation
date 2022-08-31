#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <random>

#include "Street.h"
#include "Intersection.h"
#include "Vehicle.h"

/* Implementation of class 'WaitingVehicles' */

int WaitingVehicles::getSize() {
    std::lock_guard<std::mutex> lock(_mutex);
    return _vehicles.size();
}

void WaitingVehicles::pushBack(std::shared_ptr<Vehicle> vehicle, std::promise<void> &&promise) {
    std::lock_guard<std::mutex> lock(_mutex);

    _vehicles.push_back(vehicle);
    _promises.push_back(std::move(promise));
}

void WaitingVehicles::permitEntryToFirstInQueue() {
    std::lock_guard<std::mutex> lock(_mutex);

    // Get entry fron the front of both queues
    auto firstPromise = _promises.begin();
    auto firstVehicle = _vehicles.begin();

    // Fulfill promise and send signal back that permission to enter has been been granted
    firstPromise->set_value();

    // Remove first element from both queues
    _vehicles.erase(firstVehicle);
    _promises.erase(firstPromise);
}

/* Implementation of class 'Intersection' */

Intersection::Intersection() {
    _type = ObjectType::objectIntersection;
    _isBlocked = false;
}

void Intersection::addStreet(std::shared_ptr<Street> street) {
    _streets.push_back(street);
}

std::vector<std::shared_ptr<Street>> Intersection::queryStreets(std::shared_ptr<Street> incoming) {
    // Store all outgoing streets in a vector
    std::vector<std::shared_ptr<Street>> outgoing;
    for(auto it : _streets) {
        // Except the street making the inquiry
        if(incoming->getID() != it->getID()) {
            outgoing.push_back(it);
        }
    }
    return outgoing;
}

// Add a new vehicle to the queue and return once the vehicle is allowed to enter
void Intersection::addVehicleToQueue(std::shared_ptr<Vehicle> vehicle) {
    std::unique_lock<std::mutex> lck(_mtx);
    std::cout << "Intersection #" << _id << "::addVehicleToQueue: thread id = " << std::this_thread::get_id() << std::endl;
    lck.unlock();

    // Add new vehicle to end of waiting line
    std::promise<void> prmsVehicleAllowedToEnter;
    std::future<void> ftrVehicleAllowedToEnter = prmsVehicleAllowedToEnter.get_future();
    _waitingVehicles.pushBack(vehicle, std::move(prmsVehicleAllowedToEnter));

    // Wait until the vehicle is allowed to enter
    ftrVehicleAllowedToEnter.wait();
    lck.lock();
    std::cout << "Intersection #" << _id << ": Vehicle #" << vehicle->getID() << " is granted entry." << std::endl;

    if(_trafficLight.getCurrentPhase() == TrafficLight::TrafficLightPhase::red) {
        _trafficLight.waitForGreen();
    }
    lck.unlock();
}

void Intersection::vehicleHasLeft(std::shared_ptr<Vehicle> vehicle) {
    //std::cout << "Intersection #" << _id << ": Vehicle #" << vehicle->getID() << " has left." << std::endl;
    this->setIsBlocked(false);
}

void Intersection::setIsBlocked(bool isBlocked) {
    _isBlocked = isBlocked;
    //std::cout << "Intersection #" << _id << " isBlocked=" << isBlocked << std::endl;
}

// Virtual function which is executed in thread
void Intersection::simulate() { // Using threads + promises/future + exceptions
    _trafficLight.simulate();
    // Launch vehicle queue processing in a thread
    threads.emplace_back(std::thread(&Intersection::processVehicleQueue, this));
}

void Intersection::processVehicleQueue() {
    while(true) {
        // Sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Only procees when atleast one vehicle is waiting in the queue
        if(_waitingVehicles.getSize() > 0 && !_isBlocked) {
            // set intersection to 'blocked' to prevent other vehicles from entering
            this->setIsBlocked(true);

            // permit entry to first vehicle in the queue (FIFO)
            _waitingVehicles.permitEntryToFirstInQueue();
        }
    }
}

bool Intersection::trafficLightIsGreen() {
    if(_trafficLight.getCurrentPhase() == TrafficLight::TrafficLightPhase::green) 
        return true;
    else 
        return false;

    // return true; //makes traffic light permanently green
}