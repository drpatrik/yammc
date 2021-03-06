#pragma once

#include "function_caller.h"

#include <tuple>
#include <string>
#include <memory>

#include <SDL.h>
#include <SDL_ttf.h>

enum class Color { White, Blue, Red, Green, Black, Yellow, Cyan };

using UniqueTexturePtr = std::unique_ptr<SDL_Texture, function_caller<void(SDL_Texture*), &SDL_DestroyTexture>>;

void RenderText(SDL_Renderer *renderer, int x, int y, TTF_Font *font, const std::string& text,
                Color text_color);

std::tuple<UniqueTexturePtr, int, int> CreateTextureFromText(SDL_Renderer *renderer, TTF_Font *font,
                                                             const std::string& text, Color text_color);

std::tuple<UniqueTexturePtr, int, int> CreateTextureFromFramedText(SDL_Renderer *renderer, TTF_Font *font,
                                                                   const std::string& text, Color text_color,
                                                                   Color background_color);
