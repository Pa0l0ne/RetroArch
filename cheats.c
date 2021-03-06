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

#include "cheats.h"
#include "general.h"
#include "dynamic.h"
#include <file/config_file.h>
#include <file/config_file_macros.h>
#include <compat/strl.h>
#include <compat/posix_string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

void cheat_manager_apply_cheats(cheat_manager_t *handle)
{
   unsigned i;
   unsigned idx = 0;

   pretro_cheat_reset();
   for (i = 0; i < handle->size; i++)
   {
      if (handle->cheats[i].state)
         pretro_cheat_set(idx++, true, handle->cheats[i].code);
   }
}

cheat_manager_t *cheat_manager_load(const char *path)
{
   unsigned cheats = 0, i;
   cheat_manager_t *cheat = NULL;
   config_file_t *conf = config_file_new(path);

   if (!conf)
      return NULL;

   config_get_uint(conf, "cheats", &cheats);

   if (cheats == 0)
      return NULL;

   cheat = cheat_manager_new(cheats);

   if (!cheat)
      return NULL;

   for (i = 0; i < cheats; i++)
   {
      char key[64];
      char desc_key[256];
      char code_key[256];
      char enable_key[256];
      char *tmp = NULL;
      bool tmp_bool = false;

      snprintf(key, sizeof(key), "cheat%u", i);
      snprintf(desc_key, sizeof(desc_key), "cheat%u_desc", i);
      snprintf(code_key, sizeof(code_key), "cheat%u_code", i);
      snprintf(enable_key, sizeof(enable_key), "cheat%u_enable", i);

      if (config_get_string(conf, desc_key, &tmp))
         cheat->cheats[i].desc   = strdup(tmp);

      if (config_get_string(conf, code_key, &tmp))
         cheat->cheats[i].code   = strdup(tmp);

      if (config_get_bool(conf, enable_key, &tmp_bool))
         cheat->cheats[i].state  = tmp_bool;
   }

   config_file_free(conf);

   return cheat;
}

cheat_manager_t *cheat_manager_new(unsigned size)
{
   unsigned i;
   cheat_manager_t *handle = NULL;
   handle = (cheat_manager_t*)calloc(1, sizeof(struct cheat_manager));
   if (!handle)
      return NULL;

   handle->buf_size = handle->size = size;
   handle->cheats = (struct item_cheat*)
      calloc(handle->buf_size, sizeof(struct item_cheat));

   if (!handle->cheats)
   {
      handle->buf_size = 0;
      handle->size = 0;
      handle->cheats = NULL;
      return handle;
   }

   for (i = 0; i < handle->size; i++)
   {
      handle->cheats[i].desc   = NULL;
      handle->cheats[i].code   = NULL;
      handle->cheats[i].state  = false;
   }

   return handle;
}

bool cheat_manager_realloc(cheat_manager_t *handle, unsigned new_size)
{
   unsigned i;

   if (!handle->cheats)
      handle->cheats = (struct item_cheat*)
         calloc(new_size, sizeof(struct item_cheat));
   else
      handle->cheats = (struct item_cheat*)
         realloc(handle->cheats, new_size * sizeof(struct item_cheat));

   if (!handle->cheats)
   {
      handle->buf_size = handle->size = 0;
      handle->cheats = NULL;
      return false;
   }

   handle->buf_size = new_size;
   handle->size     = new_size;

   for (i = 0; i < handle->size; i++)
   {
      handle->cheats[i].desc    = NULL;
      handle->cheats[i].code    = NULL;
      handle->cheats[i].state   = false;
   }

   return true;
}

void cheat_manager_free(cheat_manager_t *handle)
{
   unsigned i;
   if (!handle)
      return;

   if (handle->cheats)
   {
      for (i = 0; i < handle->size; i++)
      {
         free(handle->cheats[i].desc);
         free(handle->cheats[i].code);
      }

      free(handle->cheats);
   }

   free(handle);
}

void cheat_manager_update(cheat_manager_t *handle, unsigned handle_idx)
{
   msg_queue_clear(g_extern.msg_queue);
   char msg[256];
   snprintf(msg, sizeof(msg), "Cheat: #%u [%s]: %s",
         handle_idx, handle->cheats[handle_idx].state ? "ON" : "OFF",
         (handle->cheats[handle_idx].desc) ? 
         (handle->cheats[handle_idx].desc) : (handle->cheats[handle_idx].code)
         );
   msg_queue_push(g_extern.msg_queue, msg, 1, 180);
   RARCH_LOG("%s\n", msg);
}


void cheat_manager_toggle(cheat_manager_t *handle)
{
   if (!handle)
      return;

   handle->cheats[handle->ptr].state ^= true;
   cheat_manager_apply_cheats(handle);
   cheat_manager_update(handle, handle->ptr);
}

void cheat_manager_index_next(cheat_manager_t *handle)
{
   if (!handle)
      return;
   handle->ptr = (handle->ptr + 1) % handle->size;
   cheat_manager_update(handle, handle->ptr);
}

void cheat_manager_index_prev(cheat_manager_t *handle)
{
   if (!handle)
      return;

   if (handle->ptr == 0)
      handle->ptr = handle->size - 1;
   else
      handle->ptr--;

   cheat_manager_update(handle, handle->ptr);
}
