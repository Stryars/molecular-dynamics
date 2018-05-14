// Copyright 2018, Samuel Diebolt <samuel.diebolt@espci.fr>

#include <queue>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <random>
#include <cmath>

#include "include/SFML/Graphics.hpp"
#include "include/main.h"
#include "include/collisionSystem.h"
#include "include/particle.h"
#include "include/event.h"
#include "include/hsv2rgb.h"

// Initializes a system with the specified collection of particles.
CollisionSystem::CollisionSystem(std::vector<Particle> particles) :
    window_ {sf::VideoMode(WIDTH, HEIGHT),
        "Molecular Dynamics", sf::Style::Titlebar | sf::Style::Close},
    Hz_ {0.5},
    time_ {0},
    particles_ {particles} {
  // Initialize the window
  window_.setFramerateLimit(60);

  // Initialize priority queue with collision events and redraw event
  for (auto& particle : particles_) {
    Predict(&particle);
  }

  // First redraw event
  pq_.push(Event(Event::Type::kRedraw, 0, nullptr, nullptr));
}

// Empty constructor: prevents a segmentation fault.
CollisionSystem::~CollisionSystem() {}

// Updates priority queue with all new events for particle a.
void CollisionSystem::Predict(Particle* a) {
  if (a != nullptr) {
    // Particle-particle collisions
    for (auto& particle : particles_) {
      double dt {a->TimeToHit(particle)};
      if (dt != INFINITY && dt >= 0.0) {
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
void CollisionSystem::Redraw(bool isosurface) {
  if (isosurface == true && window_.isOpen()) {
    sf::Uint8 pixels[BOX_WIDTH * BOX_HEIGHT * 4];

    sf::Texture texture;
    texture.create(BOX_WIDTH, BOX_HEIGHT);

    sf::Sprite sprite;

    for (auto x {0}; x < BOX_WIDTH; ++x) {
      for (auto y {0}; y < BOX_HEIGHT; ++y) {
        int index {(x + y * BOX_WIDTH) * 4};
        float sum {0};
        for (const auto& particle : particles_) {
          double rx {particle.GetRx()}, ry {particle.GetRy()};
          double d {sqrt((x + 280 - rx) * (x + 280 - rx)
              + (y + 280 - ry) * (y + 280 - ry))};
          sum += 300 * particle.GetRadius() / d;
        }
        sum = fmin(sum, 360);
        float r {0}, g {0}, b {0};
        HSVtoRGB(sum, 1.0, 1.0, &r, &g, &b);
        pixels[index] = r * 255;
        pixels[index + 1] = g * 255;
        pixels[index + 2] = b * 255;
        pixels[index + 3] = 255;
      }
    }

    texture.update(pixels);
    sprite.setTexture(texture);
    sprite.setPosition(280, 280);
    sprite.setColor(sf::Color::White);
    window_.draw(sprite);
  } else {
    for (const auto& particle : particles_) {
      particle.Draw(window_);
    }
  }
}

// Pauses the simulation.
void CollisionSystem::Pause(sf::Keyboard::Key pause_key) {
  bool pause {true};

  while (pause == true) {
    sf::Event event;
    if (window_.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          pause = false;
          window_.close();
          break;
        case sf::Event::KeyPressed:
          if (event.key.code == sf::Keyboard::Escape) {
            pause = false;
            window_.close();
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

void CollisionSystem::DrawText(const sf::Font& font, const std::string& str,
    int character_size, sf::Color color, int x, int y) {
  sf::Text text;
  text.setFont(font);
  text.setString(str);
  text.setCharacterSize(character_size);
  text.setFillColor(color);
  text.setPosition(x, y);
  window_.draw(text);
}

// Displays physical quantities (temperature, pressure, etc.) and helper text.
void CollisionSystem::DisplayCharacteristics(const sf::Font& font,
    time_t elapsed_time, int collisions, double average_kinetic_energy,
    sf::Time frameTime) {
  DrawText(font,
      "Press A to add a new particle.", 20,
      sf::Color::White, 0, HEIGHT - 240);

  DrawText(font,
      "Press B to display/hide brownian motion.", 20,
      sf::Color::White, 0, HEIGHT - 210);

  DrawText(font,
      "Press C to clear brownian path.", 20,
      sf::Color::White, 0, HEIGHT - 180);

  DrawText(font,
      "Press I to visualise isosurface (WARNING: VERY POOR PERFORMANCES).", 20,
      sf::Color::White, 0, HEIGHT - 150);

  DrawText(font,
      "Press P to display/hide the particles.", 20,
      sf::Color::White, 0, HEIGHT - 120);

  DrawText(font,
      "Press Space to pause/unpause or start the simulation.", 20,
      sf::Color::White, 0, HEIGHT - 90);

  DrawText(font,
      "Press Left Shift to display the velocity histogram.", 20,
      sf::Color::White, 0, HEIGHT - 60);

  DrawText(font,
      "Press Escape to quit the simulation.", 20,
      sf::Color::White, 0, HEIGHT - 30);

  int fps {static_cast<int>(1 / (frameTime.asMicroseconds() * pow(10, -6)))};
  if (fps < 0) {
    fps = 0;
  }
  DrawText(font,
      "FPS: " + std::to_string(fps), 20,
      sf::Color::White, WIDTH - 100, 0);

  DrawText(font,
      "Speed scale", 20,
      sf::Color::White, WIDTH - 200, 340);

  sf::VertexArray speed_scale(sf::Lines);
  for (auto i {0}; i < 600; i += 2) {
    for (auto j {0}; j < 2; ++j) {
      float r {0}, g{0}, b {0};
      HSVtoRGB(300 - i / 2, 1.0, 1.0, &r, &g, &b);
      speed_scale.append(sf::Vertex(sf::Vector2f(WIDTH - 160, 400 + i + j),
          sf::Color(r * 255, g * 255, b * 255)));
      speed_scale.append(sf::Vertex(sf::Vector2f(WIDTH - 100, 400 + i + j),
          sf::Color(r * 255, g * 255, b * 255)));
    }
  }

  window_.draw(speed_scale);

  const double boltzmann_constant = 1.3806503e-23;

  DrawText(font,
      "Particles count: " + std::to_string(particles_.size()), 20,
      sf::Color::White, 0, 0);

  std::string collisions_per_second {"0"};
  if (elapsed_time != 0) {
    collisions_per_second = std::to_string(collisions / elapsed_time);
  }
  DrawText(font,
      "Collisions per second: " + collisions_per_second, 20,
      sf::Color::White, 0, 30);

  std::ostringstream streamEnerg;
  streamEnerg << average_kinetic_energy;
  std::string strEnerg = streamEnerg.str();
  DrawText(font,
      "Av. kinetic energy: " + strEnerg + "J", 20,
      sf::Color::White, 0, 60);

  double temperature {(2.0 / 3.0)
      * average_kinetic_energy / boltzmann_constant};
  std::ostringstream streamTemp;
  streamTemp << temperature;
  std::string strTemp = streamTemp.str();
  DrawText(font,
      "Temperature: " + strTemp + "K", 20,
      sf::Color::White, 0, 90);

  double pressure {(2.0 / 3.0) * average_kinetic_energy * particles_.size()
      / (HEIGHT * 0.6 * DISTANCE_UNIT * WIDTH * 0.6 * DISTANCE_UNIT)};
  std::ostringstream streamPress;
  streamPress << pressure;
  std::string strPress = streamPress.str();
  DrawText(font,
      "Pressure: " + strPress + "Pa", 20,
      sf::Color::White, 0, 120);

  double particles_area {0.0};
  for (const auto& particle : particles_) {
    particles_area += M_PI * pow(particle.GetRadius(), 2);
  }
  double packing_factor {particles_area / (BOX_WIDTH * BOX_HEIGHT)};
  DrawText(font,
      "Packing factor: " + std::to_string(packing_factor * 100) + "%", 20,
      sf::Color::White, 0, 150);

  DrawText(font,
      "Time: " + std::to_string(time_), 20,
      sf::Color::White, 600, 0);

  DrawText(font,
      "Priority queue size:" + std::to_string(pq_.size()) , 20,
      sf::Color::White, 600, 30);
}

// Display the velocity histogram.
void CollisionSystem::DisplayVelocityHistogram() {
  const int max_speed {2500};
  const float bucket_size {10};
  int number_of_buckets {static_cast<int>(ceil(max_speed / bucket_size))};

  std::vector<int> speed_histogram(number_of_buckets);

  for (auto& particle : particles_) {
    int bucket {static_cast<int>(floor(particle.GetSpeed()
        * SPEED_UNIT / bucket_size))};
    speed_histogram[bucket]++;
  }

  window_.clear(sf::Color::Black);

  for (auto i {0}; i < number_of_buckets; ++i) {
    sf::RectangleShape line(sf::Vector2f(1, speed_histogram[i] * 80));
    line.rotate(180);
    line.setFillColor(sf::Color::White);
    line.setPosition(i * bucket_size / 2, HEIGHT - 100);
    window_.draw(line);
  }

  sf::RectangleShape horizontal_line(sf::Vector2f(WIDTH, 5));
  horizontal_line.setFillColor(sf::Color::White);
  horizontal_line.setPosition(0, HEIGHT - 100);
  window_.draw(horizontal_line);

  window_.display();

  Pause(sf::Keyboard::LShift);
}

// Simulates the system of particles for the specified amount of time
int CollisionSystem::Simulate() {
  // Initialize random device
  std::mt19937 rng {std::random_device()()};
  std::uniform_real_distribution<double> random_speed(-1, 1);
  std::uniform_real_distribution<double> random_position(
        (WIDTH - BOX_WIDTH) / 2 + particles_[0].GetRadius(),
        (WIDTH - BOX_WIDTH) / 2 + BOX_WIDTH - particles_[0].GetRadius());

  // Booleans for displaying isosurfaces, particles, brownian motion, etc.
  bool isosurface {false};
  bool display_particles {true};

  // Brownian motion path
  bool display_brownian_path {false};
  sf::VertexArray brownian_path(sf::LinesStrip);
  Particle* brownian_particle {&(particles_[particles_.size() / 2])};

  // Initialize the font
  sf::Font source_code_pro;
  if (!source_code_pro.loadFromFile("etc/fonts/sourcecodepro.otf")) {
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

  // Initial display before starting the simulation
  window_.clear(sf::Color::Black);

  DisplayCharacteristics(source_code_pro, elapsed_time, collisions,
      0, sf::Time {});
  window_.draw(simulation_box);
  Redraw(isosurface);

  window_.display();

  Pause(sf::Keyboard::Space);

  // Main simulation loop
  while (window_.isOpen() && !pq_.empty()) {
    // printf("Time: %lf\n", time_);
    // printf("PQ size: %lu\n", pq_.size());
    sf::Event event;
    // Process user events
    if (window_.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window_.close();
          return 0;
          break;
        case sf::Event::KeyPressed:
          if (event.key.code == sf::Keyboard::Escape) {
            window_.close();
            return 0;
            continue;
          }
          break;
        case sf::Event::KeyReleased:
          // Left Shift: display the velocity histogram
          if (event.key.code == sf::Keyboard::LShift) {
            DisplayVelocityHistogram();
          // Right Shift: display isosurfaces
          } else if (event.key.code == sf::Keyboard::I) {
            isosurface = !isosurface;
          // Space: pause the simulation
          } else if (event.key.code == sf::Keyboard::Space) {
            Pause(sf::Keyboard::Space);
          // B: display brownian path
          } else if (event.key.code == sf::Keyboard::B) {
            display_brownian_path = !display_brownian_path;
          // C: clear the brownian path
          } else if (event.key.code == sf::Keyboard::C) {
            brownian_path.clear();
          // P: display particles
          } else if (event.key.code == sf::Keyboard::P) {
            display_particles = !display_particles;
          /// A: add a new particle
          } else if (event.key.code == sf::Keyboard::A) {
            while (!pq_.empty()) {
              pq_.pop();
            }
            particles_.push_back(Particle(
                random_position(rng),
                random_position(rng),
                random_speed(rng), random_speed(rng),
                particles_[0].GetRadius() / 2,
                0.25,
                sf::Color::Red));
            for (auto& particle : particles_) {
              Predict(&particle);
            }

            pq_.push(Event(Event::Type::kRedraw, time_, nullptr, nullptr));
          }
          break;
        default:
          break;
      }
    }

    // Get the next valid event from the priority queue
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

    // Physical collision, update positions, simulation clock and
    // calculate average kinetic energy
    for (auto& particle : particles_) {
      particle.Move(e.GetTime() - time_);
      average_kinetic_energy += particle.KineticEnergy();

      float hue {static_cast<float>(particle.GetSpeed() * 300.0 / 3.0)};
      float red {0}, green {0}, blue {0};
      HSVtoRGB(hue, 1.0, 1.0, &red, &green, &blue);
      particle.SetColor(sf::Color(red * 255, green * 255, blue * 255));
    }
    average_kinetic_energy /= particles_.size();
    double event_time {e.GetTime()};
    if (event_time < time_) {
      printf("Invalid event time.\n");
      exit(1);
    }
    time_ = e.GetTime();

    if (a == brownian_particle) {
      brownian_path.append(sf::Vector2f(a->GetRx(), a->GetRy()));
    } else if (b == brownian_particle) {
      brownian_path.append(sf::Vector2f(b->GetRx(), b->GetRy()));
    }

    // Process event
    switch (event_type) {
      // Particle-particle collision
      case Event::Type::kParticleParticle:
        a->BounceOff(b);
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
        window_.clear(sf::Color::Black);

        elapsed_time = time(nullptr) - start_time;
        DisplayCharacteristics(source_code_pro, elapsed_time,
            collisions, average_kinetic_energy, frameTime);

        window_.draw(simulation_box);
        if (display_particles) {
          Redraw(isosurface);
        }

        if (display_brownian_path) {
          window_.draw(brownian_path);
        }

        window_.display();

        // FPS counter
        frameTime = clock.restart();

        // Add a redraw event to the priority queue
        pq_.push(Event(Event::Type::kRedraw, time_ + 1.0 / Hz_,
            nullptr, nullptr));
        break;
      default:
        printf("Error: event type invalid.\n");
        exit(1);
        break;
    }

    // Remove every invalid event from the priority queue
    // Source of huge performance decrease
    // But without it, the RAM gets eaten FAST
    // std::vector<Event> temp_events {};
    // while (!pq_.empty()) {
    //   Event temp_e {pq_.top()};
    //   pq_.pop();
    //   if (temp_e.IsValid()) {
    //     temp_events.push_back(temp_e);
    //   }
    // }
    //
    // for (const auto& ev : temp_events) {
    //   pq_.push(ev);
    // }

    // Predict the next events for particles a and b
    Predict(a);
    Predict(b);
  }

  return 0;
}
