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
  double n {0};
  if (!(ss >> n)) {
    std::cerr << "Invalid number " << argv[1] << '\n';
    return 1;
  }

  // std::mt19937 rng {std::random_device()()};
  std::mt19937 rng {1};
  std::uniform_real_distribution<double> random_speed(-5, 5);
  std::uniform_int_distribution<int> random_color(0, 255);

  // Initialization of the particles collection
  std::vector<Particle> particles {};

  int particle_radius {20};
  double x {0.2 * WIDTH + particle_radius * 2}, y {0};
  while (x + particle_radius < 0.8 * WIDTH) {
    y = 0.2 * HEIGHT + particle_radius * 2;
    while (y + particle_radius < 0.8 * HEIGHT) {
      particles.push_back(Particle(x, y,
          random_speed(rng), random_speed(rng),
          particle_radius,
          1,
          sf::Color(random_color(rng), random_color(rng), random_color(rng))));

      y += 2 * particle_radius + 5;
    }

    x += 2 * particle_radius + 5;
  }

  // Initialization of the collision system
  CollisionSystem system {particles};

  // Initialization of the simulation
  system.Simulate();

  return 0;
}
