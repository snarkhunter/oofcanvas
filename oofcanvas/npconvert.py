# -*- python -*-

# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modified
# versions of this software, you first contact the authors at
# oof_manager@nist.gov.

# This file is included in the python script generated by swig from
# oofcanvas.swg.

import numpy
import skimage
import sys

# Cairo stores image data in pixman format.  We need to convert the
# numpy RGBA data to Cairo::FORMAT_ARGB32.  Cairo::FORMAT_ARGB32
# corresponds to PIXMAN_a8r8g8b8, according to
# _cairo_format_from_pixman_format() in cairo-image-surface.c in the
# Cairo source.
#
# From https://afrantzis.com/pixel-format-guide/pixman.html: The pixel
# is represented by a 32-bit value, with A in bits 24-31, R in bits
# 16-23, G in bits 8-15 and B in bits 0-7.  On little-endian systems
# the pixel is stored in memory as the bytes B, G, R, A (B at the
# lowest address, A at the highest).  On big-endian systems the pixel
# is stored in memory as the bytes A, R, G, B (A at the lowest
# address, B at the highest).

def npconvert(image, flipy):
    try:
        image = image.copy(order='C') # don't modify the caller's data
        if flipy:
            image = numpy.flip(image, axis=0)

        # Convert to unsigned 8-bit
        image = skimage.util.img_as_ubyte(image)

        # Convert to RGB if gray
        if image.ndim == 2:
            image = skimage.color.gray2rgba(image)

        # Add an alpha channel if necessary
        if image.shape[2] == 3:
            alpha = numpy.ndarray((image.shape[0], image.shape[1], 1),
                                  numpy.uint8)
            alpha.fill(255)
            image = numpy.concatenate((image, alpha), axis=2)
            
        # Reorder the channels for Cairo compatibility.
        if sys.byteorder == 'little':
            image = image[:, :, (2, 1, 0, 3)]
        else:
            image = image[:, :, (3, 0, 1, 2)]
            
        # Return a copy of the image to ensure that the above
        # operations are actually carried out on the data, instead of
        # just creating new views of the data.  This also guarantees
        # C-contiguity of the data, which we require.
        return image.copy()
    except Exception as exc:
        print("Exception in npconvert:", exc)
        raise
        