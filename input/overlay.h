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

#ifndef INPUT_OVERLAY_H__
#define INPUT_OVERLAY_H__

#include <boolean.h>
#include "../libretro.h"
#include "../gfx/image/image.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Overlay driver acts as a medium between input drivers 
 * and video driver.
 *
 * Coordinates are fetched from input driver, and an 
 * overlay with pressable actions are displayed on-screen.
 *
 * This interface requires that the video driver has support 
 * for the overlay interface.
 */

typedef struct video_overlay_interface
{
   void (*enable)(void *data, bool state);
   bool (*load)(void *data,
         const struct texture_image *images, unsigned num_images);
   void (*tex_geom)(void *data, unsigned image,
         float x, float y, float w, float h);
   void (*vertex_geom)(void *data, unsigned image,
         float x, float y, float w, float h);
   void (*full_screen)(void *data, bool enable);
   void (*set_alpha)(void *data, unsigned image, float mod);
} video_overlay_interface_t;

enum overlay_hitbox
{
   OVERLAY_HITBOX_RADIAL = 0,
   OVERLAY_HITBOX_RECT
};

enum overlay_type
{
   OVERLAY_TYPE_BUTTONS = 0,
   OVERLAY_TYPE_ANALOG_LEFT,
   OVERLAY_TYPE_ANALOG_RIGHT,
   OVERLAY_TYPE_KEYBOARD
};

struct overlay_desc
{
   float x;
   float y;

   enum overlay_hitbox hitbox;
   float range_x, range_y;
   float range_x_mod, range_y_mod;
   float mod_x, mod_y, mod_w, mod_h;
   float delta_x, delta_y;

   enum overlay_type type;
   uint64_t key_mask;
   float analog_saturate_pct;

   unsigned next_index;
   char next_index_name[64];

   struct texture_image image;
   unsigned image_index;

   float alpha_mod;
   float range_mod;

   bool updated;
   bool movable;
};

struct overlay
{
   struct overlay_desc *descs;
   size_t size;

   struct texture_image image;

   bool block_scale;
   float mod_x, mod_y, mod_w, mod_h;
   float x, y, w, h;
   float scale;
   float center_x, center_y;

   bool full_screen;

   char name[64];

   struct texture_image *load_images;
   unsigned load_images_size;
};

struct input_overlay
{
   void *iface_data;
   const video_overlay_interface_t *iface;
   bool enable;

   bool blocked;

   struct overlay *overlays;
   const struct overlay *active;
   size_t index;
   size_t size;

   unsigned next_index;
   char *overlay_path;
};

typedef struct input_overlay input_overlay_t;

typedef struct input_overlay_state
{
   /* This is a bitmask of (1 << key_bind_id). */
   uint64_t buttons;
   /* Left X, Left Y, Right X, Right Y */
   int16_t analog[4]; 
   uint32_t keys[RETROK_LAST / 32 + 1];
} input_overlay_state_t;

#define OVERLAY_GET_KEY(state, key) (((state)->keys[(key) / 32] >> ((key) % 32)) & 1)
#define OVERLAY_SET_KEY(state, key) (state)->keys[(key) / 32] |= 1 << ((key) % 32)
#define OVERLAY_CLEAR_KEY(state, key) (state)->keys[(key) / 32] &= ~(1 << ((key) % 32))

input_overlay_t *input_overlay_new(const char *overlay, bool enable);
void input_overlay_free(input_overlay_t *ol);

void input_overlay_enable(input_overlay_t *ol, bool enable);

bool input_overlay_full_screen(input_overlay_t *ol);

/* norm_x and norm_y are the result of
 * input_translate_coord_viewport(). */
void input_overlay_poll(input_overlay_t *ol,
      input_overlay_state_t *out, int16_t norm_x, int16_t norm_y);

/* called after all the input_overlay_poll calls to
 * update the range modifiers for pressed/unpressed regions
 * and alpha mods */
void input_overlay_post_poll(input_overlay_t *ol);

/* Call when there is nothing to poll. Allows overlay to
 * clear certain state. */
void input_overlay_poll_clear(input_overlay_t *ol);

/* Sets a modulating factor for alpha channel. Default is 1.0.
 * The alpha factor is applied for all overlays. */
void input_overlay_set_alpha_mod(input_overlay_t *ol, float mod);

/* Scales the overlay by a factor of scale. */
void input_overlay_set_scale_factor(input_overlay_t *ol, float scale);

void input_overlay_next(input_overlay_t *ol);

#ifdef __cplusplus
}
#endif

#endif

