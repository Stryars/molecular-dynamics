// Copyright 2018, Samuel Diebolt <samuel.diebolt@espci.fr>

#pragma once

#include <vector>
#include <queue>
#include <functional>
#include <string>

#include "include/particle.h"
#include "include/event.h"

class CollisionSystem {
 public:
  // Initializes a system with the specified collection of particles.
  explicit CollisionSystem(std::vector<Particle> particles, double friction);

  // Empty constructor: prevents a segmentation fault.
  ~CollisionSystem();

  // Updates priority queue with all new events for particle a.
  void Predict(Particle* a, double wall_size, double wall_speed);

  // Empties the priority queue and predicts all future events.
  void RegenerateEvents(double wall_size, double wall_speed);

  // Redraws all particles.
  void Redraw(bool isosurface);

  // Pauses the simulation.
  void Pause(sf::Keyboard::Key pause_key);

  void DrawText(const sf::Font& font, const std::string& str,
      int character_size, sf::Color color, int x, int y);

  // Displays helper text.
  void DisplayHelp(const sf::Font& font);

  // Displays physical quantities (temperature, pressure, etc.) and helper text.
  void DisplayCharacteristics(const sf::Font& font, time_t elapsed_time,
      int collisions, double average_kinetic_energy, double wall_size,
      double wall_speed, sf::Time frameTime);

  // Display the velocity histogram.
  void DisplayVelocityHistogram(double horizontal_scale,
      double average_kinetic_energy);

  // Simulates the system of particles for the specified amount of time.
  int Simulate();

 private:
  // The RenderWindow for the simulation
  sf::RenderWindow window_;

  // Number of redraw per clock tick
  double Hz_;

  // Priority queue
  std::priority_queue<Event, std::vector<Event>, std::greater<Event>> pq_;

  // Simulation clock time
  double time_;

  // Array of particles
  std::vector<Particle> particles_;

  // Friction coefficient
  double friction_;
};
