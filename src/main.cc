// Copyright 2018 <Samuel Diebolt>

#include <iostream>
#include <sstream>

#include "SFML/Graphics.hpp"
#include "main.h"
#include "particle.h"
#include "collisionSystem.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Please enter the number of particles to simulate.\n");
    return 1;
  }

  std::istringstream ss {argv[1]};
  int n {0};
  if (!(ss >> n)) {
    std::cerr << "Invalid number " << argv[1] << '\n';
    return 1;
  }

  // Initialization of the particles collection
  std::vector<Particle> particles {};
  particles.push_back(Particle(0.05, 0.05, 0.04, 0, 0.01, 0.5, sf::Color::Black));

  // Initialization of the collision system
  const int limit {1000};
  CollisionSystem system {particles, limit};

  // Initialization of the simulation
  system.Simulate(limit);

  return 0;
}
