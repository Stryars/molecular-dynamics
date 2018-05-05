// Copyright 2018 <Samuel Diebolt>

#include <queue>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <cmath>

#include "SFML/Graphics.hpp"
#include "main.h"
#include "collisionSystem.h"
#include "particle.h"
#include "event.h"

// Initializes a system with the specified collection of particles.
CollisionSystem::CollisionSystem(std::vector<Particle>& particles) :
    Hz_ {10},
    time_ {0},
    particles_ {particles} {
  // Initialize priority queue with collision events and redraw event
  for (auto& particle : particles_) {
    Predict(&particle);
  }

  // Redraw event
  pq_.push(Event(Event::Type::kRedraw, 0, nullptr, nullptr));
}

// Updates priority queue with all new events for particle a.
void CollisionSystem::Predict(Particle* a) {
  if (a != nullptr) {
    // Particle-particle collisions
    for (auto& particle : particles_) {
      double dt {a->TimeToHit(particle)};
      pq_.push(Event(Event::Type::kParticleParticle, time_ + dt, a, &(particle)));
    }

    // Particle-wall collisions
    double dtX {a->TimeToHitVerticalWall()};
    double dtY {a->TimeToHitHorizontalWall()};
    pq_.push(Event(Event::Type::kVerticalWall, time_ + dtX, a, nullptr));
    pq_.push(Event(Event::Type::kHorizontalWall, time_ + dtY, nullptr, a));
  }
}

// Redraws all particles.
void CollisionSystem::Redraw(sf::RenderWindow& window, sf::RectangleShape& box) {
  window.draw(box);

  for (const auto& particle : particles_) {
    particle.Draw(window);
  }
}

// Pauses the simulation.
void CollisionSystem::Pause(sf::RenderWindow& window, sf::Keyboard::Key pause_key) {
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
              && event.key.code == pause_key) {
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
void CollisionSystem::DisplayCharacteristics(sf::RenderWindow& window, sf::Font& font, time_t elapsed_time, int collisions, double average_kinetic_energy) {
  sf::Text space_help;
  space_help.setFont(font);
  space_help.setString("Press Space to pause/unpause or start the simulation.");
  space_help.setCharacterSize(20);
  space_help.setFillColor(sf::Color::White);
  space_help.setPosition(0, HEIGHT - 90);
  window.draw(space_help);

  sf::Text shift_help;
  shift_help.setFont(font);
  shift_help.setString("Press Left Shift to display the velocity histogram.");
  shift_help.setCharacterSize(20);
  shift_help.setFillColor(sf::Color::White);
  shift_help.setPosition(0, HEIGHT - 60);
  window.draw(shift_help);

  sf::Text escape_help;
  escape_help.setFont(font);
  escape_help.setString("Press Escape to quit the simulation.");
  escape_help.setCharacterSize(20);
  escape_help.setFillColor(sf::Color::White);
  escape_help.setPosition(0, HEIGHT - 30);
  window.draw(escape_help);

  const double boltzmann_constant = 1.3806503e-23;

  sf::Text particles_text;
  particles_text.setFont(font);
  particles_text.setString("Particles count: " + std::to_string(particles_.size()));
  particles_text.setCharacterSize(20);
  particles_text.setFillColor(sf::Color::White);
  window.draw(particles_text);

  sf::Text collisions_text;
  collisions_text.setFont(font);
  std::string collisions_per_second {"0"};
  if (elapsed_time != 0) {
    collisions_per_second = std::to_string(collisions / elapsed_time);
  }
  collisions_text.setString("Collisions per second: " + collisions_per_second);
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

  double pressure {(2.0 / 3.0) * average_kinetic_energy * particles_.size()
      / (HEIGHT * 0.6 * DISTANCE_UNIT * WIDTH * 0.6 * DISTANCE_UNIT)};
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

// Display the velocity histogram.
void CollisionSystem::DisplayVelocityHistogram(sf::RenderWindow& window) {
  const int max_speed {1500};
  const float bucket_size {20};
  int number_of_buckets {static_cast<int>(ceil(max_speed / bucket_size))};

  std::vector<int> speed_histogram(number_of_buckets);

  for (auto& particle : particles_) {
    int bucket {static_cast<int>(floor(particle.GetSpeed() * DISTANCE_UNIT / bucket_size))};
    speed_histogram[bucket]++;
  }

  window.clear(sf::Color::Black);

  for (auto i {0}; i < number_of_buckets; ++i) {
    sf::RectangleShape line(sf::Vector2f(1, speed_histogram[i] * 20));
    line.rotate(180);
    line.setFillColor(sf::Color::White);
    line.setPosition(i * bucket_size, 900);
    window.draw(line);
  }

  sf::RectangleShape horizontal_line(sf::Vector2f(WIDTH, 5));
  horizontal_line.setFillColor(sf::Color::White);
  horizontal_line.setPosition(0, 900);
  window.draw(horizontal_line);

  window.display();

  Pause(window, sf::Keyboard::LShift);
}

// Simulates the system of particles for the specified amount of time
void CollisionSystem::Simulate() {
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
  simulation_box.setOutlineThickness(5);
  simulation_box.setOutlineColor(sf::Color::White);


  // Initialize the timer and collisions counter
  time_t start_time {time(NULL)};
  time_t elapsed_time {0};
  int collisions {0};

  window.clear(sf::Color::Black);

  DisplayCharacteristics(window, source_code_pro, elapsed_time, collisions, 0);
  Redraw(window, simulation_box);

  window.display();

  Pause(window, sf::Keyboard::Space);

  while (window.isOpen() && !pq_.empty()) {
    printf("Collisions: %d\n", collisions);
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
          if (event.key.code == sf::Keyboard::LShift) {
            DisplayVelocityHistogram(window);
          } else if (event.key.code == sf::Keyboard::Space) {
            Pause(window, sf::Keyboard::Space);
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
      for (auto& particle : particles_) {
        particle.Move(e.GetTime() - time_);
        average_kinetic_energy += particle.KineticEnergy();
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
          window.clear(sf::Color::Black);

          elapsed_time = time(NULL) - start_time;
          DisplayCharacteristics(window, source_code_pro, elapsed_time,
              collisions, average_kinetic_energy);

          Redraw(window, simulation_box);

          window.display();

          pq_.push(Event(Event::Type::kRedraw, time_ + 1.0 / Hz_, nullptr, nullptr));
          break;
        default:
          printf("Error: event type invalid.\n");
          exit(1);
          break;
      }

      Predict(a);
      Predict(b);
    }
  }
}
