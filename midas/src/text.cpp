#include "text.h"

#include <SDL.h>
#include <SDL_TTF.h>

namespace {

SDL_Color GetColor(TextColor color, uint8_t alpha = 0) {
  if (color == TextColor::Blue) {
    return { 0, 0, 255, alpha };
  } else if (color == TextColor::Red) {
    return { 255, 0, 0, alpha };
  } else if (color == TextColor::Green) {
    return  { 0, 255, 0, alpha };
  } else if (color == TextColor::Black) {
    return  { 0, 0, 0, alpha };
  } else {
    return  { 255, 255, 255, alpha };
  }
}

}

void RenderText(SDL_Renderer *renderer, int x, int y, TTF_Font *font, const std::string& text, TextColor text_color, uint8_t alpha) {
  SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), GetColor(text_color, alpha));
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

  int width, height;

  SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

  SDL_Rect rc{ x, y, width, height };

  SDL_RenderCopy(renderer, texture, nullptr, &rc);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

SDL_Texture* CreateScoreSign(SDL_Renderer *renderer, TTF_Font *font, const std::string& text) {
  SDL_Surface* surface = TTF_RenderText_Shaded(font, text.c_str(), GetColor(TextColor::White), GetColor(TextColor::Black));
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_FreeSurface(surface);

  return texture;
}
