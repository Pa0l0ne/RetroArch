/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 * 
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef __RARCH_NETPLAY_H
#define __RARCH_NETPLAY_H

#include <stdint.h>
#include <stddef.h>
#include <boolean.h>
#include "libretro.h"
#include "retro.h"

void input_poll_net(void);

int16_t input_state_net(unsigned port, unsigned device,
      unsigned idx, unsigned id);

void video_frame_net(const void *data, unsigned width,
      unsigned height, size_t pitch);

void audio_sample_net(int16_t left, int16_t right);

size_t audio_sample_batch_net(const int16_t *data, size_t frames);

int16_t input_state_spectate(unsigned port, unsigned device,
      unsigned idx, unsigned id);

int16_t input_state_spectate_client(unsigned port, unsigned device,
      unsigned idx, unsigned id);

typedef struct netplay netplay_t;

bool network_init(void);

void network_deinit(void);

/* Creates a new netplay handle. A NULL host means we're 
 * hosting (user 1). :) */
netplay_t *netplay_new(const char *server,
      uint16_t port, unsigned frames,
      const struct retro_callbacks *cb, bool spectate,
      const char *nick);

void netplay_free(netplay_t *handle);

/* On regular netplay, flip who controls user 1 and 2. */
void netplay_flip_users(netplay_t *handle);

/* Call this before running retro_run(). */
void netplay_pre_frame(netplay_t *handle);

/* Call this after running retro_run(). */
void netplay_post_frame(netplay_t *handle);

#endif

