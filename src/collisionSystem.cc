// Copyright 2018 <Samuel Diebolt>

#include <queue>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <iomanip>

#include "SFML/Graphics.hpp"
#include "main.h"
#include "collisionSystem.h"
#include "particle.h"
#include "event.h"

// Initializes a system with the specified collection of particles.
CollisionSystem::CollisionSystem(const std::vector<Particle>& particles, double limit) :
    Hz_ {3},
    time_ {0},
    particles_ {particles} {
  for (unsigned int i {0}; i < particles_.size(); ++i) {
    Predict(&(particles_[i]), limit);
  }

  // Redraw event
  pq_.push(Event(Event::Type::kRedraw, 0, nullptr, nullptr));
}

// Updates priority queue with all new events for particle a
void CollisionSystem::Predict(Particle* a, double limit) {
  if (a != nullptr) {
    // Particle-particle collisions
    for (unsigned int i {0}; i < particles_.size(); ++i) {
      double dt {a->TimeToHit(particles_[i])};
      if (time_ + dt <= limit) {
        pq_.push(Event(Event::Type::kParticleParticle, time_ + dt, a, &(particles_[i])));
      }
    }

    // Particle-wall collisions
    double dtX {a->TimeToHitVerticalWall()};
    double dtY {a->TimeToHitHorizontalWall()};
    if (time_ + dtX <= limit) {
      pq_.push(Event(Event::Type::kVerticalWall, time_ + dtX, a, nullptr));
    }
    if (time_ + dtY <= limit) {
      pq_.push(Event(Event::Type::kHorizontalWall, time_ + dtY, nullptr, a));
    }
  }
}

// Redraw all particles
void CollisionSystem::Redraw(sf::RenderWindow& window, double limit) {
  window.clear(sf::Color::Black);
  for (unsigned int i {0}; i < particles_.size(); ++i) {
    particles_[i].Draw(window);
  }

  window.display();

  if (time_ < limit) {
      pq_.push(Event(Event::Type::kRedraw, time_ + 1.0 / Hz_, nullptr, nullptr));
  }
}

// Simulates the system of particles for the specified amount of time
void CollisionSystem::Simulate(double limit) {
  sf::RenderWindow window {sf::VideoMode(WIDTH, HEIGHT),
      "Molecular Dynamics", sf::Style::Titlebar | sf::Style::Close};
  window.setFramerateLimit(50);

  time_t start_time {time(NULL)};
  long collisions {0};

  // Initialize priority queue with collision events and redraw event
  while (window.isOpen() && !pq_.empty()) {
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

    time_t elapsed_time = time(NULL) - start_time;
    if (elapsed_time > 0) {
      std::cout << "\rCollisions per second: " << std::setw(3) << std::setfill('0') << collisions / elapsed_time << '.' << std::flush;
      fflush(stdout);
    }

    Event e = pq_.top();
    pq_.pop();

    if (e.IsValid()) {
      Particle* a {e.GetParticleA()};
      Particle* b {e.GetParticleB()};
      Event::Type event_type {e.GetType()};

      // Physical collision, update positions and simulation clock
      for (unsigned int i {0}; i < particles_.size(); ++i) {
        // Update positions
        particles_[i].Move(e.GetTime() - time_);
        // if (particles_[i].GetRadius() < 30) {
        //     particles_[i].SetRadius(particles_[i].GetRadius() + 0.01);
        // }
        // Get collisions count
      }
      time_ = e.GetTime();

      // Process event
      switch (event_type) {
        // Particle-particle collision
        case Event::Type::kParticleParticle:
          a->BounceOff(*b);
          collisions++;
          break;
        // Particle-vertical wall collision
        case Event::Type::kVerticalWall:
          a->BounceOffVerticalWall();
          collisions++;
          break;
        // Particle-horizontal wall collision
        case Event::Type::kHorizontalWall:
          b->BounceOffHorizontalWall();
          collisions++;
          break;
        // Redraw event
        case Event::Type::kRedraw:
          Redraw(window, limit);
          break;
        default:
          printf("Error: event type invalid.\n");
          exit(1);
          break;
      }

      Predict(a, limit);
      Predict(b, limit);
    }
  }
}
