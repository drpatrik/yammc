#pragma once

#include <string>

#include <SDL.h>
#include <SDL_TTF.h>

enum class TextColor { White, Blue, Red, Green, Black };

void RenderText(SDL_Renderer *renderer, int x, int y, TTF_Font *font, const std::string& text, TextColor text_color, uint8_t alpha = 0);
std::tuple<SDL_Texture*, int, int> CreateTextureFromFramedText(SDL_Renderer *renderer, TTF_Font *font, const std::string& text);
