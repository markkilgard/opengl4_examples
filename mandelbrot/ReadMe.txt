
Interactive mandelbrot set explorer
  by Jason Allen
  enhanced by Mark Kilgard

This demo explores the Mandelbrot and Julian sets.

Demo cheat sheet:

  'q' - quit

  Esc - quit

  'r' - reset settings

  left-mouse-drag - zoom/rotate around the initially clicked point,
                    dragging down zooms in,
                    dragging up zooms out,
                    dragging right rotates clockwise,
                    dragging left rotates counterclockwise.

  Ctrl-left-mouse-drag - up/down zooms without rotating

  Shift-left-mouse-drag - left/right rotates without zooming

  middle-mouse-drag - pans or translates

  Shift-middle-mouse-drag - modifies the (x,y) offsets for the Julian set

  'j' - toggles showing the Mandelbrot and Julian sets, initially Mandelbrot

  'm' - toggles use per-sample shading versus multisampling

  'a' - increase maximum iterations by 10

  'z' - decrease maximum iterations by 10

  'p' - cycle through precision modes, initially single precision:
        1) single precsion
        2) double precision
        3) vertical split screen
        4) horizontal split screen
        5) 5x5 checkboard split screen

  '1' - single precision mode
  '2' - double precision mode
  '3' - vertical split screen
  '4' - horizontal split screen
  '4' - 5x5 checkboard split screen

  's' - toggle frames/second display, initially on

Issue: If you resize the window while zoomed in significantly, you'll find
you lose track of where you are in the fractal.  I don't quite understand
this effect but it seems to do with the limitations of single-precision
texture coordinates.  Resize the window larger before you zoooom in a lot.

- Mark Kilgard
  September 14, 2010
