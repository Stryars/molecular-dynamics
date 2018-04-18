// Copyright 2018 <Samuel Diebolt>

#include "game.h"

Game::Game() : window(sf::VideoMode(WIDTH, HEIGHT), "Molecular Dynamics") {
  window.setFramerateLimit(60);
}

void Game::Run() {
  while (window.isOpen()) {
    ProcessEvents();
    Update();
    Render();
  }
}

void Game::ProcessEvents() {
  sf::Event event;
  while (window.pollEvent(event)) {
    switch (event.type) {
      case sf::Event::Closed:
        window.close();
        break;
      default:
        break;
    }
  }
}

void Game::Update() {}

void Game::Render() {
  window.clear();

  // draw()

  window.display();
}
