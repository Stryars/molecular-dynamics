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
    radius_ {radius}, mass_ {mass},
    color_ {color} {
  circle_.setRadius(radius_ * HEIGHT);
  circle_.setOrigin(circle_.getRadius(), circle_.getRadius());
  circle_.setFillColor(color_);
  circle_.setPosition(rx_ * HEIGHT, ry * HEIGHT);
}

// Initializes a copy of a particle from a pointer.
Particle::Particle(Particle* p) :
    rx_ {p->rx_}, ry_ {p->ry_},
    vx_ {p->vx_}, vy_{p->vy_},
    radius_ {p->radius_}, mass_ {p->mass_},
    color_ {p->color_} {
  circle_.setRadius(radius_ * HEIGHT);
  circle_.setOrigin(circle_.getRadius(), circle_.getRadius());
  circle_.setFillColor(color_);
  circle_.setPosition(rx_ * HEIGHT, ry_ * HEIGHT);
}

// Initializes a copy of a particle.
Particle::Particle(const Particle& p) :
    rx_ {p.rx_}, ry_ {p.ry_},
    vx_ {p.vx_}, vy_{p.vy_},
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
    radius_ {0.01}, mass_ {0.5},
    color_ {sf::Color::Black} {
  circle_.setRadius(radius_ * HEIGHT);
  circle_.setOrigin(circle_.getRadius(), circle_.getRadius());
  circle_.setFillColor(color_);
  circle_.setPosition(rx_ * HEIGHT, ry_ * HEIGHT);
}

bool Particle::operator==(const Particle& rhs) const {
  return (this->rx_ == rhs.rx_);
}

// Moves this particle in a straight line, based on its velocity,
// for a specified amount of time dt.
void Particle::Move(double dt) {
  this->rx_ += this->vx_ * dt;
  this->ry_ += this->vy_ * dt;
}

// Draws this particle on the SFML window.
void Particle::Draw(sf::RenderWindow* window) {
  this->circle_.setPosition(this->rx_ * HEIGHT, this->ry_ * HEIGHT);
  window->draw(this->circle_);
}

// Returns the number of collisions involving this particle with either
// walls or other particles.
int Particle::Count() {
  return this->collisions_count_;
}

// Returns the amount of time for this particle to collide with the specified
// particle, assuming no intervening collisions.
double Particle::TimeToHit(Particle& that) {
  if (*this == that) {
    return INFINITY;
  }

  double dx {that.rx_ - this->rx_};
  double dy {that.ry_ - this->ry_};
  double dvx {that.vx_ - this->vx_};
  double dvy {that.vy_ - this->vy_};

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
  double sigma {this->radius_ + that.radius_};

  double d {(dvdr * dvdr) - dvdv * (drdr - sigma * sigma)};
  if (d < 0) {
    return INFINITY;
  }

  return -(dvdr + sqrt(d)) / dvdv;
}

// Returns the amount of time for this particle to collide with a vertical
// wall, assuming no intervening collisions.
double Particle::TimeToHitVerticalWall() {
  if (this->vx_ > 0) {
    return (1 - this->rx_ - this->radius_) / this->vx_;
  } else if (this->vx_ < 0) {
    return (this->radius_ - this->rx_) / this->vx_;
  } else {
    return INFINITY;
  }
}

// Returns the amount of time for this particle to collide with a horizontal
// wall, assuming no intervening collisions.
double Particle::TimeToHitHorizontalWall() {
  printf("Time to hit horizontal\n");
  if (this->vy_ > 0) {
    printf("vy>0\n");
    return (1 - this->ry_ - this->radius_) / this->vy_;
  } else if (this->vy_ < 0) {
    return (this->radius_ - this->ry_) / this->vy_;
  } else {
    return INFINITY;
  }
}

// Updates the velocity of this particle and the specified particle according
// to the laws of elastic collision.
void Particle::BounceOff(Particle& that) {
  double dx {that.rx_ - this->rx_};
  double dy {that.ry_ - this->ry_};
  double dvx {that.vx_ - this->vx_};
  double dvy {that.vy_ - this->vy_};

  // Dot product dv.dr
  double dvdr {dx * dvx + dy * dvy};

  // Distance between particles centers at collision.
  double dist {this->radius_ + that.radius_};

  // Magnitude of normal force
  double magnitude {2 * this->mass_ * that.mass_ * dvdr /
    ((this->mass_ + that.mass_) * dist)};

  // Normal force in x and y directions
  double fx {magnitude * dx / dist};
  double fy {magnitude * dy / dist};

  // Update velocities according to normal force
  this->vx_ += fx / this->mass_;
  this->vy_ += fy / this->mass_;
  that.vx_ -= fx / that.mass_;
  that.vy_ -= fy / that.mass_;

  // Update collision counts
  this->collisions_count_++;
  that.collisions_count_++;
}

// Updates the velocity of this particle upon collision with a vertical wall.
void Particle::BounceOffVerticalWall() {
  this->vx_ = -this->vx_;
  this->collisions_count_++;
}

// Updates the velocity of this particle upon collision with a
// horizontal wall.
void Particle::BounceOffHorizontalWall() {
  printf("BounceOffHorizontalWall\n");
  this->vy_ = -this->vy_;
  this->collisions_count_++;
}

// Returns the kinetic energy of this particle.
double Particle::KineticEnergy() {
  return 0.5 * this->mass_ * (this->vx_ * this->vx_ + this->vy_ * this->vy_);
}
