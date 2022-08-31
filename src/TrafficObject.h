#ifndef TRAFFICOBJECT_H
#define TRAFFICOBJECT_H

#include <vector>
#include <thread>
#include <mutex>

enum ObjectType {
    noObject,
    objectVehicle,
    objectIntersection,
    objectStreet,
};

class TrafficObject {
public:
    // Constructor and Destructor
    TrafficObject();
    ~TrafficObject();

    // Getters and Setters
    int getID() { return _id; }
    void setPosition(double x, double y);
    void getPosition(double &x, double &y);
    ObjectType getType() { return _type; }

    // Behavior methods
    virtual void simulate(){}

protected:
    ObjectType _type;                 // identifies the class type
    int _id;                          // Unique ID of every traffic object
    double _posX, _posY;              // Vehicle position (in pixels)
    std::vector<std::thread> threads; // Holds all the threads that have been launched within this object
    static std::mutex _mtx;           // Mutex shared by all traffic objects for protecting cout

private:
    static int _idCnt;                // Global variables for counting object IDs
};

#endif