/*
Copyright 2017 Zachary Blystone <zakblystone@gmail.com>

This file is part of Metacade.

Metacade is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Metacade is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Metacade.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
===============================================================================
lua_sound.cpp:
===============================================================================
*/

#include "lua_private.h"
#include "sound/sound_public.h"

static IGameInstance* getGameInstance(lua_State *L)
{
	lua_getglobal(L, "__gameinstance");
	IGameInstance* instance = (IGameInstance*) lua_touserdata(L, -1);
	lua_pop(L, 1);
	return instance;
}

MODULE_FUNCTION_DEF(sound_playsound)
{
	IGameInstance* instance = getGameInstance(L);
	if ( instance == nullptr ) return 0;

	ISoundMixer* mixer = instance->getSoundMixer();
	if ( mixer == nullptr ) return 0;

	CAssetRef* sound = toAssetRef(L, 1);
	if ( sound == nullptr || sound->getType() != ASSET_SOUND ) return 0;

	int32 targetChannel = luaL_optint(L, 2, EChannelID::CHANNEL_ANY);

	uint32 channel = mixer->playSound(*sound, targetChannel);
	lua_pushinteger(L, channel);

	return 1;
}

MODULE_FUNCTION_DEF(sound_stopsound)
{
	IGameInstance* instance = getGameInstance(L);
	if ( instance == nullptr ) return 0;

	ISoundMixer* mixer = instance->getSoundMixer();
	if ( mixer == nullptr ) return 0;

	int32 channel = (int32) luaL_checkinteger(L, 1);

	mixer->stopSound(channel);
	return 0;
}

MODULE_FUNCTION_DEF(sound_setpitch)
{
	IGameInstance* instance = getGameInstance(L);
	if ( instance == nullptr ) return 0;

	ISoundMixer* mixer = instance->getSoundMixer();
	if ( mixer == nullptr ) return 0;

	int32 channel = (int32) luaL_checkinteger(L, 1);
	float pitch = (float) luaL_optnumber(L, 2, 1.f);

	mixer->setChannelPitch(channel, pitch);
	lua_pushinteger(L, channel);
	return 1;
}

MODULE_FUNCTION_DEF(sound_setvolume)
{
	IGameInstance* instance = getGameInstance(L);
	if ( instance == nullptr ) return 0;

	ISoundMixer* mixer = instance->getSoundMixer();
	if ( mixer == nullptr ) return 0;

	int32 channel = (int32) luaL_checkinteger(L, 1);
	float volume = (float) luaL_optnumber(L, 2, 1.f);

	mixer->setChannelVolume(channel, volume);
	lua_pushinteger(L, channel);
	return 1;
}

MODULE_FUNCTION_DEF(sound_setlooping)
{
	IGameInstance* instance = getGameInstance(L);
	if ( instance == nullptr ) return 0;

	ISoundMixer* mixer = instance->getSoundMixer();
	if ( mixer == nullptr ) return 0;

	int32 channel = (int32) luaL_checkinteger(L, 1);
	bool loop = lua_toboolean(L, 2) == 1 ? true : false;

	mixer->setChannelLooping(channel, loop);
	lua_pushinteger(L, channel);
	return 1;
}

static const luaL_Reg soundlib[] = {
	{"play", sound_playsound},
	{"stop", sound_stopsound},
	{"setPitch", sound_setpitch},
	{"setVolume", sound_setvolume},
	{"setLooping", sound_setlooping},
	{nullptr, nullptr}
};

void Arcade::openLuaSoundModule(lua_State* L)
{
	luaL_register(L, "sound", soundlib);
}
