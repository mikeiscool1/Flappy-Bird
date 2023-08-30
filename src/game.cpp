#include "game.hpp"
#include <string>

Game::Game(): 
  isRunning(false),
  screen(Screen::AWAIT_BEGIN),
  bird({ (Window::WIDTH - Bird::WIDTH) / 2, (Window::HEIGHT - Bird::HEIGHT) / 2, Bird::WIDTH, Bird::HEIGHT }),
  gforce(9),
  counter(0)
 {}

Game::~Game() { clean(); }

int Game::init(const char* title, int x, int y, int w, int h) {
  srand(time(0));

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    
    return 1;
  }

  window = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_SHOWN);

  if (!window) {
    SDL_Log("Window creation failed! SDL_Error: %s\n", SDL_GetError());
    SDL_Quit();

    return 1;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    SDL_Log("Renderer creation failed! SDL_Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 1;
  }

  int ttf = TTF_Init();
  if (ttf != 0) {
    SDL_Log("Failed to initialize TrueType Format! SDL_Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 1;
  }

  font = TTF_OpenFont(Assets::Fonts::FONT, Assets::Fonts::SIZE);
  if (!font) {
    SDL_Log("Failed to open font! SDL_Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 1;
  }

  IMG_Init(IMG_INIT_PNG);

  SDL_Surface *temp = IMG_Load(Assets::Sprites::STILL);
  birdAnimationStill = SDL_CreateTextureFromSurface(renderer, temp);

  temp = IMG_Load(Assets::Sprites::UP);
  birdAnimationUp = SDL_CreateTextureFromSurface(renderer, temp);

  temp = IMG_Load(Assets::Sprites::POLE);
  poleTexture = SDL_CreateTextureFromSurface(renderer, temp);

  SDL_FreeSurface(temp);

  isRunning = true;
  return 0;
}

void Game::update() {
  if (screen == Screen::DIED) return gravity();
  else if (screen != Screen::PLAYING) return;
  
  gravity();
  shiftPolesLeft();

  static int pole_distance_counter = 0;

  pole_distance_counter++;
  if (pole_distance_counter == SpawnPole::DISTANCE) {
    pole_distance_counter = 0;
    spawnRandomPole();
  }

  if (checkPoleCollision()) screen = Screen::DIED;
  if (checkFinishedPole()) counter++;
}

void Game::render() {
  SDL_RenderClear(renderer);

  renderBackground();
  renderPoles();
  renderCounter();
  renderBird();

  SDL_RenderPresent(renderer);
}

void Game::clean() {
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

void Game::renderBackground() {
  SDL_SetRenderDrawColor(renderer, Colors_background);
  SDL_RenderClear(renderer);
}

void Game::handleEvents() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        isRunning = false;
        break;

      case SDL_KEYDOWN: case SDL_MOUSEBUTTONDOWN:
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym != SDLK_UP && event.key.keysym.sym != SDLK_SPACE) break;
        else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button != SDL_BUTTON_LEFT) break;

        if (screen == Screen::DIED) {
          // ensure animation of bird falling has finished
          if (bird.y + Bird::HEIGHT != Window::HEIGHT) break;

          screen = Screen::AWAIT_BEGIN;
          reset();
          break;
        } else if (screen == Screen::AWAIT_BEGIN) {
          screen = Screen::PLAYING;
          flap();
          break;
        }

        flap();
    }
  }
}

void Game::renderPoles() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  for (const Pole& pole : poles) {
    SDL_RenderCopy(renderer, poleTexture, NULL, &pole.top);
    SDL_RenderCopy(renderer, poleTexture, NULL, &pole.bottom);

    SDL_RenderFillRect(renderer, &pole.topBase);
    SDL_RenderFillRect(renderer, &pole.bottomBase);
  }
}

// magical function
void Game::spawnRandomPole() {
  using namespace SpawnPole;

  int random_height = MAX_HEIGHT + (rand() % (MIN_HEIGHT - MAX_HEIGHT + 1));

  SDL_Rect top = { Window::WIDTH, 0, WIDTH, random_height };
  SDL_Rect bottom = { Window::WIDTH, random_height + SPACE, WIDTH, Window::HEIGHT - (random_height + SPACE) };
  SDL_Rect topBase = { Window::WIDTH, random_height - 10, WIDTH, 10 };
  SDL_Rect bottomBase = { Window::WIDTH, random_height + SPACE, WIDTH, 10 };

  Pole pole = { top, bottom, topBase, bottomBase };
  poles.push_back(pole);

  if (poles.front().bottom.x + WIDTH < 0) poles.pop_front();
}

void Game::shiftPolesLeft() {
  for (Pole& pole : poles) {
    pole.bottom.x -= Bird::FLY_SPEED;
    pole.top.x -= Bird::FLY_SPEED;
    pole.topBase.x -= Bird::FLY_SPEED;
    pole.bottomBase.x -= Bird::FLY_SPEED;
  }
}

bool Game::checkPoleCollision() {
  static int bird_front = bird.x + Bird::WIDTH;

  for (Pole& pole : poles) {
    if (pole.top.x - bird_front > 0 || (pole.top.x + SpawnPole::WIDTH) - bird.x < 0) continue;

    if (bird.y < 0) return true;
    if (bird.y <= pole.top.y + pole.top.h) return true;
    if (bird.y + Bird::HEIGHT >= pole.bottom.y) return true;
  }

  return false;
}

bool Game::checkFinishedPole() {
  static int bird_front = bird.x + Bird::WIDTH;
  static bool finished = true;

  for (Pole& pole : poles) {
    if (pole.top.x - bird_front > 0 || (pole.top.x + SpawnPole::WIDTH) - bird.x < 0) continue;

    if (finished) return false;

    finished = true;
    return true;
  }

  finished = false;
  return false;
}

void Game::renderBird() {
  if (gforce <= 5) {
    SDL_RenderCopy(renderer, birdAnimationUp, NULL, &bird);

    bird.h = Bird::HEIGHT + 10;
    bird.w = Bird::WIDTH + 10;
  }
  else {
    SDL_RenderCopy(renderer, birdAnimationStill, NULL, &bird);

    bird.h = Bird::HEIGHT;
    bird.w = Bird::WIDTH;
  }
}

void Game::flap() {
  gforce = -Bird::FLY_POWER;
}

void Game::gravity() {
  bird.y += gforce;
  if (bird.y + Bird::HEIGHT > Window::HEIGHT) bird.y = Window::HEIGHT - Bird::HEIGHT;
  gforce += 1;

  if (bird.y + Bird::HEIGHT == Window::HEIGHT) screen = Screen::DIED;
}

void Game::reset() {
  poles.clear();
  bird.y = (Window::HEIGHT - Bird::HEIGHT) / 2;
  counter = 0;
}

void Game::renderCounter() {
  constexpr SDL_Color black = { 0, 0, 0 };

  static SDL_Surface* surface;
  static SDL_Texture* texture;
  static SDL_Rect rect;

  static int lastCount = -1;

  if (counter != lastCount) {
    static int pixelPerChar = 70;

    std::string textString = std::to_string(counter);
    const char* text = textString.c_str();
    int text_size = strlen(text);

    surface = TTF_RenderText_Solid(font, text, black);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect = { (Window::WIDTH - (pixelPerChar * text_size)) / 2, 50, pixelPerChar * text_size, 100 };

    lastCount++;
  }

  SDL_RenderCopy(renderer, texture, NULL, &rect);
}