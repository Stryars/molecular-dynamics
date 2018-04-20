// Copyright 2018 <Samuel Diebolt>

#include <iostream>
#include <sstream>

#include "SFML/Graphics.hpp"
#include "main.h"
#include "simulation.h"
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
  std::vector<Particle>* particles = new std::vector<Particle>(n, Particle());

  // Initialization of the collision system
  CollisionSystem* system = new CollisionSystem {particles, 10000};

  // Initialization of the simulation
  system->Simulate(10000);

  delete particles;
  delete system;

  return 0;
}
