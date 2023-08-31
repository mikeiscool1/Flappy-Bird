#pragma once

namespace Window {
  constexpr int HEIGHT = 800;
  constexpr int WIDTH = 800;
}

namespace FPS {
  constexpr int FPS = 60;
  constexpr int FRAME_DELAY = 1000 / FPS;
}

namespace Assets {
  namespace Fonts {
    constexpr char const *FONT = "assets/fonts/font.ttf";
    constexpr int SIZE = 1000;
  }

  namespace Sprites {
    constexpr char const *STILL = "assets/sprites/still.png";
    constexpr char const *UP = "assets/sprites/up.png";
    constexpr char const *POLE = "assets/sprites/pole.png";
  }
}

namespace Bird {
  constexpr int HEIGHT = 48;
  constexpr int WIDTH = 68;
  constexpr int FLY_SPEED = 3;
  constexpr int FLY_POWER = 10;
  constexpr float GRAVITY = 0.7f;
}

namespace SpawnPole {
  constexpr int SPACE = Bird::HEIGHT * 4;
  constexpr int MAX_HEIGHT = 200;
  constexpr int MIN_HEIGHT = Window::HEIGHT - MAX_HEIGHT;

  constexpr int WIDTH = Bird::WIDTH;
  constexpr int DISTANCE = 300 / Bird::FLY_SPEED;
}

#define Colors_background 50, 0, 143, 255