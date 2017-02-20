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

std::tuple<SDL_Texture*, int, int> RenderFramedText(SDL_Renderer *renderer, TTF_Font *font, const std::string& text) {
  SDL_Surface* surface = TTF_RenderText_Shaded(font, text.c_str(), GetColor(TextColor::White), GetColor(TextColor::Black));
  SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_FreeSurface(surface);

  int width = surface->w + 2;
  int height = surface->h + 2;

  SDL_Texture *target_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

  SDL_SetRenderTarget(renderer, target_texture);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
  SDL_Rect target_rc { 0, 0, width, height };
  SDL_RenderFillRect(renderer, &target_rc);
  SDL_Rect dest_rc { 1, 1, width - 2, height - 2 };
  SDL_RenderCopy(renderer, text_texture, NULL, &dest_rc);
  SDL_SetRenderTarget(renderer, NULL);

  SDL_DestroyTexture(text_texture);
  return std::make_tuple(target_texture, width, height);
}
