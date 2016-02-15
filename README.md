ObjectivelyMVC
===
Object Oriented MVC Framework for OpenGL and SDL2 in c
---

ObjectivelyMVC is a user interface and interaction framework for [SDL2](http://www.libsdl.org) inspired by 
Apple's [UIKit](https://developer.apple.com/library/ios/documentation/UIKit/Reference/UIKit_Framework/). 
It is geared towards building high-quality, modern looking user interfaces within [OpenGL](http://www.opengl.org/) 
video games that are already using SDL2. It is written in [GNU C](http://www.gnu.org/software/gnu-c-manual/), and requires `gcc` or `clang`.

Features
---
 * Object-oriented Model-View-Controller implementation directly inspired by Apple's [UIKit](https://developer.apple.com/library/ios/documentation/UIKit/Reference/UIKit_Framework/)
    ObjectivelyMVC is built on [Objectively](https://github.com/jdolan/Objectively), an ultra-lightweight Object-oriented framework for [GNU C](http://www.gnu.org/software/gnu-c-manual/). ObjectivelyMVC delivers the elegance of OO / MVC without imposing C++ on your project. If you're' using C++ or Objective-C, ObjectivelyMVC is perfectly happy alongside those, too.
    
        ViewController *myViewController = $((ViewController *) alloc(MyViewController), initRootViewController, window);

 * Easily embeddable in any SDL2 / OpenGL application
    ObjectivelyMVC is purpose-built for video games. Unlike Gtk+, Qt, wxWidgets, FLTK, ..ObjectivelyMVC *does not hijack the main loop*. ObjectivelyMVC does not create a window, manage an OpenGL context, or originate events. Your game engine already does that, because it must. Like your mother, ObjectivelyMVC only asks that you give it a call once in a while. That's it.

        $(myViewController, respondToEvent, &event);
        ...
        $(myViewController, drawView, renderer);
    

Dependencies
---
 * fontconfig
 * SDL2
 * SDL2_image
 * SDL2_ttf
 * [Objectively](https://github.com/jdolan/Objectively)

