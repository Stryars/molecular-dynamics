// Copyright 2018 <Samuel Diebolt>

#include "event.h"
#include "particle.h"

// Initializes a new event to occur at time t, involving particles a and b
Event::Event(Event::Type type, double t, Particle* a, Particle* b) :
    type_ {type}, time_ {t}, a_ {a}, b_ {b},
    collisions_count_a_ {0}, collisions_count_b_ {0} {
  if (a != nullptr) {
    collisions_count_a_ = a->Count();
  } else {
    collisions_count_a_ = -1;
  }

  if (b != nullptr) {
    collisions_count_b_ = b->Count();
  } else {
    collisions_count_b_ = -1;
  }
}

// > operator for the piority queue
bool Event::operator>(const Event& rhs) const {
  return time_ > rhs.time_;
}

// Has any collision occurred between when event was created and now?
bool Event::IsValid() const {
  if (a_ != nullptr && a_->Count() != collisions_count_a_) {
    return false;
  }
  if (b_ != nullptr && b_->Count() != collisions_count_b_) {
    return false;
  }

  return true;
}

// Returns the time that event is scheduled to occur
double Event::GetTime() const {
  return time_;
}

// Returns particle A
Particle* Event::GetParticleA() const {
  return a_;
}

// Returns particle B
Particle* Event::GetParticleB() const {
  return b_;
}

// Returns event type
Event::Type Event::GetType() const {
  return type_;
}

// Prints event type.
void Event::PrintType() const {
  printf("Event type: ");
  switch(type_) {
    case Event::Type::kParticleParticle:
      printf("Particle-Particle\n");
      break;
    case Event::Type::kHorizontalWall:
      printf("Particle-Horizontal wall\n");
      break;
    case Event::Type::kVerticalWall:
      printf("Particle-Vertical wall\n");
      break;
    case Event::Type::kRedraw:
      // printf("Redraw\n");
      break;
    default:
      printf("None\n");
      break;
  }
}
