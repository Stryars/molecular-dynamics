// Copyright 2018 <Samuel Diebolt>

#include <iostream>
#include <sstream>

#include "SFML/Graphics.hpp"
#include "main.h"
#include "simulation.h"
#include "particle.h"

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

  std::vector<Particle> particles(n, Particle());

  Simulation simulation;
  simulation.Run();

  return 0;
}
