#include "raylib.h"

struct AnimData
{
  Rectangle rec;
  Vector2 pos;
  int frame;
  float updateTime;
  float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
  return data.pos.y >= windowHeight - data.rec.height;
};

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
  // update animation frame
  data.runningTime += deltaTime;

  if (data.runningTime >= data.updateTime)
  {
    data.runningTime = 0.0;
    data.rec.x = data.frame * data.rec.width;
    data.frame++;

    if (data.frame > maxFrame)
    {
      data.frame = 0;
    }
  }

  return data;
}

int main(void)
{
  // window dimensions
  const int windowWidth{512};
  const int windowHeight{380};
  InitWindow(windowWidth, windowHeight, "Dapper Dasher");

  Texture2D scarfy = LoadTexture("textures/scarfy.png");
  Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
  Texture2D background = LoadTexture("textures/far-buildings.png");
  Texture2D midground = LoadTexture("textures/back-buildings.png");
  Texture2D foreground = LoadTexture("textures/foreground.png");

  AnimData scarfyData;
  scarfyData.rec.x = 0.0;
  scarfyData.rec.y = 0.0;
  scarfyData.rec.width = scarfy.width / 6;
  scarfyData.rec.height = scarfy.height;
  scarfyData.pos.x = windowWidth / 2 - (scarfy.width / 6) / 2;
  scarfyData.pos.y = windowHeight - scarfy.height;
  scarfyData.frame = 0;
  scarfyData.updateTime = 1.0 / 12.0;
  scarfyData.runningTime = 0.0;

  const int numberOfNebulae{5};
  AnimData nebulae[numberOfNebulae];

  for (int i = 0; i < numberOfNebulae; i++)
  {
    nebulae[i].rec.x = 0.0;
    nebulae[i].rec.y = 0.0;
    nebulae[i].rec.width = nebula.width / 8;
    nebulae[i].rec.height = nebula.height / 8;
    nebulae[i].pos.x = windowWidth + 400 * i;
    nebulae[i].pos.y = windowHeight - nebula.height / 8;
    nebulae[i].frame = 0;
    nebulae[i].updateTime = 1.0 / 50.0;
    nebulae[i].runningTime = 0.0;
  }

  const int gravity{1'000};
  bool isInAir{false};
  int velocity{0};
  int nebulaVelocity{-200};
  const int jumpVelocity{-600};
  float bgX{};
  float mgX{};
  float fgX{};
  float finishLine{ nebulae[numberOfNebulae - 1].pos.x };
  bool collision{};

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);

    float dT{GetFrameTime()};

    bgX -= 20 * dT;

    if (bgX <= -background.width * 2)
    {
      bgX = 0;
    }

    mgX -= 40 * dT;

    if (mgX <= -midground.width * 2)
    {
      mgX = 0;
    }

    fgX -= 80 * dT;

    if (fgX <= -foreground.width * 2)
    {
      fgX = 0;
    }

    Vector2 bgPos1{bgX, 0.0};
    Vector2 bgPos2{bgX + background.width * 2, 0.0};
    DrawTextureEx(background, bgPos1, 0.0, 2.0, WHITE);
    DrawTextureEx(background, bgPos2, 0.0, 2.0, WHITE);

    Vector2 mgPos1{mgX, 0.0};
    Vector2 mgPos2{mgX + midground.width * 2, 0.0};
    DrawTextureEx(midground, mgPos1, 0.0, 2.0, WHITE);
    DrawTextureEx(midground, mgPos2, 0.0, 2.0, WHITE);

    Vector2 fgPos1{fgX, 0.0};
    Vector2 fgPos2{fgX + foreground.width * 2, 0.0};
    DrawTextureEx(foreground, fgPos1, 0.0, 2.0, WHITE);
    DrawTextureEx(foreground, fgPos2, 0.0, 2.0, WHITE);

    scarfyData.pos.y += velocity * dT;
    finishLine += nebulaVelocity * dT;

    if (isOnGround(scarfyData, windowHeight))
    {
      // rectangle is on the ground
      velocity = 0;
      isInAir = false;
    }
    else
    {
      // rectangle is in the air
      velocity += gravity * dT;
      isInAir = true;
    }

    if (!isInAir && IsKeyPressed(KEY_SPACE))
    {
      velocity += jumpVelocity;
    }

    if (!isInAir)
    {
      scarfyData = updateAnimData(scarfyData, dT, 5);
    }

    for (AnimData nebula : nebulae)
    {
      int pad{50};
      Rectangle nebRec{
        nebula.pos.x + pad,
        nebula.pos.y + pad,
        nebula.rec.width - pad * 2,
        nebula.rec.height - pad * 2
      };

      Rectangle scarfyRec{
        scarfyData.pos.x,
        scarfyData.pos.y,
        scarfyData.rec.width,
        scarfyData.rec.height
      };

      if (CheckCollisionRecs(nebRec, scarfyRec))
      {
        collision = true;
      }
    }

    if (collision)
    {
      DrawText("Game Over", windowWidth / 2 - 100, windowHeight / 2 - 50, 40, WHITE);
    }
    else if (finishLine < scarfyData.pos.x - 250)
    {
      DrawText("You win!", windowWidth / 2 - 100, windowHeight / 2 - 50, 40, WHITE);
    }
    else
    {
      DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);

      for (int i = 0; i < numberOfNebulae; i++)
      {
        nebulae[i] = updateAnimData(nebulae[i], dT, 7);
        nebulae[i].pos.x += nebulaVelocity * dT;
        DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
      }
    }

    EndDrawing();
  }

  UnloadTexture(scarfy);
  UnloadTexture(nebula);
  UnloadTexture(background);
  UnloadTexture(midground);
  UnloadTexture(foreground);
  CloseWindow();
}
