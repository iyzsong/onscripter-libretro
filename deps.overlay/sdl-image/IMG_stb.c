/*
  SDL_image:  An example image loading library for use with SDL
  Copyright (C) 1997-2012 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
  claim that you wrote the original software. If you use this software
  in a product, an acknowledgment in the product documentation would be
  appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
  misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/


#include "SDL_image.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC  SDL_malloc
#define STBI_REALLOC SDL_realloc
#define STBI_FREE    SDL_free
#include "stb_image.h"

typedef struct {
    SDL_RWops *src;
    int end;
} img_stb_t;

static int img_stb_read(void *user, char *data, int size)
{
    img_stb_t *ctx = (img_stb_t *)user;
    return SDL_RWread(ctx->src, data, 1, size);
}

static void img_stb_skip(void *user, int n)
{
    img_stb_t *ctx = (img_stb_t *)user;
    SDL_RWseek(ctx->src, n, RW_SEEK_CUR);
}

static int img_stb_eof(void *user)
{
    img_stb_t *ctx = (img_stb_t *)user;
    return SDL_RWtell(ctx->src) == ctx->end;
}

static SDL_Surface *IMG_LoadSTB_RW(SDL_RWops *src)
{
    int width, height, channels, depth = 0;
    img_stb_t ctx;
    Uint32 Rmask = 0, Gmask = 0, Bmask = 0, Amask = 0;
    ctx.src = src;
    SDL_RWseek(src, 0, RW_SEEK_END);
    ctx.end = SDL_RWtell(src);
    SDL_RWseek(src, 0, RW_SEEK_SET);
    stbi_io_callbacks clbk = {
        .read = img_stb_read,
        .skip = img_stb_skip,
        .eof  = img_stb_eof,
    };
    void *pixels = stbi_load_from_callbacks(&clbk, &ctx, &width, &height, &channels, 0);
    if (pixels == NULL) {
        IMG_SetError(stbi_failure_reason());
        return NULL;
    }
    switch (channels) {
    case STBI_grey:
        depth = 8;
        Rmask = 0xff;
        Gmask = 0xff;
        Bmask = 0xff;
        Amask = 0x00;
        break;
    case STBI_grey_alpha:
        depth = 16;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        Rmask = 0xff00;
        Gmask = 0xff00;
        Bmask = 0xff00;
        Amask = 0x00ff;
#else
        Rmask = 0x00ff;
        Gmask = 0x00ff;
        Bmask = 0x00ff;
        Amask = 0xff00;
#endif
        break;
    case STBI_rgb:
        depth = 24;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        Rmask = 0xff0000;
        Gmask = 0x00ff00;
        Bmask = 0x0000ff;
        Amask = 0x000000;
#else
        Rmask = 0x0000ff;
        Gmask = 0x00ff00;
        Bmask = 0xff0000;
        Amask = 0x000000;
#endif
        break;
    case STBI_rgb_alpha:
        depth = 32;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        Rmask = 0xff000000;
        Gmask = 0x00ff0000;
        Bmask = 0x0000ff00;
        Amask = 0x000000ff;
#else
        Rmask = 0x000000ff;
        Gmask = 0x0000ff00;
        Bmask = 0x00ff0000;
        Amask = 0xff000000;
#endif
        break;
    }
    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, 0, depth, Rmask, Gmask, Bmask, Amask);
    if (surface == NULL) {
        stbi_image_free(pixels);
        IMG_SetError(SDL_GetError());
        return NULL;
    }
    surface->w = width;
    surface->h = height;
    surface->pixels = pixels;
    surface->pitch = width * channels;
    SDL_SetClipRect(surface, NULL);
    return surface;
}

int IMG_InitPNG()
{
    return 0;
}

void IMG_QuitPNG()
{
}

int IMG_isPNG(SDL_RWops *src)
{
    int start;
    int is_PNG;
    Uint8 magic[4];

    if ( !src )
        return 0;
    start = SDL_RWtell(src);
    is_PNG = 0;
    if ( SDL_RWread(src, magic, 1, sizeof(magic)) == sizeof(magic) ) {
        if ( magic[0] == 0x89 &&
             magic[1] == 'P' &&
             magic[2] == 'N' &&
             magic[3] == 'G' ) {
            is_PNG = 1;
        }
    }
    SDL_RWseek(src, start, RW_SEEK_SET);
    return(is_PNG);
}

SDL_Surface *IMG_LoadPNG_RW(SDL_RWops *src)
{
    return IMG_LoadSTB_RW(src);
}

int IMG_InitJPG()
{
    return 0;
}

void IMG_QuitJPG()
{
}

int IMG_isJPG(SDL_RWops *src)
{
    return 1;
}

SDL_Surface *IMG_LoadJPG_RW(SDL_RWops *src)
{
    return IMG_LoadSTB_RW(src);
}
