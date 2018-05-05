// Copyright 2018 <Samuel Diebolt>

#pragma once

#include <queue>
#include <functional>

#include "particle.h"
#include "event.h"

class CollisionSystem {
public:
  // Initializes a system with the specified collection of particles.
  CollisionSystem(std::vector<Particle>& particles);

  // Updates priority queue with all new events for particle a.
  void Predict(Particle* a);

  // Redraws all particles.
  void Redraw(sf::RenderWindow& window, sf::RectangleShape& box);

  // Pauses the simulation.
  void Pause(sf::RenderWindow& window, sf::Keyboard::Key pause_key);

  // Displays physical quantities (temperature, pressure, etc.) and helper text.
  void DisplayCharacteristics(sf::RenderWindow& window, sf::Font& font,
      time_t elapsed_time, int collisions, double average_kinetic_energy);

  // Display the velocity histogram.
  void DisplayVelocityHistogram(sf::RenderWindow& window);

  // Simulates the system of particles for the specified amount of time.
  void Simulate();

private:
  // Number of redraw per clock tick
  double Hz_;

  // Priority queue
  std::priority_queue<Event, std::vector<Event>, std::greater<Event>> pq_;

  // Simulation clock time
  double time_;

  // Array of particles
  std::vector<Particle> particles_;
};
