#include <iostream>
#include <random>

#include "TrafficLight.h"

template <typename T>
T MessageQueue<T>::receive() {
    std::unique_lock<std::mutex> oneLock(mtx);
    while(_queue.empty()) {
        _cond.wait(oneLock);
    }
    T msg = std::move(_queue.front());
    _queue.pop_front();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg) {
    std::lock_guard<std::mutex> oneLock(mtx);
    // _queue.clear();
    _queue.push_back(std::move(msg));
    _cond.notify_one();
}

/* Implementation of class TrafficLight */

TrafficLight::TrafficLight() {
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen() {
    while(true) {
        if(LightPhaseQ.receive() == TrafficLightPhase::green) {
            return;
        }
    }
}

TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase() {
    return _currentPhase;
}

// Toggling the phase between red and green
void TrafficLight::TogglePhase() {
    _currentPhase = (_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;
}

void TrafficLight::simulate() {
    TrafficObject::threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// Generating random time between 4 to 6 seconds
int TrafficLight::RandomTime() {
    std::random_device device;
    std::uniform_int_distribution<int> distribution(4000, 6000); // Creating a dist between 4 and 6 seconds
    std::mt19937 generator(device());
    return distribution(generator);
}

// Virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point t2;

    double timeBetweenLights = TrafficLight::RandomTime(); // selecting random time between 4 and 6

    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Sleeping for 1ms to reduce CPU load

        // Starting the clock now
        // collecting the time between first measurement and now
        t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count(); //Measuring the time difference in seconds
        if(duration >= timeBetweenLights) {
            timeBetweenLights = TrafficLight::RandomTime(); //Resetting time between lights to new random value
            TrafficLight::TogglePhase(); // Toggling the current phase
            LightPhaseQ.send(std::move(TrafficLight::getCurrentPhase())); //Sending a message to queue for _currentPhase update
            t1 = std::chrono::high_resolution_clock::now(); // Resetting the time here to current time value
        }
    } 
}