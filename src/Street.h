#ifndef STREET_H
#define STREE_H

#include "TrafficObject.h"

// Forward declaration to avoid include cycle
class Intersection;

class Street : public TrafficObject, public std::enable_shared_from_this<Street> {
public:
    // Constructor / Destructor
    Street();

    // Getters / Setters
    double getLength() { return _length; }
    void setInIntersation(std::shared_ptr<Intersection> in);
    void setOutIntersation(std::shared_ptr<Intersection> out);
    std::shared_ptr<Intersection> getOutIntersection() { return _interOut; }
    std::shared_ptr<Intersection> getInIntersection() { return _interIn; }

    // Miscellaneous
    std::shared_ptr<Street> get_shared_this() { return shared_from_this(); }

private:
    double _length;                                    // Length of this street in m
    std::shared_ptr<Intersection> _interIn, _interOut; // intersections from which a vehicle can enter (one-way streets is always from 'in' to 'out') 
};

#endif