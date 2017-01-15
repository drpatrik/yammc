#include "text.h"

#include <SDL.h>
#include <SDL_TTF.h>

void RenderText(SDL_Renderer *renderer, int x, int y, TTF_Font *font, const std::string& text, TextColor text_color, uint8_t alpha) {
  SDL_Color color;

  if (text_color == TextColor::Blue) {
    color = { 0, 0, 255, alpha };
  } else if (text_color == TextColor::Red) {
    color = { 255, 0, 0, alpha };
  } else if (text_color == TextColor::Green) {
    color = { 0, 255, 0, alpha };
  } else if (text_color == TextColor::Black) {
    color = { 0, 0, 0, alpha };
  } else {
    color = { 255, 255, 255, alpha };
  }
  SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

  int width, height;

  SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

  SDL_Rect rc{ x, y, width, height };

  SDL_RenderCopy(renderer, texture, nullptr, &rc);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}
