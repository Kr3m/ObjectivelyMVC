[![Build Status](http://ci.quetoo.org/buildStatus/icon?job=ObjectivelyMVC-Linux-x86_64)](http://ci.quetoo.org/job/ObjectivelyMVC-Linux-x86_64/)
[![Zlib License](https://img.shields.io/badge/license-Zlib-brightgreen.svg)](https://opensource.org/licenses/Zlib)
![This software is PRE-ALPHA](https://img.shields.io/badge/development_stage-PRE_ALPHA-orange.svg)

ObjectivelyMVC
===

Object oriented MVC framework for OpenGL, SDL2 and GNU C
---

ObjectivelyMVC is a user interface and interaction framework for [SDL2](http://www.libsdl.org) inspired by 
Apple's [AppKit](https://developer.apple.com/reference/appkit). 
It is geared towards building high-quality, modern looking user interfaces within [OpenGL](http://www.opengl.org/) 
video games that are already using SDL2. It is written in [GNU C](http://www.gnu.org/software/gnu-c-manual/), and requires `gcc` or `clang`.

Features
---

### Object oriented Model-View-Controller implementation in C

ObjectivelyMVC is built on [Objectively](https://github.com/jdolan/Objectively), an ultra-lightweight object oriented framework for [GNU C](http://www.gnu.org/software/gnu-c-manual/). ObjectivelyMVC delivers the elegance of OO / MVC without imposing C++ on your project. If you *are* using C++ or Objective-C, ObjectivelyMVC is perfectly happy alongside those, too.
    
```c
   ViewController *vc = $((ViewController *) alloc(MyViewController), initRootViewController, window);
```

### Easily embeddable in any SDL2 / OpenGL application

ObjectivelyMVC is purpose-built for video games. Unlike Gtk+, Qt, wxWidgets, FLTK, ..ObjectivelyMVC **does not hijack the main loop**. ObjectivelyMVC does not create a window, manage an OpenGL context, or originate events. Your game already does that, because it has to. Like your mother, ObjectivelyMVC only asks that you give it a call once in a while. That's it.

```c
    $(vc, respondToEvent, &event);
    ...
    $(vc, drawView, renderer);
```

### Beautiful, discoverable TrueType fonts

ObjectivelyMVC uses [Fontconfig](https://www.freedesktop.org/wiki/Software/fontconfig/) and [SDL_ttf](https://www.libsdl.org/projects/SDL_ttf/) to discover and render the TrueType fonts that are available on your system. It also automatically detects High-DPI (Retina, 4K) displays, and scales fonts accordingly. The result is crisp, beautiful vector-based fonts that look native, because they are.

```c
Array *fonts = $$(Font, allFonts);
...
release(fonts);
...
Font *verdana = $(alloc(Font), initWithAttributes, "Verdana", 24, 0); // will render at 48pt on Retina displays
```

### Full suite of Views and Controls

ObjectivelyMVC provides a robust set of containers, views and form elements. Stack and arrange components with `Box`, `Panel` and `StackView`. Add `Buttons`, `Checkboxes`, `Selects`, `Sliders`, editable `TextViews` and more by simply instantiating them. Bind `Actions` to `SDL_Event` types on each element, or use the specialized _delegate_ callbacks for convenience.

```c
$((Control *) button, addActionForEventType, SDL_MOUSEBUTTONUP, my_callback, my_data);
```

```c
Select *select = $(alloc(Select), initWithFrame, NULL, ControlStyleDefault);
...
select->delegate.didSelectOption = my_callback;
```

Examples
---

### HelloViewController
An example application that creates a Window, enters its main loop and draws a scene before rendering a simple menu:

![Screenshot](Documentation/demo.gif)

 * [Hello.c](Examples/Hello.c) - The application source code
 * [HelloViewController.h](Examples/HelloViewController.h) - The `HelloViewController` header.
 * [HelloViewController.c](Examples/HelloViewController.c) - The `HelloViewController` source code. 

Dependencies
---
 * [Objectively](https://github.com/jdolan/Objectively)
 * [Fontconfig](https://www.freedesktop.org/wiki/Software/fontconfig/)
 * [SDL2](https://www.libsdl.org/)
 * [SDL2_image](https://www.libsdl.org/projects/SDL_image/)
 * [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)

