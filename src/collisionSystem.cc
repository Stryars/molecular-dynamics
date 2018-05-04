// Copyright 2018 <Samuel Diebolt>

#include <queue>
#include <cstdio>
#include <ctime>
#include <sstream>

#include "SFML/Graphics.hpp"
#include "main.h"
#include "collisionSystem.h"
#include "particle.h"
#include "event.h"

// Initializes a system with the specified collection of particles.
CollisionSystem::CollisionSystem(const std::vector<Particle>& particles, double limit) :
    Hz_ {50000},
    time_ {0},
    particles_ {particles} {
  for (unsigned int i {0}; i < particles_.size(); ++i) {
    Predict(&(particles_[i]), limit);
  }

  // Redraw event
  pq_.push(Event(Event::Type::kRedraw, 0, nullptr, nullptr));
}

// Updates priority queue with all new events for particle a.
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

// Redraws all particles.
void CollisionSystem::Redraw(sf::RenderWindow& window, sf::RectangleShape& box) {
  window.draw(box);

  for (unsigned int i {0}; i < particles_.size(); ++i) {
    particles_[i].Draw(window);
  }

  window.display();
}

// Pauses the simulation.
void CollisionSystem::Pause(sf::RenderWindow& window) {
  bool pause {true};

  while (pause == true) {
    sf::Event event;
    while (window.pollEvent(event)) {
      switch(event.type) {
        case sf::Event::Closed:
          pause = false;
          window.close();
          break;
        case sf::Event::KeyPressed:
          if (event.key.code == sf::Keyboard::Escape) {
            pause = false;
            window.close();
          }
          break;
        case sf::Event::KeyReleased:
          if (event.type == sf::Event::KeyReleased
              && event.key.code == sf::Keyboard::Space) {
            pause = false;
          }
          break;
        default:
          break;
      }
    }
  }
}

// Displays physical quantities (temperature, pressure, etc.) and helper text.
void CollisionSystem::DisplayCharacteristics(sf::RenderWindow& window, sf::Font font, time_t elapsed_time, int collisions, double average_kinetic_energy) {
  sf::Text space_help;
  space_help.setFont(font);
  space_help.setString("Press Space to pause the simulation");
  space_help.setCharacterSize(20);
  space_help.setFillColor(sf::Color::White);
  space_help.setPosition(0, HEIGHT - 60);
  window.draw(space_help);

  const double boltzmann_constant = 1.3806503e-23;

  sf::Text particles_text;
  particles_text.setFont(font);
  particles_text.setString("Particles count: " + std::to_string(particles_.size()));
  particles_text.setCharacterSize(20);
  particles_text.setFillColor(sf::Color::White);
  window.draw(particles_text);

  sf::Text collisions_text;
  collisions_text.setFont(font);
  collisions_text.setString("Collisions per second: " + std::to_string(collisions / elapsed_time));
  collisions_text.setCharacterSize(20);
  collisions_text.setFillColor(sf::Color::White);
  collisions_text.setPosition(0, 30);
  window.draw(collisions_text);

  double temperature {(2.0 / 3.0) * average_kinetic_energy / boltzmann_constant};

  std::ostringstream streamTemp;
  streamTemp << temperature;
  std::string strTemp = streamTemp.str();

  sf::Text temperature_text;
  temperature_text.setFont(font);
  temperature_text.setString("Temperature: " + strTemp + " K");
  temperature_text.setCharacterSize(20);
  temperature_text.setFillColor(sf::Color::White);
  temperature_text.setPosition(0, 60);
  window.draw(temperature_text);

  double pressure {(2.0 / 3.0) * average_kinetic_energy * particles_.size() / (HEIGHT * 0.6 * DISTANCE_UNIT * WIDTH * 0.6 * DISTANCE_UNIT)};
  std::ostringstream streamPress;
  streamPress << pressure;
  std::string strPress = streamPress.str();

  sf::Text pressure_text;
  pressure_text.setFont(font);
  pressure_text.setString("Pressure: " + strPress + " Pa");
  pressure_text.setCharacterSize(20);
  pressure_text.setFillColor(sf::Color::White);
  pressure_text.setPosition(0, 90);
  window.draw(pressure_text);
}

// Simulates the system of particles for the specified amount of time
void CollisionSystem::Simulate(double limit) {
  // Initialize the window
  sf::RenderWindow window {sf::VideoMode(WIDTH, HEIGHT),
      "Molecular Dynamics", sf::Style::Titlebar | sf::Style::Close};
  window.setFramerateLimit(50);

  // Initialize the font
  sf::Font source_code_pro;
  if (!source_code_pro.loadFromFile("../etc/fonts/sourcecodepro.otf")) {
    printf("Couldn't load Source Code Pro font.\n");
    exit(1);
  }

  // Initialize the box
  sf::RectangleShape simulation_box(sf::Vector2f(WIDTH * 0.6, HEIGHT * 0.6));
  simulation_box.setPosition(WIDTH * 0.2, HEIGHT * 0.2);
  simulation_box.setFillColor(sf::Color::Black);
  simulation_box.setOutlineThickness(10);
  simulation_box.setOutlineColor(sf::Color::White);


  // Initialize the timer and collisions counter
  time_t start_time {time(NULL)};
  int collisions {0};

  // Initialize priority queue with collision events and redraw event
  bool display_simulation {true};

  while (window.isOpen() && !pq_.empty()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        case sf::Event::KeyPressed:
          if (event.key.code == sf::Keyboard::Escape) {
            window.close();
          }
          break;
        case sf::Event::KeyReleased:
          if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift) {
            window.clear(sf::Color::Black);
            display_simulation = !display_simulation;

            //std::map<int, int> speeds;
            for (unsigned int i {0}; i < particles_.size(); ++i) {
              //speeds[particles_[i].GetSpeed()]++;
              sf::RectangleShape line(sf::Vector2f(1, 100));
              line.setFillColor(sf::Color::White);
              line.setPosition(particles_[i].GetSpeed() / 1000, 900);
              window.draw(line);
            }

            sf::RectangleShape horizontal_line(sf::Vector2f(1900, 5));
            horizontal_line.setFillColor(sf::Color::White);
            horizontal_line.setPosition(10, 1000);
            window.draw(horizontal_line);

            window.display();

          } else if (event.key.code == sf::Keyboard::Space) {
            Pause(window);
          }
          break;
        default:
          break;
      }
    }

    Event e = pq_.top();
    pq_.pop();

    if (e.IsValid()) {
      Particle* a {e.GetParticleA()};
      Particle* b {e.GetParticleB()};
      Event::Type event_type {e.GetType()};

      double average_kinetic_energy {0.0};

      // Physical collision, update positions and simulation clock and calculate average kinetic energy
      for (unsigned int i {0}; i < particles_.size(); ++i) {
        particles_[i].Move(e.GetTime() - time_);
        average_kinetic_energy += particles_[i].KineticEnergy();
      }
      average_kinetic_energy /= particles_.size();
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
          if (display_simulation == true) {
            window.clear(sf::Color::Black);

            time_t elapsed_time = time(NULL) - start_time;
            if (elapsed_time > 0) {
              DisplayCharacteristics(window, source_code_pro, elapsed_time, collisions, average_kinetic_energy);
            }

            Redraw(window, simulation_box);
          }

          if (time_ < limit) {
              pq_.push(Event(Event::Type::kRedraw, time_ + 1.0 / Hz_, nullptr, nullptr));
          }

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
