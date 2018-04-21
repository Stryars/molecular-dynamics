// Copyright 2018 <Samuel Diebolt>

#include <iostream>
#include <random>
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

  std::mt19937 rng {std::random_device()()};
  std::uniform_real_distribution<double> random_position(0.11, 0.89);
  std::uniform_real_distribution<double> random_speed(-0.02, 0.02);
  std::uniform_real_distribution<double> random_radius(0.01, 0.02);
  std::uniform_int_distribution<int> random_color(0, 255);

  // Initialization of the particles collection
  std::vector<Particle> particles {};
  for (auto i {0}; i < n; ++i) {
    particles.push_back(Particle(random_position(rng), random_position(rng),
        random_speed(rng), random_speed(rng),
        random_radius(rng),
        1,
        sf::Color(random_color(rng), random_color(rng), random_color(rng), 255)));
  }

  // Initialization of the collision system
  const int limit {1000};
  CollisionSystem system {particles, limit};

  // Initialization of the simulation
  system.Simulate(limit);

  return 0;
}
