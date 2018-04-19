// Copyright 2018 <Samuel Diebolt>

#pragma once

#include "SFML/Graphics.hpp"

#define WIDTH 800
#define HEIGHT 800

class Simulation {
public:
  Simulation();
  void Run();

private:
  void ProcessEvents();
  void Update();
  void Render();

  sf::RenderWindow window_;
};
