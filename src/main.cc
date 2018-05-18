// Copyright 2018, Samuel Diebolt <samuel.diebolt@espci.fr>

#include <iostream>
#include <random>
#include <sstream>
#include <SFML/Graphics.hpp>

#include "include/main.h"
#include "include/particle.h"
#include "include/collisionSystem.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Please enter the particle radius and space between the "
    "particles.\n");
    return 1;
  }

  int particle_radius {0};
  std::istringstream ss1 {argv[1]};
  if (!(ss1 >> particle_radius)) {
    std::cerr << "Invalid number " << argv[1] << '\n';
    return 1;
  }

  int space_between_particles {0};
  std::istringstream ss2 {argv[2]};
  if (!(ss2 >> space_between_particles)) {
    std::cerr << "Invalid number " << argv[2] << '\n';
    return 1;
  }

  std::mt19937 rng {std::random_device()()};
  std::uniform_real_distribution<double> random_speed(-1, 1);

  // Initialization of the particles collection
  std::vector<Particle> particles {};

  particles.push_back(Particle(0, 600, 500, 0, 0, particle_radius, 1, sf::Color::Red));

  // Initialize particles in a simple square crystal.
  // double x {0.2 * WINDOW_SIZE + particle_radius * 2}, y {0};
  // while (x + particle_radius < 0.8 * WINDOW_SIZE) {
  //   y = 0.2 * WINDOW_SIZE + particle_radius * 2;
  //   while (y + particle_radius < 0.8 * WINDOW_SIZE) {
  //     particles.push_back(Particle(0, x, y,
  //         random_speed(rng), random_speed(rng),
  //         particle_radius,
  //         1,
  //         sf::Color::Red));
  //
  //     y += 2 * particle_radius + space_between_particles;
  //   }
  //
  //   x += 2 * particle_radius + space_between_particles;
  // }

  // Initialization of the collision system
  CollisionSystem system {particles};

  // Initialization of the simulation
  system.Simulate();

  return 0;
}
