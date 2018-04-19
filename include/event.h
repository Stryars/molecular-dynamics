// Copyright 2018 <Samuel Diebolt>

#pragma once

#include "particle.h"

class Event {
public:
  // Initializes a new event to occur at time t, involving particles a and b
  Event(double t, Particle* a, Particle* b);

  // Compare times when two events will occur
  int CompareTo(Event* that);

  // Has any collision occurred between when event was created and now?
  bool IsValid();

private:
  // Time that event is scheduled to occur
  double time_;

  // Particles involved in event, possibly null
  Particle *a_, *b_;

  // Collision counts at event creation
  int collisions_count_a_, collisions_count_b_;
};
