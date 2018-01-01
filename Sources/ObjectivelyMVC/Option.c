/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <assert.h>

#include <ObjectivelyMVC/Option.h>
#include <ObjectivelyMVC/Select.h>

#define _Class _Option

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Option *this = (Option *) self;

	release(this->title);

	super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	Option *this = (Option *) self;

	const SDL_Rect *f = &this->view.frame;

	return str("%s@%p \"%s:%p\"(%d,%d) %dx%d", this->view.identifier ?: self->clazz->name, self,
			   this->title->text, this->value, f->x, f->y, f->w, f->h);
}

#pragma mark - View

/**
 * @see View::acceptsFirstResponder(const View *)
 */
static _Bool acceptsFirstResponder(const View *self) {
	return true;
}

/**
 * @see View::matchesSelector(const View *, const SimpleSelector *)
 */
static _Bool matchesSelector(const View *self, const SimpleSelector *simpleSelector) {

	assert(simpleSelector);

	const Option *this = (Option *) self;

	switch (simpleSelector->type) {
		case SimpleSelectorTypePseudo:
			if (strcmp("selected", simpleSelector->pattern) == 0) {
				return this->isSelected;
			}
			break;
		default:
			break;
	}

	return super(View, self, matchesSelector, simpleSelector);
}

#pragma mark - Option

/**
 * @fn Option *Option::initWithTitle(Option *self, const char *title, ident value)
 * @memberof Option
 */
static Option *initWithTitle(Option *self, const char *title, ident value) {

	self = (Option *) super(View, self, initWithFrame, NULL);
	if (self) {

		Font *font = $$(Font, defaultFont, FontCategoryPrimaryControl);

		self->title = $(alloc(Text), initWithText, title, font);
		assert(self->title);

		self->value = value;

		self->title->view.alignment = ViewAlignmentMiddleLeft;
		$((View *) self, addSubview, (View *) self->title);
	}

	return self;
}

/**
 * @fn void Option::setSelected(Option *self, _Bool isSelected)
 * @memberof Option
 */
static void setSelected(Option *self, _Bool isSelected) {

	if (self->isSelected != isSelected) {
		self->isSelected = isSelected;

		self->view.needsLayout = true;
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->def->interface)->description = description;

	((ViewInterface *) clazz->def->interface)->acceptsFirstResponder = acceptsFirstResponder;
	((ViewInterface *) clazz->def->interface)->matchesSelector = matchesSelector;

	((OptionInterface *) clazz->def->interface)->initWithTitle = initWithTitle;
	((OptionInterface *) clazz->def->interface)->setSelected = setSelected;

}

/**
 * @fn Class *Option::_Option(void)
 * @memberof Option
 */
Class *_Option(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Option";
		clazz.superclass = _View();
		clazz.instanceSize = sizeof(Option);
		clazz.interfaceOffset = offsetof(Option, interface);
		clazz.interfaceSize = sizeof(OptionInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class

