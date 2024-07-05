/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 */

#include <assert.h>
#include <string.h>

#include "Sound.h"

SDL_AudioSpec MVC_AUDIO_DEFAULT = {
	.freq = 44100,
	.format = AUDIO_S16,
	.channels = 2,
	.samples = 2048
};

#define _Class _Sound

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Sound *this = (Sound *) self;

	release(this->data);

	super(Object, self, dealloc);
}

#pragma mark - Sound

/**
 * @fn Sound *Sound::soundWithBytes(const uint8_t *bytes, size_t length)
 * @memberof Sound
 */
static Sound *soundWithBytes(const uint8_t *bytes, size_t length) {
	return $(alloc(Sound), initWithBytes, bytes, length);
}

/**
 * @fn Sound *Sound::soundWithData(const Data *data)
 * @memberof Sound
 */
static Sound *soundWithData(Data *data) {
	return $(alloc(Sound), initWithData, data);
}

/**
 * @fn Sound *Sound::soundWithResource(const Resource *resource)
 * @memberof Sound
 */
static Sound *soundWithResource(const Resource *resource) {
	return $(alloc(Sound), initWithResource, resource);
}

/**
 * @fn Sound *Sound::soundWithResourceName(const char *name)
 * @memberof Sound
 */
static Sound *soundWithResourceName(const char *name) {
	return $(alloc(Sound), initWithResourceName, name);
}

/**
 * @fn Sound *Sound::initWithBytes(Sound *self, const uint8_t *bytes, size_t length)
 * @memberof Sound
 */
static Sound *initWithBytes(Sound *self, const uint8_t *bytes, size_t length) {

	self = (Sound *) super(Object, self, init);
	if (self) {

		assert(bytes);
		assert(length);

		SDL_RWops *ops = SDL_RWFromConstMem(bytes, (Uint32) length);
		if (ops) {
			struct {
				Uint8 *bytes;
				Uint32 length;
			} buffer;

			SDL_LoadWAV_RW(ops, 0, &self->spec, &buffer.bytes, &buffer.length);
			if (self->spec.format) {
				self->data = $$(Data, dataWithConstMemory, buffer.bytes, buffer.length);
				assert(self->data);
			} else {
				self = release(self);
			}

			SDL_FreeRW(ops);
		}
	}

	return self;
}

/**
 * @fn Sound *Sound::initWithData(Sound *self, Data *data)
 * @memberof Sound
 */
static Sound *initWithData(Sound *self, const Data *data) {
	return $(self, initWithBytes, data->bytes, data->length);
}

/**
 * @fn Sound *Sound::initWithResource(Sound *self, const Resource *resource)
 * @memberof Sound
 */
static Sound *initWithResource(Sound *self, const Resource *resource) {

	if (resource) {
		self = $(self, initWithData, resource->data);
	} else {
		self = release(self);
	}

	return self;
}

/**
 * @fn Sound *Sound::initWithResourceName(Sound *self, const char *name)
 * @memberof Sound
 */
static Sound *initWithResourceName(Sound *self, const char *name) {

	Resource *resource = $$(Resource, resourceWithName, name);

	self = $(self, initWithResource, resource);

	release(resource);

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((SoundInterface *) clazz->interface)->soundWithBytes = soundWithBytes;
	((SoundInterface *) clazz->interface)->soundWithData = soundWithData;
	((SoundInterface *) clazz->interface)->soundWithResource = soundWithResource;
	((SoundInterface *) clazz->interface)->soundWithResourceName = soundWithResourceName;
	((SoundInterface *) clazz->interface)->initWithBytes = initWithBytes;
	((SoundInterface *) clazz->interface)->initWithData = initWithData;
	((SoundInterface *) clazz->interface)->initWithResource = initWithResource;
	((SoundInterface *) clazz->interface)->initWithResourceName = initWithResourceName;
}

/**
 * @fn Class *Sound::_Sound(void)
 * @memberof Sound
 */
Class *_Sound(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Sound",
			.superclass = _Object(),
			.instanceSize = sizeof(Sound),
			.interfaceOffset = offsetof(Sound, interface),
			.interfaceSize = sizeof(SoundInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
