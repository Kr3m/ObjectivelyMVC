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

#include <ObjectivelyMVC/Panel.h>

#include <resize.png.h>

static Image *_resize;

#define _Class _Panel

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Panel *this = (Panel *) self;

	release(this->accessoryView);
	release(this->contentView);
	release(this->resizeHandle);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	Panel *this = (Panel *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("accessoryView", InletTypeView, &this->accessoryView, NULL),
		MakeInlet("contentView", InletTypeView, &this->contentView, NULL),
		MakeInlet("isDraggable", InletTypeBool, &this->isDraggable, NULL),
		MakeInlet("isResiable", InletTypeBool, &this->isResizable, NULL),
	    MakeInlet("stackView", InletTypeView, &this->stackView, NULL)
	);

	$(self, bind, inlets, dictionary);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((Panel *) self, initWithFrame, NULL);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

	const Panel *this = (Panel *) self;

	const SDL_Size size = $(this, contentSize);

	$((View *) this->contentView, resize, &size);

	super(View, self, layoutSubviews);

	View *resizeHandle = (View *) this->resizeHandle;

	resizeHandle->frame.x = self->frame.w - resizeHandle->frame.w;
	resizeHandle->frame.y = self->frame.h - resizeHandle->frame.h;

	resizeHandle->hidden = !this->isResizable;
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static _Bool captureEvent(Control *self, const SDL_Event *event) {

	Panel *this = (Panel *) self;

	if (event->type == SDL_MOUSEMOTION && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK)) {

		if ((self->state & ControlStateHighlighted) == 0) {
			if ($((View *) this->resizeHandle, didReceiveEvent, event)) {
				self->state |= ControlStateHighlighted;
				this->isResizing = true;
			} else if (this->isDraggable) {
				self->state |= ControlStateHighlighted;
				this->isDragging = true;
			}
		}

		if (this->isResizing) {
			SDL_Size size = $((View *) self, size);

			size.w = clamp(size.w + event->motion.xrel, this->minSize.w, this->maxSize.w);
			size.h = clamp(size.h + event->motion.yrel, this->minSize.h, this->maxSize.h);

			$((View *) self, resize, &size);
		} else if (this->isDragging) {
			self->view.frame.x += event->motion.xrel;
			self->view.frame.y += event->motion.yrel;
		}

		return true;
	}

	if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
		self->state &= ~ControlStateHighlighted;
		this->isResizing = this->isDragging = false;
		return true;
	}

	return super(Control, self, captureEvent, event);
}

#pragma mark - Panel

/**
 * @fn SDL_Size Panel::contentSize(const Panel *self)
 * @memberof Panel
 */
static SDL_Size contentSize(const Panel *self) {

	const View *accessoryView = (View *) self->accessoryView;
	const View *contentView = (View *) self->contentView;

	SDL_Size size = $(contentView, sizeThatContains);

	if (accessoryView->hidden == false) {
		const SDL_Size accessorySize = $(accessoryView, sizeThatContains);
		size.h -= accessorySize.h + self->stackView->spacing;
	}

	return size;
}

/**
 * @fn Panel *Panel::initWithFrame(Panel *self, const SDL_Rect *frame)
 * @memberof Panel
 */
static Panel *initWithFrame(Panel *self, const SDL_Rect *frame) {

	self = (Panel *) super(Control, self, initWithFrame, frame);
	if (self) {
		View *this = (View *) self;

		self->isDraggable = true;
		self->isResizable = true;

		self->maxSize = MakeSize(INT32_MAX, INT32_MAX);

		self->stackView = $(alloc(StackView), initWithFrame, NULL);
		assert(self->stackView);

		$(this, addSubview, (View *) self->stackView);

		self->contentView = $(alloc(StackView), initWithFrame, NULL);
		assert(self->contentView);

		$((View *) self->contentView, addClassName, "contentView");
		$((View *) self->contentView, addClassName, "container");

		self->contentView->view.autoresizingMask |= ViewAutoresizingWidth;

		$((View *) self->stackView, addSubview, (View *) self->contentView);

		self->accessoryView = $(alloc(StackView), initWithFrame, NULL);
		assert(self->accessoryView);

		$((View *) self->accessoryView, addClassName, "accessoryView");
		$((View *) self->accessoryView, addClassName, "container");

		self->accessoryView->axis = StackViewAxisHorizontal;
		self->accessoryView->spacing = DEFAULT_PANEL_SPACING;
		self->accessoryView->view.alignment = ViewAlignmentMiddleRight;

		self->accessoryView->view.hidden = true;

		$((View *) self->stackView, addSubview, (View *) self->accessoryView);

		self->resizeHandle = $(alloc(ImageView), initWithImage, _resize);
		assert(self->resizeHandle);

		self->resizeHandle->view.alignment = ViewAlignmentInternal;

		self->resizeHandle->view.frame.w = DEFAULT_PANEL_RESIZE_HANDLE_SIZE;
		self->resizeHandle->view.frame.h = DEFAULT_PANEL_RESIZE_HANDLE_SIZE;

		$((View *) self, addSubview, (View *) self->resizeHandle);

		this->autoresizingMask = ViewAutoresizingContain;
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->def->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->def->interface)->init = init;
	((ViewInterface *) clazz->def->interface)->layoutSubviews = layoutSubviews;

	((ControlInterface *) clazz->def->interface)->captureEvent = captureEvent;

	((PanelInterface *) clazz->def->interface)->contentSize = contentSize;
	((PanelInterface *) clazz->def->interface)->initWithFrame = initWithFrame;

	_resize = $(alloc(Image), initWithBytes, resize_png, resize_png_len);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {
	release(_resize);
}

/**
 * @fn Class *Panel::_Panel(void)
 * @memberof Panel
 */
Class *_Panel(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Panel";
		clazz.superclass = _Control();
		clazz.instanceSize = sizeof(Panel);
		clazz.interfaceOffset = offsetof(Panel, interface);
		clazz.interfaceSize = sizeof(PanelInterface);
		clazz.initialize = initialize;
		clazz.destroy = destroy;
	});

	return &clazz;
}

#undef _Class

