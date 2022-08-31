#include <iostream>
#include <random>

#include "Intersection.h"
#include "Street.h"
#include "Vehicle.h"

Vehicle::Vehicle() {
    _currStreet = nullptr;
    _posStreet = 0.0;
    _type= ObjectType::objectVehicle;
    _speed = 400; // m/s
}

void Vehicle::SetCurrentDestination(std::shared_ptr<Intersection> destination) {
    // Update destination
    _currDestination = destination;

    // Reset other simulation parameters
    _posStreet = 0.0;
}

void Vehicle::simulate() {
    // Launch drive function in a thread
    threads.emplace_back(std::thread(&Vehicle::drive, this));
}

// Virtual function which is executed in a thread
void Vehicle::drive() {
    // Print ID of the current thread
    std::unique_lock<std::mutex> lck(_mtx);
    std::cout << "Vehicle #" << _id << "::drive: thread id = " << std::this_thread::get_id() << std::endl;
    lck.unlock();

    // Initialize variables
    bool hasEnteredIntersection = false;
    double cycleDuration = 1; // Duration of a single simulation in ms
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;

    // Initialize stop watch
    lastUpdate = std::chrono::system_clock::now();
    while(true) {
        // Sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Computer time differene to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if(timeSinceLastUpdate >= cycleDuration) {
            // Update position with a constant velocity motion model
            _posStreet += (_speed * timeSinceLastUpdate) / 1000;

            // Compute completion rate of current street
            double completion = _posStreet/_currStreet->getLength();

            // Compute current pixel position on street based on driving direction
            std::shared_ptr<Intersection> i1, i2;
            i2 = _currDestination;
            i1 = i2->getID() == _currStreet->getInIntersection()->getID() ? _currStreet->getOutIntersection() : _currStreet->getInIntersection();

            double x1, y1, x2, y2, xv, yv, dx, dy, l;
            i1->getPosition(x1, y1);
            i2->getPosition(x2, y2);
            dx = x2 - x1;
            dy = y2 - y1;
            l = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (x1 - x2));
            xv = x1 + completion * dx;
            yv = y1 + completion * dy;
            this->setPosition(xv, yv);

            // Check whether halting position in front of destination has been reached
            if(completion >=0.9 && !hasEnteredIntersection) {
                // Request entry to thr current intersection (using async)
                auto ftrEntryGranted = std::async(&Intersection::addVehicleToQueue, _currDestination, get_shared_this());

                // Wait until entry has been granted
                ftrEntryGranted.get();

                // Slow doen and set intersection flag
                _speed /= 10.0;
                hasEnteredIntersection = true;
            }

            // Check whether intersection has been crossed
            if(completion >= 1.0 && hasEnteredIntersection) {
                // Choose next street and destination
                std::vector<std::shared_ptr<Street>> streetOptions = _currDestination->queryStreets(_currStreet);
                std::shared_ptr<Street> nextStreet;
                if(streetOptions.size() > 0) {
                    // Pick one street at random and query intersection to enter this street
                    std::random_device rd;
                    std::mt19937 eng(rd());
                    std::uniform_int_distribution<> distr(0, streetOptions.size() - 1);
                    nextStreet = streetOptions.at(distr(eng));
                }
                else {
                    // This street is a dead-end, so drive back the same way
                    nextStreet = _currStreet;
                }

                // Pick the one intersection at which the vehicle is currently not
                std::shared_ptr<Intersection> nextIntersection = nextStreet->getInIntersection()->getID() == _currDestination->getID() ? nextStreet->getOutIntersection() : nextStreet->getInIntersection(); 

                // Send signal to intersection that vehicle has left the intersection
                _currDestination->vehicleHasLeft(get_shared_this());

                // Assign new street and new destination
                this->SetCurrentDestination(nextIntersection);
                this->setCurrentStreet(nextStreet);

                // Reset speed and intersection flag
                _speed *= 10.0;
                hasEnteredIntersection = false;
            }

            //reset stop watch for next cycle
            lastUpdate = std::chrono::system_clock::now();
        }   
    } // eof simulation loop
}
