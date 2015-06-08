
This example demonstrates programmable tessellation introduced by
OpenGL 4.

The demo requires a Fermi-based NVIDIA GPU to run.  Be sure to download
the latest NVIDIA OpenGL 4.1 (or later) driver from:

  http://developer.nvidia.com/object/opengl_driver.html

Demo cheat sheet:

  'p' -- cycle scenes:  (elephant->teapot->PN triangle monkey
  head->faceted monkey head->bicubic cube->bezier triangle->single
  bicubic patch->linear triangle->linear quadrilateral)

  'n' -- turn off any overlaid mesh and control points (normal rendeirng
  mode)

  'm' -- cycle through shading (diffuse->phong->UV visualization)

  'c' -- cycle visualization of control points

  'w' -- cycle visualize of tessellated mesh

  Arrow keys -- moves yellow light source in Phong mode

  'x' -- toggle adaptive (initial) vs. explicit tessellation

  'f' -- toggle frame rate counter

  'v' -- toggle vertical retrace synchronization

  '+' -- zoom in
  '-' -- zoom out

  Right mouse -- spin scene
  Ctrl+Right mouse -- zoom in/out scene
  Middle mouse -- drag control points
  Left mouse -- pop up menu

The demo includes an implementation of adaptive PN triangles (the monkey
head scene). 

The demo also includes shaders for bicubic quadrilateral patches
(teapot and elephant scenes), Bezier triangles, and linear triangle and
quadrilateral patches.

- Mark Kilgard
  mjk@nvidia.com
  August 3, 2010
