#ifndef VEHICLE_H
#define VEHICLE_H

#include "TrafficObject.h"

//forward declarations to avoid include cycle
class Street;
class Intersection;

class Vehicle : public TrafficObject, public std::enable_shared_from_this<Vehicle> {
public:
    // constructor / destructor
    Vehicle();

    // Getters / Setters
    void setCurrentStreet(std::shared_ptr<Street> street) { _currStreet = street; }
    void SetCurrentDestination(std::shared_ptr<Intersection> destination);

    // Typical behaviour method
    void simulate();

    // Miscellaneous
    std::shared_ptr<Vehicle> get_shared_this() { return shared_from_this(); }

private:
    // Typical behaviour methods
    void drive();

    std::shared_ptr<Street> _currStreet;            // Street on which the vehicle is currently on
    std::shared_ptr<Intersection> _currDestination; // Destinatin to which the vehicle is currently driving
    double _posStreet;                              // Position on current street
    double _speed;                                  // ego speed in m/s
};

#endif