#include "TrafficLight.h"
#include <iostream>
#include <random>
template <class T> T MessageQueue<T>::receive() {
  // perform vector modification under the lock
  std::unique_lock<std::mutex> uLock(_mutex);
  _cond.wait(uLock, [this] {
    return !_queue.empty();
  }); // pass unique lock to condition variable

  // remove last vector element from queue
  T v = std::move(_queue.front());

  _queue.clear();
  return v; // will not be copied due to return value optimization (RVO) in
            // C++
}
template <class T> void MessageQueue<T>::send(T &&v) {
  // perform vector modification under the lock
  std::lock_guard<std::mutex> uLock(_mutex);

  // add vector to queue
  _queue.emplace_back(std::move(v));
  _cond.notify_all(); // notify client after pushing new Vehicle into vector
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::waitForGreen() {
  // FP.5b : add the implementation of the method waitForGreen, in which an
  // infinite while - loop
  // runs and repeatedly calls the receive function on the message queue.
  // Once it receives TrafficLightPhase::green, the method returns.
  while (true) {
    TrafficLightPhase curr_signal = message_queue.receive();
    if (curr_signal == TrafficLightPhase::green) {
      return;
    }
  }
}
std::random_device TrafficLight::rd;
std::mt19937 TrafficLight::eng(rd());
TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::red; }
TrafficLight::~TrafficLight(){};

void TrafficLight::simulate() {
  cycleThroughPhases();
  // FP.2b : Finally, the private method „cycleThroughPhases“ should be
  //   started in a thread when the public method „simulate“ is called.To do
  //   this,use the thread queue in the base class.
}
void TrafficLight::cycleThroughPhases() {
  // FP.2a : Implement the function with an infinite loop that measures the time
  // between two loop cycles and toggles the current phase of the traffic light
  // between red and green and sends an update method to the message queue using
  // move semantics. The cycle duration should be a random value between 4 and 6
  // seconds. Also, the while-loop should use std::this_thread::sleep_for to
  // wait 1ms between two cycles.
  std::chrono::time_point<std::chrono::system_clock> lastUpdate;

  std::uniform_real_distribution<> distr(4000, 6000);
  int cycleDuration = distr(eng);
  // init stop watch
  lastUpdate = std::chrono::system_clock::now();
  long timeSinceLastUpdate;
  while (true) {
    // sleep at every iteration to reduce CPU usage
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // compute time difference to stop watch
    timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
                              std::chrono::system_clock::now() - lastUpdate)
                              .count();
    if (timeSinceLastUpdate >= cycleDuration) {
      if (_currentPhase == TrafficLightPhase::red) {
        _currentPhase = TrafficLightPhase::green;

      } else
        _currentPhase = TrafficLightPhase::red;

      TrafficLightPhase message = _currentPhase;
      lastUpdate = std::chrono::system_clock::now();
      message_queue.send(std::move(message));
    }
  }
}