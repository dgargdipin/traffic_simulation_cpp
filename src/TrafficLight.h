#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <mutex>

// forward declarations to avoid include cycle
class Vehicle;

// FP.3 Define a class „MessageQueue“ which has the public methods send and
// receive. Send should take an rvalue reference of type TrafficLightPhase
// whereas receive should return this type. Also, the class should define an
// std::dequeue called _queue, which stores objects of type TrafficLightPhase.
// Also, there should be an std::condition_variable as well as an std::mutex as
// private members.

template <class T> class MessageQueue {
public:
  MessageQueue() {}

  T receive() {
    // perform vector modification under the lock
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] {
      return !_queue.empty();
    }); // pass unique lock to condition variable

    // remove last vector element from queue
    T v = std::move(_queue.front());
    _queue.pop_front();

    return v; // will not be copied due to return value optimization (RVO) in
              // C++
  }

  void send(T &&v) {
    // simulate some work
    // perform vector modification under the lock
    std::lock_guard<std::mutex> uLock(_mutex);

    // add vector to queue
    _queue.emplace_back(std::move(v));
    _cond.notify_all(); // notify client after pushing new Vehicle into vector
  }

private:
  std::mutex _mutex;
  std::condition_variable _cond;
  std::deque<T>
      _queue; // list of all vehicles waiting to enter this intersection
};
// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject.
// The class shall have the public methods „void waitForGreen()“ and „void
// simulate()“ as well as „TrafficLightPhase getCurrentPhase()“, where
// TrafficLightPhase is an enum that can be either „red“ or „green“. Also, add
// the private method „void cycleThroughPhases()“. Furthermore, there shall be
// the private member _currentPhase which can take „red“ or „green“ as its
// value.
enum TrafficLightPhase { red, green };
class TrafficLight : public TrafficObject

{
public:
  void waitForGreen();
  void simulate();
  TrafficLightPhase getCurrentPhase();
  // constructor / desctructor
  TrafficLight();

  // getters / setters

  // typical behaviour methods

private:
  // typical behaviour methods
  void cycleThroughPhases();
  TrafficLightPhase _currentPhase;
  // FP.4b : create a private member of type MessageQueue for messages of type
  // TrafficLightPhase and use it within the infinite loop to push each new
  // TrafficLightPhase into it by calling send in conjunction with move
  // semantics.
  MessageQueue<TrafficLightPhase> message_queue;
  std::condition_variable _condition;
  std::mutex _mutex;
};

#endif