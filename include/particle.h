// Copyright 2018 <Samuel Diebolt>

#pragma once

#include <random>

#include "SFML/Graphics.hpp"

class Particle {
public:
  // Initializes a particle with specified position, velocity, radius,
  // mass and color.
  Particle(double rx, double ry, double vx, double vy, const double radius,
    const double mass, sf::Color color);

  // Initializes a copy of a particle from a pointer.
  Particle(Particle* p);

  // Initializes a copy of a particle.
  Particle(const Particle& p);

  // Initializes a particle with random position and velocity.
  Particle();

  // Moves this particle in a straight line, based on its velocity,
  // for a specified amount of time dt.
  void Move(double dt);

  // Draws this particle on the SFML window.
  void Draw(sf::RenderWindow* window);

  // Returns the number of collisions involving this particle with either
  // walls or other particles.
  int Count();

  // Returns the amount of time for this particle to collide with the specified
  // particle, assuming no intervening collisions.
  double TimeToHit(Particle* that);

  // Returns the amount of time for this particle to collide with a vertical
  // wall, assuming no intervening collisions.
  double TimeToHitVerticalWall();

  // Returns the amount of time for this particle to collide with a horizontal
  // wall, assuming no intervening collisions.
  double TimeToHitHorizontalWall();

  // Updates the velocity of this particle and the specified particle according
  // to the laws of elastic collision.
  void BounceOff(Particle* that);

  // Updates the velocity of this particle upon collision with a vertical wall.
  void BounceOffVerticalWall();

  // Updates the velocity of this particle upon collision with a
  // horizontal wall.
  void BounceOffHorizontalWall();

  // Returns the kinetic energy of this particle.
  double KineticEnergy();

private:
  double rx_, ry_;            // Position
  double vx_, vy_;            // Velocity

  double collisions_count_;   // Number of collisions so far

  const double radius_;       // Radius
  const double mass_;         // Mass

  sf::Color color_;           // Color

  sf::CircleShape circle_;    // SFML shape
};
