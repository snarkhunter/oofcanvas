# OOFCanvas summary

OOFCanvas is a replacement for libgnomecanvas, designed for use in
OOF2, but hopefully useful elsewhere. OOFCanvas is based on
[Cairo](https://www.cairographics.org/) and is compatible with gtk3.

The canvas is a drawing area that can display a variety of shapes,
including text. It can be scrolled, zoomed, and printed.  It supports
mouse selection by providing a list of items at a position.

OOF2 used [libgnomecanvas](https://gitlab.gnome.org/Archive/libgnomecanvas)
to display and interact with images and meshes.  But libgnomecanvas
requires gtk+2, and gtk+2 works only with Python3, not Python2, and
Python2 is obsolete.  In order to upgrade OOF2 to Python3, we
need to first upgrade it from gtk+2 to gtk+3, and to do that we need
to first replace libgnomecanvas.

OOFCanvas is *not* a drop-in replacement for libgnomecanvas.  It's
also not a full-fledged gtk widget.  It's a set of classes that does
some of what libgnomecanvas did and uses gtk.

All of the code is in C++.  Wrappers for Python 2.7 and 3.x can be
generated using [Swig](https://www.swig.org).

For all the details, including prerequisites and installation
instructions, see the [Manual](manual.html).

### Disclaimer and Copyright

NIST-developed software is provided by NIST as a public service. You
may use, copy and distribute copies of the software in any medium,
provided that you keep intact this entire notice. You may improve,
modify and create derivative works of the software or any portion of
the software, and you may copy and distribute such modifications or
works. Modified works should carry a notice stating that you changed
the software and should note the date and nature of any such
change. Please explicitly acknowledge the National Institute of
Standards and Technology as the source of the software. To facilitate
maintenance we ask that before distributing modified versions of this
software, you first contact the authors at oof_manager@nist.gov.

NIST-developed software is expressly provided "AS IS." NIST MAKES NO
WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT
AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE
OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT
ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY
REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS
THEREOF, INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY,
RELIABILITY, OR USEFULNESS OF THE SOFTWARE.

You are solely responsible for determining the appropriateness of
using and distributing the software and you assume all risks
associated with its use, including but not limited to the risks and
costs of program errors, compliance with applicable laws, damage to or
loss of data, programs or equipment, and the unavailability or
interruption of operation. This software is not intended to be used in
any situation where a failure could cause risk of injury or damage to
property. The software developed by NIST employees is not subject to
copyright protection within the United States.



