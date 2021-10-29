#include "TrafficLight.h"
#include <iostream>
#include <random>

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::waitForGreen() {
  // FP.5b : add the implementation of the method waitForGreen, in which an
  //infinite while - loop
  // runs and repeatedly calls the receive function on the message queue.
  // Once it receives TrafficLightPhase::green, the method returns.
  while(true){
      TrafficLightPhase curr_signal=message_queue.receive();
      if(curr_signal==TrafficLightPhase::green){
        std::cout<<"Thread id "<<std::this_thread::get_id()<<" received green"<<std::endl;
        return;
      }
  }
}
TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::red; }

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
  std::random_device rd;
  std::mt19937 eng(rd());
  std::uniform_int_distribution<> distr(4,6);
  int cycleDuration = distr(eng);
  std::cout<<"Cycle duration is "<<cycleDuration<<std::endl;
  // init stop watch
  lastUpdate = std::chrono::system_clock::now();
  while (true) {
    // sleep at every iteration to reduce CPU usage
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // compute time difference to stop watch
    long timeSinceLastUpdate =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - lastUpdate)
            .count();
    if (timeSinceLastUpdate/1000 >= cycleDuration) {
      if (_currentPhase == TrafficLightPhase::red) {
        _currentPhase = TrafficLightPhase::green;
        
      } else
        _currentPhase = TrafficLightPhase::red;
    
        TrafficLightPhase message=_currentPhase;
        lastUpdate = std::chrono::system_clock::now();
        message_queue.send(std::move(message));
    }
  }
}