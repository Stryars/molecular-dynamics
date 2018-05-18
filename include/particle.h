// Copyright 2018, Samuel Diebolt <samuel.diebolt@espci.fr>

#pragma once

#include <random>
#include <SFML/Graphics.hpp>

#include "include/main.h"

class Particle {
 public:
  // Initializes a particle with specified position, velocity, radius,
  // mass and color.
  Particle(double birthdate, double rx, double ry, double vx, double vy,
      double radius, double mass, sf::Color color);

  // Necessary for TimeToHit().
  bool operator==(const Particle& rhs) const;

  // Moves this particle in a straight line, based on its velocity,
  // for a specified amount of time dt.
  void Move(double dt);

  // Draws this particle on the SFML window.
  void Draw(sf::RenderWindow* window) const;

  // Returns the number of collisions involving this particle with either
  // walls or other particles.
  int Count() const;

  // Returns the amount of time for this particle to collide with the specified
  // particle, assuming no intervening collisions.
  double TimeToHit(const Particle& that) const;

  // Returns the amount of time for this particle to collide with a vertical
  // wall, assuming no intervening collisions.
  double TimeToHitVerticalWall(double wall_size, double wall_speed) const;

  // Returns the amount of time for this particle to collide with a horizontal
  // wall, assuming no intervening collisions.
  double TimeToHitHorizontalWall(double wall_size, double wall_speed) const;

  // Updates the velocity of this particle and the specified particle according
  // to the laws of elastic collision.
  void BounceOff(Particle* that);

  // Updates the velocity of this particle upon collision with a vertical wall.
  void BounceOffVerticalWall(double wall_speed);

  // Updates the velocity of this particle upon collision with a
  // horizontal wall.
  void BounceOffHorizontalWall(double wall_speed);

  // Returns the kinetic energy of this particle.
  double KineticEnergy() const;

  // Returns the particle's radius.
  double GetRadius() const;

  // Sets the particle's radius.
  void SetRadius(double radius);

  // Returns the particle's speed.
  double GetSpeed() const;

  // Sets the particle's color.
  void SetColor(sf::Color color);

  // Returns the rx coordinate.
  double GetRx() const;

  // Sets the rx coordinate.
  void SetRx(double rx);

  // Returns the ry coordinate.
  double GetRy() const;

  // Sets the ry coordinate.
  void SetRy(double ry);

  // Returns the particle's birthdate.
  double GetBirthdate() const;

 private:
  double birthdate_;

  double rx_, ry_;            // Position
  double vx_, vy_;            // Velocity

  int collisions_count_;      // Number of collisions so far

  double radius_;             // Radius
  double mass_;         // Mass

  sf::Color color_;           // Color

  sf::CircleShape circle_;    // SFML shape
};
