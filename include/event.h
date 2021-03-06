// Copyright 2018, Samuel Diebolt <samuel.diebolt@espci.fr>

#pragma once

#include "include/particle.h"

// A class describing an event: particle-particle collision, particle-wall
// collision or redraw of each particle.
// The collision system uses a priority queue to store all events.
class Event {
 public:
  enum class Type {
    kParticleParticle,
    kVerticalWall,
    kHorizontalWall,
    kRedraw
  };

  // Initializes a new event to occur at time t, involving particles a and b.
  Event(Type type, double t, Particle* a = nullptr, Particle* b = nullptr);

  // > operator for the piority queue.
  bool operator>(const Event& rhs) const;

  // Has any collision occurred between when event was created and now?
  bool IsValid() const;

  // Returns the time that event is scheduled to occur.
  double GetTime() const;

  // Returns particle A.
  Particle* GetParticleA() const;

  // Returns particle B.
  Particle* GetParticleB() const;

  // Returns event type.
  Type GetType() const;

 private:
  // Event type
  Type type_;

  // Time that event is scheduled to occur
  double time_;

  // Pointers to particles involved in event, possibly null
  Particle *a_, *b_;

  // Collision counts at event creation
  int collisions_count_a_, collisions_count_b_;
};
