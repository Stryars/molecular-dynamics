// Copyright 2018 <Samuel Diebolt>

#include <queue>
#include <cstdio>

#include "SFML/Graphics.hpp"
#include "main.h"
#include "collisionSystem.h"
#include "particle.h"
#include "event.h"

// Initializes a system with the specified collection of particles.
CollisionSystem::CollisionSystem(const std::vector<Particle>& particles, double limit) :
    Hz_ {2},
    time_ {0},
    particles_ {particles} {
  pq_ = new std::priority_queue<Event, std::vector<Event>, std::greater<Event>>;
  for (unsigned int i {0}; i < particles_.size(); i++) {
    Predict(&(particles_[i]), limit);
  }

  // Redraw event
  pq_->push(Event(Event::Type::kRedraw, 0, nullptr, nullptr));
}

// Deletes the priority queue.
CollisionSystem::~CollisionSystem() {
  delete pq_;
}

// Updates priority queue with all new events for particle a
void CollisionSystem::Predict(Particle* a, double limit) {
  if (a != nullptr) {
    // Particle-particle collisions
    for (unsigned int i {0}; i < this->particles_.size(); i++) {
      double dt {a->TimeToHit(this->particles_[i])};
      if (this->time_ + dt <= limit) {
        this->pq_->push(Event(Event::Type::kParticleParticle, this->time_ + dt, a, &(this->particles_[i])));
      }
    }

    // Particle-wall collisions
    double dtX {a->TimeToHitVerticalWall()};
    double dtY {a->TimeToHitHorizontalWall()};
    if (this->time_ + dtX <= limit) {
      this->pq_->push(Event(Event::Type::kVerticalWall, this->time_ + dtX, a, nullptr));
    }
    if (this->time_ + dtY <= limit) {
      this->pq_->push(Event(Event::Type::kHorizontalWall, this->time_ + dtY, nullptr, a));
    }
  }
}

// Redraw all particles
void CollisionSystem::Redraw(sf::RenderWindow* window, double limit) {
  window->clear(sf::Color::White);
  for (unsigned int i {0}; i < this->particles_.size(); i++) {
    this->particles_[i].Draw(window);
  }

  window->display();

  // bool nextDisplay {false};
  // while (window->isOpen() && nextDisplay == false) {
  //   sf::Event event;
  //   while (window->pollEvent(event)) {
  //     switch (event.type) {
  //       case sf::Event::Closed:
  //         window->close();
  //         break;
  //       case sf::Event::KeyReleased:
  //         nextDisplay = true;
  //       default:
  //         break;
  //     }
  //   }
  // }

  if (this->time_ < limit) {
      this->pq_->push(Event(Event::Type::kRedraw, this->time_ + 1.0 / this->Hz_, nullptr, nullptr));
  }
}

// Simulates the system of particles for the specified amount of time
void CollisionSystem::Simulate(double limit) {
  sf::RenderWindow *window = new sf::RenderWindow {sf::VideoMode(WIDTH, HEIGHT),
      "Molecular Dynamics", sf::Style::Titlebar | sf::Style::Close};
  window->setFramerateLimit(60);

  // Initialize priority queue with collision events and redraw event
  while (window->isOpen() && !this->pq_->empty()) {
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
      Particle* a {e.GetParticleA()};
      Particle* b {e.GetParticleB()};
      Event::Type event_type {e.GetType()};

      // Physical collision, update positions and simulation clock
      for (unsigned int i {0}; i < this->particles_.size(); i++) {
        this->particles_[i].Move(e.GetTime() - this->time_);
      }
      this->time_ = e.GetTime();

      // Process event
      switch (event_type) {
        // Particle-particle collision
        case Event::Type::kParticleParticle:
          a->BounceOff(*b);
          break;
        // Particle-wall collision
        case Event::Type::kVerticalWall:
          a->BounceOffVerticalWall();
          break;
        // Particle-wall collision
        case Event::Type::kHorizontalWall:
          b->BounceOffHorizontalWall();
          break;
        // Redraw event
        case Event::Type::kRedraw:
          this->Redraw(window, limit);
          break;
        default:
          printf("Error: event type invalid.\n");
          exit(1);
          break;
      }

      this->Predict(a, limit);
      this->Predict(b, limit);
    }
  }

  delete window;
}
