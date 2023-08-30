#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "constants.hpp"
#include <deque>

class Game {
public:
  Game();
  ~Game();

  enum Screen {
    DIED,
    AWAIT_BEGIN,
    PLAYING
  };

  int init(const char* title, int x, int y, int w, int h);
  void handleEvents();
  void clean();

  void update();
  void render();

  inline bool running() const { return isRunning; };
  inline bool getScreen() const { return screen; }
private:
  bool isRunning;
  Screen screen;
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;

  SDL_Rect bird;
  SDL_Texture *birdAnimationStill;
  SDL_Texture *birdAnimationUp;
  SDL_Texture *poleTexture;

  int gforce;

  unsigned int counter;

  struct Pole {
    SDL_Rect top;
    SDL_Rect bottom;
    SDL_Rect topBase;
    SDL_Rect bottomBase;
  };
  std::deque<Pole> poles;

  // render functions
  void renderBackground();
  void renderPoles();
  void renderBird();
  void renderCounter();

  // update functions
  void flap();
  void gravity();
  void shiftPolesLeft();
  void reset();

  // this function will also pop the first pole if it is off screen.
  // Poles are spawned just before going into the screen
  void spawnRandomPole();

  bool checkPoleCollision();
  bool checkFinishedPole();
};