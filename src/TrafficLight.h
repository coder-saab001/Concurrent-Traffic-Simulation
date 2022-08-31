#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>

#include <random>
#include <bits/stdc++.h>

#include "TrafficObject.h"

// Forward declaration to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue {
public:
    void send(T &&LightPhaseState);
    T receive();

private:
    std::deque<T> _queue;
    std::mutex mtx;
    std::condition_variable _cond;
};

class TrafficLight : public TrafficObject {
public:
    enum TrafficLightPhase {red, green};

    // Constructor/Destructor
    TrafficLight();

    // Getters/Setters
    TrafficLightPhase getCurrentPhase(); //get Light phase

    // Typical behaviour method
    void TogglePhase();
    int RandomTime(); // Generate random time for traffic lights
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases();
    TrafficLightPhase _currentPhase;

    MessageQueue<TrafficLight::TrafficLightPhase> LightPhaseQ;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif