// Copyright 2018, Samuel Diebolt <samuel.diebolt@espci.fr>

#include <cstdio>
#include <cmath>
#include <ctime>
#include <SFML/Graphics.hpp>

#include "include/main.h"
#include "include/particle.h"

// Initializes a particle with specified position, velocity, radius,
// mass and color.
Particle::Particle(double birthdate, double rx, double ry, double vx,
    double vy, double radius, double mass, sf::Color color) :
    birthdate_ {birthdate},
    rx_ {rx}, ry_ {ry},
    vx_ {vx}, vy_ {vy},
    collisions_count_ {0},
    radius_ {radius}, mass_ {mass},
    color_ {color} {
  circle_.setRadius(radius_);
  circle_.setOrigin(circle_.getRadius(), circle_.getRadius());
  circle_.setFillColor(color_);
  circle_.setPosition(rx_, ry);
}

// Necessary for TimeToHit().
bool Particle::operator==(const Particle& rhs) const {
  return (rx_ == rhs.rx_);
}

// Moves this particle in a straight line, based on its velocity,
// for a specified amount of time dt.
void Particle::Move(double dt) {
  rx_ += vx_ * dt;
  ry_ += vy_ * dt;
  circle_.setPosition(rx_, ry_);
}

// Draws this particle on the SFML window.
void Particle::Draw(sf::RenderWindow* window) const {
  window->draw(circle_);
}

// Returns the number of collisions involving this particle with either
// walls or other particles.
int Particle::Count() const {
  return collisions_count_;
}

// Returns the amount of time for this particle to collide with the specified
// particle, assuming no intervening collisions.
double Particle::TimeToHit(const Particle& that) const {
  if (this == &that) {
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
  if (dvdr >= 0) {
    return INFINITY;
  }

  // Distance between particles centers
  double sigma {radius_ + that.radius_};
  if (drdr - sigma * sigma < 0) {
    printf("Overlapping particles: %ld.\n", time(NULL));
    return INFINITY;
  }

  double d {(dvdr * dvdr) - dvdv * (drdr - sigma * sigma)};
  if (d < 0) {
    return INFINITY;
  }

  return -(dvdr + sqrt(d)) / dvdv;
}

// Returns the amount of time for this particle to collide with a vertical
// wall, assuming no intervening collisions.
double Particle::TimeToHitVerticalWall() const {
  if (vx_ > 0) {
    return (0.8 * WIDTH - rx_ - radius_) / vx_;
  } else if (vx_ < 0) {
    return (radius_ - rx_ + 0.2 * WIDTH) / vx_;
  } else {
    return INFINITY;
  }
}

// Returns the amount of time for this particle to collide with a horizontal
// wall, assuming no intervening collisions.
double Particle::TimeToHitHorizontalWall() const {
  if (vy_ > 0) {
    return (0.8 * HEIGHT - ry_ - radius_) / vy_;
  } else if (vy_ < 0) {
    return (radius_ - ry_ + 0.2 * HEIGHT) / vy_;
  } else {
    return INFINITY;
  }
}

// Updates the velocity of this particle and the specified particle according
// to the laws of elastic collision.
void Particle::BounceOff(Particle* that) {
  double dx {that->rx_ - rx_};
  double dy {that->ry_ - ry_};
  double dvx {that->vx_ - vx_};
  double dvy {that->vy_ - vy_};

  // Dot product dv.dr
  double dvdr {dx * dvx + dy * dvy};

  // Distance between particles centers at collision.
  double dist {radius_ + that->radius_};

  // Magnitude of normal force
  double magnitude {(1 + FRICTION) * mass_ * that->mass_ * dvdr /
    ((mass_ + that->mass_) * dist)};

  // Normal force in x and y directions
  double fx {magnitude * dx / dist};
  double fy {magnitude * dy / dist};

  // Update velocities according to normal force
  vx_ += fx / mass_;
  vy_ += fy / mass_;
  that->vx_ -= fx / that->mass_;
  that->vy_ -= fy / that->mass_;

  // Update collision counts
  collisions_count_++;
  that->collisions_count_++;
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
double Particle::KineticEnergy() const {
  double kinetic_energy {0.5 * mass_ * MASS_UNIT
      * pow(SPEED_UNIT * GetSpeed(), 2)};
  return kinetic_energy;
}

// Returns the particle's radius.
double Particle::GetRadius() const {
  return radius_;
}

// Sets the particle's radius.
void Particle::SetRadius(double radius) {
  radius_ = radius;
  circle_.setRadius(radius_);
  circle_.setOrigin(radius_, radius_);
}

// Returns the particle's speed.
double Particle::GetSpeed() const {
  return sqrt(vx_ * vx_ + vy_ * vy_);
}

// Sets the particle's color.
void Particle::SetColor(sf::Color color) {
  color_ = color;
  circle_.setFillColor(color_);
}

// Returns the rx coordinate.
double Particle::GetRx() const {
  return rx_;
}

// Returns the ry coordinate.
double Particle::GetRy() const {
  return ry_;
}

// Returns the particle's birthdate.
double Particle::GetBirthdate() const {
  return birthdate_;
}
