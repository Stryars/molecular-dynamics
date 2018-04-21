// Copyright 2018 <Samuel Diebolt>

#pragma once

#include <queue>
#include <functional>

#include "particle.h"
#include "event.h"

class CollisionSystem {
public:
  // Initializes a system with the specified collection of particles.
  CollisionSystem(const std::vector<Particle>& particles, double limit);

  // Deletes the priority queue.
  ~CollisionSystem();

  // Updates priority queue with all new events for particle a
  void Predict(Particle* a, double limit);

  // Redraw all particles
  void Redraw(sf::RenderWindow& window, double limit);

  // Simulates the system of particles for the specified amount of time
  void Simulate(double limit);

private:
  // Number of redraw per clock tick
  double Hz_;

  // Priority queue
  std::priority_queue<Event, std::vector<Event>, std::greater<Event>>* pq_;

  // Simulation clock time
  double time_;

  // Array of particles
  std::vector<Particle> particles_;
};
