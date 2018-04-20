// Copyright 2018 <Samuel Diebolt>

#include <queue>
#include <cstdio>

#include "SFML/Graphics.hpp"
#include "main.h"
#include "collisionSystem.h"
#include "particle.h"
#include "event.h"

// Initializes a system with the specified collection of particles.
CollisionSystem::CollisionSystem(std::vector<Particle>* particles, double limit) :
    particles_ {particles} {
  pq_ = new std::priority_queue<Event, std::vector<Event>, std::greater<Event>>;
  for (unsigned int i {0}; i < particles_->size(); i++) {
    Predict(&((*particles_)[i]), limit);
  }

  // Redraw event
  pq_->push(Event(0, nullptr, nullptr));
}

CollisionSystem::~CollisionSystem() {
  delete pq_;
}

// Updates priority queue with all new events for particle a
void CollisionSystem::Predict(Particle* a, double limit) {
  if (a != nullptr) {
    // Particle-particle collisions
    for (unsigned int i {0}; i < this->particles_->size(); i++) {
      double dt {a->TimeToHit(&((*this->particles_)[i]))};
      if (this->time_ + dt <= limit) {
        this->pq_->push(Event(this->time_ + dt, a, &((*this->particles_)[i])));
      }
    }

    // Particle-wall collisions
    double dtX {a->TimeToHitVerticalWall()};
    double dtY {a->TimeToHitHorizontalWall()};
    if (this->time_ + dtX <= limit) {
      this->pq_->push(Event(this->time_ + dtX, a, nullptr));
    }
    if (this->time_ + dtY <= limit) {
      this->pq_->push(Event(this->time_ + dtY, nullptr, a));
    }
  }
}

// Redraw all particles
void CollisionSystem::Redraw(sf::RenderWindow* window, double limit) {
  printf("Redraw\n");
  window->clear(sf::Color::White);
  for (unsigned int i {0}; i < this->particles_->size(); i++) {
    (*this->particles_)[i].Draw(window);
  }
  window->display();
  if (this->time_ < limit) {
      this->pq_->push(Event(this->time_ + 1.0 / this->Hz_, nullptr, nullptr));
  }
}

// Simulates the system of particles for the specified amount of time
void CollisionSystem::Simulate(double limit) {
  sf::RenderWindow *window = new sf::RenderWindow {sf::VideoMode(WIDTH, HEIGHT),
      "Molecular Dynamics", sf::Style::Titlebar | sf::Style::Close};

  // Initialize priority queue with collision events and redraw event
  while (window->isOpen()) {
    printf("Loop\n");

    sf::Event event;
    while (window->pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window->close();
          break;
        default:
          break;
      }
    }

    Event e = this->pq_->top();
    this->pq_->pop();

    if (e.IsValid()) {
      printf("Valid Event\n");
      Particle* a {e.GetParticleA()};
      Particle* b {e.GetParticleB()};

      // Physical collision, update positions and simulation clock
      for (unsigned int i {0}; i < this->particles_->size(); i++) {
        (*this->particles_)[i].Move(e.GetTime() - this->time_);
      }
      this->time_ = e.GetTime();

      // Process event
      if (a != nullptr && b != nullptr) {
        // Particle-particle collision
        a->BounceOff(b);
      } else if (a!= nullptr && b == nullptr) {
        // Particle-wall collision
        a->BounceOffVerticalWall();
      } else if (a == nullptr && b != nullptr) {
        // Particle-wall collision
        b->BounceOffHorizontalWall();
      } else if (a == nullptr && b == nullptr) {
        // Redraw event
        this->Redraw(window, limit);
      }

      this->Predict(a, limit);
      this->Predict(b, limit);
    }

    printf("##########\n");
  }

  if (window != nullptr) {
    window->close();
  }
  delete window;
}
