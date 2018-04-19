// Copyright 2018 <Samuel Diebolt>

#include "simulation.h"

Simulation::Simulation() : window_ {sf::VideoMode(WIDTH, HEIGHT), "Molecular Dynamics"} {
  window_.setFramerateLimit(60);
}

void Simulation::Run() {
  while (window_.isOpen()) {
    ProcessEvents();
    Update();
    Render();
  }
}

void Simulation::ProcessEvents() {
  sf::Event event;
  while (window_.pollEvent(event)) {
    switch (event.type) {
      case sf::Event::Closed:
        window_.close();
        break;
      default:
        break;
    }
  }
}

void Simulation::Update() {}

void Simulation::Render() {
  window_.clear();

  // draw()

  window_.display();
}
