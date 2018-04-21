// Copyright 2018 <Samuel Diebolt>

#include <cstdio>
#include <cmath>

#include "SFML/Graphics.hpp"

#include "main.h"
#include "particle.h"

// Initializes a particle with specified position, velocity, radius,
// mass and color.
Particle::Particle(double rx, double ry, double vx, double vy,
    const double radius, const double mass, sf::Color color) :
    rx_ {rx}, ry_ {ry},
    vx_ {vx}, vy_ {vy},
    collisions_count_ {0},
    radius_ {radius}, mass_ {mass},
    color_ {color} {
  circle_.setRadius(radius_ * HEIGHT);
  circle_.setOrigin(circle_.getRadius(), circle_.getRadius());
  circle_.setFillColor(color_);
  circle_.setPosition(rx_ * HEIGHT, ry * HEIGHT);
}

// Initializes a copy of a particle.
Particle::Particle(const Particle& p) :
    rx_ {p.rx_}, ry_ {p.ry_},
    vx_ {p.vx_}, vy_{p.vy_},
    collisions_count_ {0},
    radius_ {p.radius_}, mass_ {p.mass_},
    color_ {p.color_} {
  circle_.setRadius(radius_ * HEIGHT);
  circle_.setOrigin(circle_.getRadius(), circle_.getRadius());
  circle_.setFillColor(color_);
  circle_.setPosition(rx_ * HEIGHT, ry_ * HEIGHT);
}

// Initializes a particle with random position and velocity.
Particle::Particle() :
    rx_ {0.5}, ry_ {0.5},
    vx_ {0.005}, vy_{0.005},
    collisions_count_ {0},
    radius_ {0.01}, mass_ {0.5},
    color_ {sf::Color::Black} {
  circle_.setRadius(radius_ * HEIGHT);
  circle_.setOrigin(circle_.getRadius(), circle_.getRadius());
  circle_.setFillColor(color_);
  circle_.setPosition(rx_ * HEIGHT, ry_ * HEIGHT);
}

// Necessary for TimeToHit()
bool Particle::operator==(const Particle& rhs) const {
  return (rx_ == rhs.rx_);
}

// Moves this particle in a straight line, based on its velocity,
// for a specified amount of time dt.
void Particle::Move(double dt) {
  rx_ += vx_ * dt;
  ry_ += vy_ * dt;
}

// Draws this particle on the SFML window.
void Particle::Draw(sf::RenderWindow& window) {
  circle_.setPosition(rx_ * HEIGHT, ry_ * HEIGHT);
  window.draw(circle_);
}

// Returns the number of collisions involving this particle with either
// walls or other particles.
int Particle::Count() {
  return collisions_count_;
}

// Returns the amount of time for this particle to collide with the specified
// particle, assuming no intervening collisions.
double Particle::TimeToHit(Particle& that) {
  if (*this == that) {
    return INFINITY;
  }

  double dx {that.rx_ - rx_};
  double dy {that.ry_ - ry_};
  double dvx {that.vx_ - vx_};
  double dvy {that.vy_ - vy_};

  // Dot product dv.dr
  double dvdr {dx * dvx + dy * dvy};
  // Dot product dv.dv
  double dvdv {dvx * dvx + dvy * dvy};
  // Dot product dr.dr
  double drdr {dx * dx + dy * dy};
  if (dvdr > 0) {
    return INFINITY;
  }

  // Distance between particles centers
  double sigma {radius_ + that.radius_};

  double d {(dvdr * dvdr) - dvdv * (drdr - sigma * sigma)};
  if (d < 0) {
    return INFINITY;
  }

  return -(dvdr + sqrt(d)) / dvdv;
}

// Returns the amount of time for this particle to collide with a vertical
// wall, assuming no intervening collisions.
double Particle::TimeToHitVerticalWall() {
  if (vx_ > 0) {
    return (1 - rx_ - radius_) / vx_;
  } else if (vx_ < 0) {
    return (radius_ - rx_) / vx_;
  } else {
    return INFINITY;
  }
}

// Returns the amount of time for this particle to collide with a horizontal
// wall, assuming no intervening collisions.
double Particle::TimeToHitHorizontalWall() {
  if (vy_ > 0) {
    return (1 - ry_ - radius_) / vy_;
  } else if (vy_ < 0) {
    return (radius_ - ry_) / vy_;
  } else {
    return INFINITY;
  }
}

// Updates the velocity of this particle and the specified particle according
// to the laws of elastic collision.
void Particle::BounceOff(Particle& that) {
  double dx {that.rx_ - rx_};
  double dy {that.ry_ - ry_};
  double dvx {that.vx_ - vx_};
  double dvy {that.vy_ - vy_};

  // Dot product dv.dr
  double dvdr {dx * dvx + dy * dvy};

  // Distance between particles centers at collision.
  double dist {radius_ + that.radius_};

  // Magnitude of normal force
  double magnitude {2 * mass_ * that.mass_ * dvdr /
    ((mass_ + that.mass_) * dist)};

  // Normal force in x and y directions
  double fx {magnitude * dx / dist};
  double fy {magnitude * dy / dist};

  // Update velocities according to normal force
  vx_ += fx / mass_;
  vy_ += fy / mass_;
  that.vx_ -= fx / that.mass_;
  that.vy_ -= fy / that.mass_;

  // Update collision counts
  collisions_count_++;
  that.collisions_count_++;
}

// Updates the velocity of this particle upon collision with a vertical wall.
void Particle::BounceOffVerticalWall() {
  vx_ = -vx_;
  collisions_count_++;
}

// Updates the velocity of this particle upon collision with a
// horizontal wall.
void Particle::BounceOffHorizontalWall() {
  vy_ = -vy_;
  collisions_count_++;
}

// Returns the kinetic energy of this particle.
double Particle::KineticEnergy() {
  return 0.5 * mass_ * (vx_ * vx_ + vy_ * vy_);
}
