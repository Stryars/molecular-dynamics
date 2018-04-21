// Copyright 2018 <Samuel Diebolt>

#include "event.h"
#include "particle.h"

// Initializes a new event to occur at time t, involving particles a and b
Event::Event(double t, Particle* a, Particle* b) :
    time_ {t}, a_ {a}, b_ {b} {
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
  return this->GetTime() > rhs.GetTime();
}

// Has any collision occurred between when event was created and now?
bool Event::IsValid() {
  if (this->a_ != nullptr && this->a_->Count() != this->collisions_count_a_) {
    return false;
  }
  if (this->b_ != nullptr && this->b_->Count() != this->collisions_count_a_) {
    return false;
  }

  return true;
}

// Returns the time that event is scheduled to occur
double Event::GetTime() const {
  return this->time_;
}

// Returns particle A
Particle* Event::GetParticleA() const {
  return this->a_;
}

// Returns particle B
Particle* Event::GetParticleB() const {
  return this->b_;
}
