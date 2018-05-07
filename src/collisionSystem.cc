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
CollisionSystem::CollisionSystem(std::vector<Particle> particles) :
    Hz_ {0.5},
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
      if (dt != INFINITY) {
        pq_.push(Event(Event::Type::kParticleParticle, time_ + dt, a,
            &(particle)));
      }
    }

    // Particle-wall collisions
    double dtX {a->TimeToHitVerticalWall()};
    if (dtX != INFINITY) {
      pq_.push(Event(Event::Type::kVerticalWall, time_ + dtX, a, nullptr));
    }
    double dtY {a->TimeToHitHorizontalWall()};
    if (dtY != INFINITY) {
      pq_.push(Event(Event::Type::kHorizontalWall, time_ + dtY, nullptr, a));
    }
  }
}

// Redraws all particles.
void CollisionSystem::Redraw(sf::RenderWindow& window,
    sf::RectangleShape& box) {
  window.draw(box);

  for (const auto& particle : particles_) {
    particle.Draw(window);
  }
}

// Pauses the simulation.
void CollisionSystem::Pause(sf::RenderWindow& window,
    sf::Keyboard::Key pause_key) {
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

void CollisionSystem::DrawText(sf::RenderWindow& window, sf::Font& font,
    std::string str, int character_size, sf::Color color, int x, int y) {
  sf::Text text;
  text.setFont(font);
  text.setString(str);
  text.setCharacterSize(character_size);
  text.setFillColor(color);
  text.setPosition(x, y);
  window.draw(text);
}

// Displays physical quantities (temperature, pressure, etc.) and helper text.
void CollisionSystem::DisplayCharacteristics(sf::RenderWindow& window,
    sf::Font& font, time_t elapsed_time, int collisions,
    double average_kinetic_energy, sf::Time frameTime) {
  DrawText(window, font,
      "Press Space to pause/unpause or start the simulation.", 30,
      sf::Color::White, 0, HEIGHT - 120);

  DrawText(window, font,
      "Press Left Shift to display the velocity histogram.", 30,
      sf::Color::White, 0, HEIGHT - 80);

  DrawText(window, font,
      "Press Escape to quit the simulation.", 30,
      sf::Color::White, 0, HEIGHT - 40);

  int fps {static_cast<int>(1 / (frameTime.asMicroseconds() * pow(10, -6)))};
  if (fps < 0) {
    fps = 0;
  }
  DrawText(window, font,
      "FPS: " + std::to_string(fps), 30,
      sf::Color::White, WIDTH - 160, 0);

  const double boltzmann_constant = 1.3806503e-23;

  DrawText(window, font,
      "Particles count: " + std::to_string(particles_.size()), 30,
      sf::Color::White, 0, 0);

  std::string collisions_per_second {"0"};
  if (elapsed_time != 0) {
    collisions_per_second = std::to_string(collisions / elapsed_time);
  }
  DrawText(window, font,
      "Collisions per second: " + collisions_per_second, 30,
      sf::Color::White, 0, 40);

  std::ostringstream streamEnerg;
  streamEnerg << average_kinetic_energy;
  std::string strEnerg = streamEnerg.str();
  DrawText(window, font,
      "Av. kinetic energy: " + strEnerg + "J", 30,
      sf::Color::White, 0, 80);

  double temperature {(2.0 / 3.0)
      * average_kinetic_energy / boltzmann_constant};
  std::ostringstream streamTemp;
  streamTemp << temperature;
  std::string strTemp = streamTemp.str();
  DrawText(window, font,
      "Temperature: " + strTemp + "K", 30,
      sf::Color::White, 0, 120);

  double pressure {(2.0 / 3.0) * average_kinetic_energy * particles_.size()
      / (HEIGHT * 0.6 * DISTANCE_UNIT * WIDTH * 0.6 * DISTANCE_UNIT)};
  std::ostringstream streamPress;
  streamPress << pressure;
  std::string strPress = streamPress.str();
  DrawText(window, font,
      "Pressure: " + strPress + "Pa", 30,
      sf::Color::White, 0, 160);
}

// Display the velocity histogram.
void CollisionSystem::DisplayVelocityHistogram(sf::RenderWindow& window) {
  const int max_speed {2500};
  const float bucket_size {10};
  int number_of_buckets {static_cast<int>(ceil(max_speed / bucket_size))};

  std::vector<int> speed_histogram(number_of_buckets);

  for (auto& particle : particles_) {
    int bucket {static_cast<int>(floor(particle.GetSpeed()
        * SPEED_UNIT / bucket_size))};
    speed_histogram[bucket]++;
  }

  window.clear(sf::Color::Black);

  for (auto i {0}; i < number_of_buckets; ++i) {
    sf::RectangleShape line(sf::Vector2f(1, speed_histogram[i] * 80));
    line.rotate(180);
    line.setFillColor(sf::Color::White);
    line.setPosition(i * bucket_size / 2, HEIGHT - 100);
    window.draw(line);
  }

  sf::RectangleShape horizontal_line(sf::Vector2f(WIDTH, 5));
  horizontal_line.setFillColor(sf::Color::White);
  horizontal_line.setPosition(0, HEIGHT - 100);
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
  time_t start_time {time(nullptr)};
  time_t elapsed_time {0};
  int collisions {0};

  // SFML Clock for the FPS counter
  sf::Clock clock;
  sf::Time frameTime {};

  window.clear(sf::Color::Black);

  DisplayCharacteristics(window, source_code_pro, elapsed_time, collisions, 0, sf::Time {});
  Redraw(window, simulation_box);

  window.display();

  Pause(window, sf::Keyboard::Space);

  while (window.isOpen() && !pq_.empty()) {
    // printf("Collisions: %d\n", collisions);
    // printf("Time: %lf\n", time_);
    // printf("PQ size: %lu\n", pq_.size());
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

    // Get the next valid event
    Event e = pq_.top();
    pq_.pop();
    while (e.IsValid() == false) {
      e = pq_.top();
      pq_.pop();
    }

    Particle* a {e.GetParticleA()};
    Particle* b {e.GetParticleB()};

    Event::Type event_type {e.GetType()};

    double average_kinetic_energy {0.0};

    // Physical collision, update positions and simulation clock and
    // calculate average kinetic energy
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

        elapsed_time = time(nullptr) - start_time;
        DisplayCharacteristics(window, source_code_pro, elapsed_time,
            collisions, average_kinetic_energy, frameTime);

        Redraw(window, simulation_box);

        window.display();

        // FPS counter
        frameTime = clock.restart();

        pq_.push(Event(Event::Type::kRedraw, time_ + 1.0 / Hz_,
            nullptr, nullptr));
        break;
      default:
        printf("Error: event type invalid.\n");
        exit(1);
        break;
    }

    std::vector<Event> temp_events {};
    while (!pq_.empty()) {
      Event temp_e {pq_.top()};
      pq_.pop();
      if (temp_e.IsValid()) {
        temp_events.push_back(temp_e);
      }
    }

    for (const auto& ev : temp_events) {
      pq_.push(ev);
    }

    Predict(a);
    Predict(b);
  }
}
