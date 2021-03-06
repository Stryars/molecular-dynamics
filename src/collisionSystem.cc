// Copyright 2018, Samuel Diebolt <samuel.diebolt@espci.fr>

#include <queue>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <random>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "include/main.h"
#include "include/collisionSystem.h"
#include "include/particle.h"
#include "include/event.h"
#include "include/hsv2rgb.h"

// Initializes a system with the specified collection of particles.
CollisionSystem::CollisionSystem(std::vector<Particle> particles,
    double friction) :
    window_ {sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE),
        "Molecular Dynamics", sf::Style::Titlebar | sf::Style::Close},
    Hz_ {0.5},
    time_ {0},
    particles_ {particles},
    friction_ {friction} {
  // Initialize the window
  window_.setFramerateLimit(60);

  // Initialize priority queue with collision events and redraw event
  for (auto& particle : particles_) {
    Predict(&particle, BOX_SIZE, 0);
  }

  // First redraw event
  pq_.push(Event(Event::Type::kRedraw, 0));
}

// Empty constructor: prevents a segmentation fault.
CollisionSystem::~CollisionSystem() {}

// Updates priority queue with all new events for particle a.
void CollisionSystem::Predict(Particle* a, double wall_size,
    double wall_speed) {
  if (a != nullptr) {
    // Particle-particle collisions
    for (auto& particle : particles_) {
      double dt {a->TimeToHit(particle)};
      if (dt != INFINITY && dt >= 0.0) {
        pq_.push(Event(Event::Type::kParticleParticle, time_ + dt, a,
            &particle));
      }
    }

    // Particle-wall collisions
    double dtX {a->TimeToHitVerticalWall(wall_size, wall_speed)};
    if (dtX != INFINITY) {
      pq_.push(Event(Event::Type::kVerticalWall, time_ + dtX, a));
    }
    double dtY {a->TimeToHitHorizontalWall(wall_size, wall_speed)};
    if (dtY != INFINITY) {
      pq_.push(Event(Event::Type::kHorizontalWall, time_ + dtY, a));
    }
  }
}

// Empties the priority queue and predicts all future events.
void CollisionSystem::RegenerateEvents(double wall_size, double wall_speed) {
  while (!pq_.empty()) {
    pq_.pop();
  }
  for (auto& particle : particles_) {
    Predict(&particle, wall_size, wall_speed);
  }
  pq_.push(Event(Event::Type::kRedraw, time_));
}

