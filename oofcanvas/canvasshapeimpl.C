// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

// This file is #include'd into canvasshapeimpl.h and includes the
// definitions of the templated functions declared there.

namespace OOFCanvas {

  // translateLineCap and translateLineJoin convert OOFCanvas
  // definitions of the Cairo enums back into the Cairo form.  Is
  // there a cleaner way to do this?  The translation is dumb because
  // it probaby doesn't do anything on the bit level, but I don't know
  // how to avoid it other than by just using the Cairo enums, which
  // would force OOFCanvas users to explicitly use Cairo in their
  // code.

  inline Cairo::LineCap translateLineCap(LineCap x) {
    return Cairo::LineCap(static_cast<int>(x));
  }

  inline Cairo::LineJoin translateLineJoin(LineJoin x) {
    return Cairo::LineJoin(static_cast<int>(x));
  }

  template <class CANVASITEM>
  double CanvasShapeImplementation<CANVASITEM>::lineWidthInUserUnits(
				     Cairo::RefPtr<Cairo::Context> ctxt)
    const
  {
    if(this->canvasitem->getLineWidthInPixels()) {
      double dx=1, dy=1;
      ctxt->device_to_user_distance(dx, dy);
      return this->canvasitem->getLineWidth()*dx;
    }
    return this->canvasitem->getLineWidth();
  }

  template <class CANVASITEM>
  double CanvasShapeImplementation<CANVASITEM>::lineWidthInUserUnits(
					     const OSCanvasImpl *canvas)
    const
  {
    if(this->canvasitem->getLineWidthInPixels()) {
      return canvas->pixel2user(this->canvasitem->getLineWidth());
    }
    return this->canvasitem->getLineWidth();
  }

  template <class CANVASITEM>
  std::vector<double>
  CanvasShapeImplementation<CANVASITEM>::dashLengthInUserUnits(
				 Cairo::RefPtr<Cairo::Context> ctxt)
    const
  {
    if(!this->canvasitem->getDashLengthInPixels())
      return this->canvasitem->getDash();
    std::vector<double> newdash(this->canvasitem->getDash());
    double dummy=0;
    for(unsigned int i=0; i<newdash.size(); i++)
      ctxt->device_to_user_distance(newdash[i], dummy);
    return newdash;
  }

  template <class CANVASITEM>
  void CanvasShapeImplementation<CANVASITEM>::pixelExtents(
						   double &left, double &right,
						   double &up, double &down)
    const
  {
    // Doing this right would involve taking the angles of the
    // segments into account and is probably not worth the trouble.
    double halfw = this->canvasitem->getLineWidthInPixels() ?
      0.5*this->canvasitem->getLineWidth() : 0.0;
    left = halfw;
    right = halfw;
    up = halfw;
    down = halfw;
  }

  template <class CANVASITEM>
  void CanvasShapeImplementation<CANVASITEM>::stroke(
				     Cairo::RefPtr<Cairo::Context> ctxt)
    const
  {
    ctxt->set_line_width(lineWidthInUserUnits(ctxt));
    ctxt->set_line_cap(translateLineCap(this->canvasitem->getLineCap()));
    ctxt->set_line_join(translateLineJoin(this->canvasitem->getLineJoin()));
    if(this->canvasitem->getDash().empty()) {
      // No dashes
      setColor(this->canvasitem->getLineColor(), ctxt);
      ctxt->stroke();
    }
    else if(!this->canvasitem->getDashColorSet()) {
      // line is dashed with gaps between dashes.
      setColor(this->canvasitem->getLineColor(), ctxt);
      ctxt->set_dash(dashLengthInUserUnits(ctxt),
		     this->canvasitem->getDashOffset());
      ctxt->stroke();
    }
    else {
      // gaps between dashes are filled with the dashColor
      setColor(this->canvasitem->getDashColor(), ctxt);
      ctxt->stroke_preserve();
      setColor(this->canvasitem->getLineColor(), ctxt);
      ctxt->set_dash(dashLengthInUserUnits(ctxt),
		     this->canvasitem->getDashOffset());
      ctxt->stroke();
    }
  }

  template <class CANVASITEM>
  void CanvasFillableShapeImplementation<CANVASITEM>::fillAndStroke(
				    Cairo::RefPtr<Cairo::Context> ctxt)
    const
  {
    if(this->canvasitem->lined() && this->canvasitem->filled()) {
      setColor(this->canvasitem->getFillColor(), ctxt);
      ctxt->fill_preserve();
      this->stroke(ctxt);
    }
    else if(this->canvasitem->lined()) {
      this->stroke(ctxt);
    }
    else if(this->canvasitem->filled()) {
      setColor(this->canvasitem->getFillColor(), ctxt);
      ctxt->fill();
    }
  }

};				// namespace OOFCanvas
