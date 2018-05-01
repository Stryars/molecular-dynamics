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

  std::mt19937 rng {std::random_device()()};
  std::uniform_real_distribution<double> random_x(40, WIDTH-40);
  std::uniform_real_distribution<double> random_y(40, HEIGHT-40);
  std::uniform_real_distribution<double> random_speed(-40, 40);
  std::uniform_real_distribution<double> random_radius(20, 30);
  std::uniform_int_distribution<int> random_color(0, 255);

  // Initialization of the particles collection
  std::vector<Particle> particles {};
  particles.push_back(Particle(WIDTH / 2, HEIGHT / 2, 30, 0, 20, 10, sf::Color(255, 0, 0)));
  for (auto i {0}; i < n; ++i) {
    particles.push_back(Particle(random_x(rng), random_y(rng),
        random_speed(rng), random_speed(rng),
        20,
        10,
        sf::Color(255, 255, 255)));
  }
  // for (auto i {0}; i < n; ++i) {
  //   particles.push_back(Particle(WIDTH / 2, HEIGHT / 2,
  //       10 * cos((i * 2 * M_PI) / n), 10 * sin((i * 2 * M_PI) / n),
  //       20, 10 * i,
  //       sf::Color(random_color(rng), random_color(rng), random_color(rng))));
  // }

  // Initialization of the collision system
  const int limit {100000};
  CollisionSystem system {particles, limit};

  // Initialization of the simulation
  system.Simulate(limit);

  return 0;
}