// Redraws all particles.
void CollisionSystem::Redraw(bool display_isosurface) {
  if (display_isosurface == true && window_.isOpen()) {
    constexpr int kPixelSize {static_cast<int>(BOX_SIZE * BOX_SIZE * 4)};
    sf::Uint8 pixels[kPixelSize];

    sf::Texture texture;
    texture.create(BOX_SIZE, BOX_SIZE);

    sf::Sprite sprite;

    for (auto x {0}; x < BOX_SIZE; ++x) {
      for (auto y {0}; y < BOX_SIZE; ++y) {
        int index {static_cast<int>((x + y * BOX_SIZE) * 4)};
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
      particle.Draw(&window_);
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

// Displays helper text.
void CollisionSystem::DisplayHelp(const sf::Font& font) {
  window_.clear(sf::Color::Black);

  DrawText(font,
      "Press A to add a new particle at a random position.", 20,
      sf::Color::White, 0, 0);

  DrawText(font,
      "Press B to display/hide brownian motion.", 20,
      sf::Color::White, 0, 30);

  DrawText(font,
      "Press C to clear brownian path.", 20,
      sf::Color::White, 0, 60);

  DrawText(font,
      "Press H to display/hide the help.", 20,
      sf::Color::White, 0, 90);

  DrawText(font,
      "Press I to visualise isosurface (WARNING: VERY POOR PERFORMANCES).", 20,
      sf::Color::White, 0, 120);

  DrawText(font,
      "Press P to display/hide the particles.", 20,
      sf::Color::White, 0, 150);

  DrawText(font,
      "Press O to clear overlapped particles.", 20,
      sf::Color::White, 0, 180);

  DrawText(font,
      "Press S to display/hide the simulation.", 20,
      sf::Color::White, 0, 210);

  DrawText(font,
      "Press Space to pause/unpause or start the simulation.", 20,
      sf::Color::White, 0, 240);

  DrawText(font,
      "Press the Up and Down arrows to change the size of the simulation box.",
      20, sf::Color::White, 0, 270);

  DrawText(font,
      "Press the Left and Right arrows to change the scale of the histogram.",
      20, sf::Color::White, 0, 300);

  DrawText(font,
      "Press Escape to quit the simulation.", 20,
      sf::Color::White, 0, 330);

  DrawText(font,
      "The histogram displays the real velocity distribution in red\n"
      "and the Maxwell-Boltzmann probability density function in white.", 20,
      sf::Color::White, 0, 450);

  window_.display();

  Pause(sf::Keyboard::H);
}

// Displays physical quantities (temperature, pressure, etc.) and helper text.
void CollisionSystem::DisplayCharacteristics(const sf::Font& font,
    time_t elapsed_time, int collisions, double average_kinetic_energy,
    double wall_size, double wall_speed, sf::Time frameTime) {
  DrawText(font,
      "Press H to display/hide the help.", 20,
      sf::Color::White, 600, 90);

  int fps {static_cast<int>(1 / (frameTime.asMicroseconds() * pow(10, -6)))};
  if (fps < 0) {
    fps = 0;
  }
  DrawText(font,
      "FPS: " + std::to_string(fps), 20,
      sf::Color::White, WINDOW_SIZE - 100, 0);

  DrawText(font,
      "Speed scale", 20,
      sf::Color::White, WINDOW_SIZE - 200, 340);

  sf::VertexArray speed_scale(sf::Lines);
  for (auto i {0}; i < 600; i += 2) {
    for (auto j {0}; j < 2; ++j) {
      float r {0}, g{0}, b {0};
      HSVtoRGB(300 - i / 2, 1.0, 1.0, &r, &g, &b);
      speed_scale.append(sf::Vertex(sf::Vector2f(WINDOW_SIZE - 160,
          400 + i + j), sf::Color(r * 255, g * 255, b * 255)));
      speed_scale.append(sf::Vertex(sf::Vector2f(WINDOW_SIZE - 100,
          400 + i + j), sf::Color(r * 255, g * 255, b * 255)));
    }
  }

  window_.draw(speed_scale);

  const double boltzmann_constant {1.3806503e-23};

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
      / (wall_size * DISTANCE_UNIT
      * wall_size * DISTANCE_UNIT)};
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
  double packing_factor {particles_area / (wall_size * wall_size)};
  DrawText(font,
      "Packing factor: " + std::to_string(packing_factor * 100) + "%", 20,
      sf::Color::White, 0, 150);

  DrawText(font,
      "Time: " + std::to_string(time_), 20,
      sf::Color::White, 600, 0);

  DrawText(font,
      "Priority queue size: " + std::to_string(pq_.size()) , 20,
      sf::Color::White, 600, 30);

  DrawText(font,
      "Wall speed: " + std::to_string(SPEED_UNIT * wall_speed / 2) , 20,
      sf::Color::White, 600, 60);
}

// Display the velocity histogram.
void CollisionSystem::DisplayVelocityHistogram(double horizontal_scale,
    double average_kinetic_energy) {
  int max_speed {0};
  for (const auto& particle : particles_) {
    if (particle.GetSpeed() > max_speed) {
      max_speed = particle.GetSpeed();
    }
  }

  max_speed++;
  // const int max_speed {20};
  const float bucket_size {0.02};
  int number_of_buckets {static_cast<int>(ceil(max_speed / bucket_size))};

  std::vector<int> speed_histogram(number_of_buckets);

  for (auto& particle : particles_) {
    int bucket {static_cast<int>(floor(particle.GetSpeed() / bucket_size))};
    speed_histogram[bucket]++;
  }

  auto max_particles = std::max_element(speed_histogram.begin(),
      speed_histogram.end());

  for (auto i {0}; i < number_of_buckets; ++i) {
    sf::RectangleShape line(sf::Vector2f(1000 * bucket_size / 4,
        speed_histogram[i] * 270 / *max_particles));
    line.rotate(180);
    line.setFillColor(sf::Color::Red);
    line.setPosition(horizontal_scale * (i + 1) * bucket_size / 2, WINDOW_SIZE - 5);
    window_.draw(line);
  }

  sf::RectangleShape horizontal_line(sf::Vector2f(WINDOW_SIZE, 5));
  horizontal_line.setFillColor(sf::Color::White);
  horizontal_line.setPosition(0, WINDOW_SIZE - 5);
  window_.draw(horizontal_line);

  // Maxwell-Boltzmann probability density function
  sf::VertexArray maxwell_boltzmann(sf::LinesStrip);
  const double boltzmann_constant {1.3806503e-23};
  double mass {MASS_UNIT};
  double temperature {(2.0 / 3.0)
      * average_kinetic_energy / boltzmann_constant};
  for (double i {0}; i < number_of_buckets; i += 0.25) {
    double y {pow(mass / (2 * M_PI * boltzmann_constant * temperature), 3 / 2)
        * 4 * M_PI * pow(i * bucket_size * SPEED_UNIT, 2)
        * exp(-mass * pow(i * bucket_size * SPEED_UNIT, 2)
        / (2 * boltzmann_constant * temperature))};
    maxwell_boltzmann.append(sf::Vector2f(horizontal_scale * i * bucket_size / 2,
        WINDOW_SIZE - 5 - 150 * y));
  }

  window_.draw(maxwell_boltzmann);
}

// Simulates the system of particles for the specified amount of time
int CollisionSystem::Simulate() {
  // Initialize random device for random position and speed when adding new
  // particles
  std::mt19937 rng {std::random_device()()};
  std::uniform_real_distribution<double> random_speed(-1, 1);
  std::uniform_real_distribution<double> random_position(
        (WINDOW_SIZE - BOX_SIZE) / 2 + particles_[0].GetRadius(),
        (WINDOW_SIZE - BOX_SIZE) / 2 + BOX_SIZE - particles_[0].GetRadius());

  // Booleans for displaying isosurfaces, particles, brownian motion, etc.
  bool display_isosurface {false};
  bool display_particles {true};
  bool display_brownian_path {false};
  bool display_simulation {true};

  // Histogram horizontal scale
  double histogram_scale {1000};

  // Brownian motion path
  // Storing an index and not a pointer to the particle because of heap
  // reallocation when calling std::vector::push_back()
  sf::VertexArray brownian_path(sf::LinesStrip);
  int brownian_particle_index = particles_.size() / 2;

  // Initialize the font
  sf::Font source_code_pro;
  if (!source_code_pro.loadFromFile("etc/fonts/sourcecodepro.otf")) {
    printf("Couldn't load Source Code Pro font.\n");
    exit(1);
  }

  // Initialize the box
  double wall_size {BOX_SIZE}, wall_speed {0.0};
  sf::RectangleShape simulation_box(sf::Vector2f(wall_size, wall_size));
  simulation_box.setPosition((WINDOW_SIZE - wall_size) / 2,
      (WINDOW_SIZE - wall_size) / 2);
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
      0, wall_size, wall_speed, sf::Time {});
  window_.draw(simulation_box);
  Redraw(display_isosurface);

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
          // A: add a new particle
          if (event.key.code == sf::Keyboard::A) {
            particles_.push_back(Particle(time_,
                random_position(rng), random_position(rng),
                random_speed(rng), random_speed(rng),
                particles_[0].GetRadius() / 2,
                0.25,
                sf::Color::Red));

            // The event priority queue is regenerated to account for the new
            // particle
            RegenerateEvents(wall_size, wall_speed);
          // B: display brownian path
          } else if (event.key.code == sf::Keyboard::B) {
              display_brownian_path = !display_brownian_path;
          // C: clear the brownian path
          } else if (event.key.code == sf::Keyboard::C) {
            brownian_path.clear();
          // H: display helper text
          } else if (event.key.code == sf::Keyboard::H) {
            DisplayHelp(source_code_pro);
          // I: display isosurfaces
          } else if (event.key.code == sf::Keyboard::I) {
            display_isosurface = !display_isosurface;
          // P: display particles
          } else if (event.key.code == sf::Keyboard::P) {
            display_particles = !display_particles;
          // O: delete overlapped particles
          } else if (event.key.code == sf::Keyboard::O) {
            std::vector<Particle> overlapped_particles;
            for (unsigned int i {0}; i < particles_.size(); ++i) {
              for (unsigned int j {i}; j < particles_.size(); ++j) {
                Particle a = particles_[i];
                Particle b = particles_[j];
                double t {a.TimeToHit(b)};
                if (t < 0) {
                  if (a.GetBirthdate() <= b.GetBirthdate()) {
                    overlapped_particles.push_back(b);
                  } else {
                    overlapped_particles.push_back(a);
                  }
                }
              }
            }

            for (const auto& particle : overlapped_particles) {
              ptrdiff_t pos = distance(particles_.begin(),
                  find(particles_.begin(), particles_.end(), particle));
              particles_.erase(particles_.begin() + pos);
            }

            RegenerateEvents(wall_size, wall_speed);
          // S: display the simulation
          } else if (event.key.code == sf::Keyboard::S) {
            display_simulation = !display_simulation;
          // Space: pause the simulation
          } else if (event.key.code == sf::Keyboard::Space) {
            Pause(sf::Keyboard::Space);
          // Down: wall speed down
          } else if (event.key.code == sf::Keyboard::Down) {
            wall_speed -= 0.1;
            RegenerateEvents(wall_size, wall_speed);
          // Up: wall speed up
          } else if (event.key.code == sf::Keyboard::Up) {
            wall_speed += 0.1;
            RegenerateEvents(wall_size, wall_speed);
          // Right: zoom in on histogram
          } else if (event.key.code == sf::Keyboard::Right) {
            histogram_scale += 100;
          // Left : zoom out on histogram
          } else if (event.key.code == sf::Keyboard::Left) {
            histogram_scale -= 100;
          }
          break;
        default:
          break;
      }
    }

    // Get the next valid event from the priority queue
    Event e = pq_.top();
    pq_.pop();
    while (e.IsValid() == false || e.GetTime() < time_) {
      e = pq_.top();
      pq_.pop();
    }

    Particle* a {e.GetParticleA()};
    Particle* b {e.GetParticleB()};

    Event::Type event_type {e.GetType()};

    double average_kinetic_energy {0.0};

    // Physical collision, update positions, simulation clock and
    // calculate average kinetic energy
    // 2 times wall_speed! wall_speed represents the speed of the whole height
    // or width of the box. Thus, when changing e.g. the width, the speed gets
    // "divided by two": half of it accounts for the left side, the other half
    // for the right side.

    if (wall_size > WINDOW_SIZE) {
      wall_size = WINDOW_SIZE;
      wall_speed = 0;
    }
    wall_size += 2 * wall_speed * (e.GetTime() - time_);
    simulation_box.setSize(sf::Vector2f(wall_size, wall_size));
    simulation_box.setPosition((WINDOW_SIZE - wall_size) / 2,
        (WINDOW_SIZE - wall_size) / 2);

    for (auto& particle : particles_) {
      particle.Move(e.GetTime() - time_);

      // Ensures the particle stays inside the simulation box. Prevents floating
      // point errors.
      if (particle.GetRx() - particle.GetRadius()
          < (WINDOW_SIZE - wall_size) / 2 - EPSILON) {
        particle.SetRx((WINDOW_SIZE - wall_size) / 2 + particle.GetRadius());
      }
      if (particle.GetRx() + particle.GetRadius()
          > (WINDOW_SIZE - wall_size) / 2 + wall_size + EPSILON) {
        particle.SetRx((WINDOW_SIZE - wall_size) / 2 + wall_size
            - particle.GetRadius());
      }
      if (particle.GetRy() - particle.GetRadius()
          < (WINDOW_SIZE - wall_size) / 2 - EPSILON) {
        particle.SetRy((WINDOW_SIZE - wall_size) / 2 + particle.GetRadius());
      }
      if (particle.GetRy() + particle.GetRadius()
          > (WINDOW_SIZE - wall_size) / 2 + wall_size + EPSILON) {
        particle.SetRy((WINDOW_SIZE - wall_size) / 2 + wall_size
            - particle.GetRadius());
      }

      average_kinetic_energy += particle.KineticEnergy();

      // Change the particle's color based on its speed
      float hue {static_cast<float>(particle.GetSpeed() * 300.0 / 3.0)};
      float red {0}, green {0}, blue {0};
      // Using HSV color space is easier to color the particles
      HSVtoRGB(hue, 1.0, 1.0, &red, &green, &blue);
      particle.SetColor(sf::Color(red * 255, green * 255, blue * 255));
    }
    average_kinetic_energy /= particles_.size();
    time_ = e.GetTime();

    if (a == &(particles_[brownian_particle_index])) {
      brownian_path.append(sf::Vector2f(a->GetRx(), a->GetRy()));
    } else if (b == &(particles_[brownian_particle_index])) {
      brownian_path.append(sf::Vector2f(b->GetRx(), b->GetRy()));
    }

    // Process event
    switch (event_type) {
      // Particle-particle collision
      case Event::Type::kParticleParticle:
        a->BounceOff(b, friction_);
        collisions++;
        break;
      // Particle-vertical wall collision
      case Event::Type::kVerticalWall:
        a->BounceOffVerticalWall(wall_speed);
        collisions++;
        break;
      // Particle-horizontal wall collision
      case Event::Type::kHorizontalWall:
        a->BounceOffHorizontalWall(wall_speed);
        collisions++;
        break;
      // Redraw event
      case Event::Type::kRedraw:
        window_.clear(sf::Color::Black);

        elapsed_time = time(nullptr) - start_time;
        DisplayCharacteristics(source_code_pro, elapsed_time, collisions,
            average_kinetic_energy, wall_size, wall_speed, frameTime);

        DisplayVelocityHistogram(histogram_scale, average_kinetic_energy);

        if (display_simulation) {
          window_.draw(simulation_box);

          if (display_particles) {
            Redraw(display_isosurface);
          }
          if (display_brownian_path) {
            window_.draw(brownian_path);
          }
        }

        window_.display();

        // FPS counter
        frameTime = clock.restart();

        // Add a redraw event to the priority queue
        pq_.push(Event(Event::Type::kRedraw, time_ + 1.0 / Hz_));
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
    Predict(a, wall_size, wall_speed);
    Predict(b, wall_size, wall_speed);
  }

  return 0;
}
