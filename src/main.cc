// Copyright 2018 <Samuel Diebolt>

#include <cstdio>

#include "SFML/Graphics.hpp"
#include "game.h"

int main(int argc, char* argv[]) {
  if (argc != 1) {
    printf("No argument needed for %s.\n", argv[0]);
  }

  Game game;
  game.Run();

  return 0;
}
