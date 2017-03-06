#pragma once

#include <string>

#include <SDL.h>
#include <SDL_TTF.h>

enum class Color { White, Blue, Red, Green, Black, Yellow, Cyan };

void RenderText(SDL_Renderer *renderer, int x, int y, TTF_Font *font, const std::string& text, Color text_color);

std::tuple<SDL_Texture*, int, int> CreateTextureFromText(SDL_Renderer *renderer,
                                                         TTF_Font *font,
                                                         const std::string& text,
                                                         Color text_color);

std::tuple<SDL_Texture*, int, int> CreateTextureFromFramedText(SDL_Renderer *renderer,
                                                               TTF_Font *font,
                                                               const std::string& text,
                                                               Color text_color,
                                                               Color background_color);
