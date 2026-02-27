// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "oofcanvas/utility_extra.h"
#include <cassert>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
  
namespace OOFCanvas {

  CanvasCoord CanvasCoord::operator*(double x) const {
    CanvasCoord result(*this);
    result *= x;
    return result;
  }

  CanvasCoord CanvasCoord::operator/(double x) const {
    CanvasCoord result(*this);
    result /= x;
    return result;
  }

  CanvasCoord CanvasCoord::operator+(const CanvasCoord &other) const {
    CanvasCoord result(*this);
    result += other;
    return result;
  }

  CanvasCoord CanvasCoord::operator-(const CanvasCoord &other) const {
    CanvasCoord result(*this);
    result -= other;
    return result;
  }

  CanvasCoord transform(const CanvasCoord &pt, const Cairo::Matrix &mat) {
    CanvasCoord result(pt);
    mat.transform_point(result.x, result.y);
    return result;
  }

  CanvasCoord user_to_device(const CanvasCoord &pt, Cairo::RefPtr<Cairo::Context> ctxt) {
    CanvasCoord result(pt);
    ctxt->user_to_device(result.x, result.y);
    return result;
  }

  CanvasCoord device_to_user(const CanvasCoord &pt, Cairo::RefPtr<Cairo::Context> ctxt) {
    CanvasCoord result(pt);
    ctxt->device_to_user(result.x, result.y);
    return result;
  }
  
  double cross(const CanvasCoord &a, const CanvasCoord &b) {
    return a.x*b.y - a.y*b.x;
  }

  bool CanvasCoord::operator==(const CanvasCoord &other) const {
    return x == other.x && y == other.y;
  }

  bool CanvasCoord::operator!=(const CanvasCoord &other) const {
    return x != other.x || y != other.y;
  }

  std::ostream &operator<<(std::ostream &os, const CanvasCoord &p) {
    return os << "(" << p.x << ", " << p.y << ")";
  }
  
  std::ostream &operator<<(std::ostream &os, const ICanvasCoord &p) {
    return os << "(" << p.x << ", " << p.y << ")";
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  CanvasCoord ICanvasCoord::operator*(double a) const {
    return CanvasCoord(x*a, y*a);
  }

  ICanvasCoord ICanvasCoord::operator*(int a) const {
    return ICanvasCoord(x*a, y*a);
  }
  
  CanvasCoord ICanvasCoord::operator/(double a) const {
    return CanvasCoord(x/a, y/a);
  }

  ICanvasCoord ICanvasCoord::operator+(const ICanvasCoord &other) const {
    ICanvasCoord result(*this);
    result += other;
    return result;
  }

  ICanvasCoord ICanvasCoord::operator-(const ICanvasCoord &other) const {
    ICanvasCoord result(*this);
    result -= other;
    return result;
  }

  bool ICanvasCoord::operator==(const ICanvasCoord &other) const {
    return x == other.x && y == other.y;
  }

  bool ICanvasCoord::operator!=(const ICanvasCoord &other) const {
    return x != other.x || y != other.y;
  }

  CanvasCoord operator+(const CanvasCoord &a, const ICanvasCoord &b) {
    return CanvasCoord(a.x+b.x, a.y+b.y);
  }

  CanvasCoord operator+(const ICanvasCoord &a, const CanvasCoord &b) {
    return CanvasCoord(a.x+b.x, a.y+b.y);
  }

  CanvasCoord operator-(const CanvasCoord &a, const ICanvasCoord &b) {
    return CanvasCoord(a.x-b.x, a.y-b.y);
  }

  CanvasCoord operator-(const ICanvasCoord &a, const CanvasCoord &b) {
    return CanvasCoord(a.x-b.x, a.y-b.y);
  }
  
  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//
  
  Rectangle::Rectangle()
    : initialized_(false)
  {}

  Rectangle::Rectangle(double x0, double y0, double x1, double y1) {
    setup(x0, y0, x1, y1);
  }
  
  Rectangle::Rectangle(const CanvasCoord &a, const CanvasCoord &b) {
    setup(a.x, a.y, b.x, b.y);
  }

  Rectangle::Rectangle(const CanvasCoord *a, const CanvasCoord *b) {
    setup(a->x, a->y, b->x, b->y);
  }

  Rectangle::Rectangle(const Rectangle &other)
    : pmin(other.pmin),
      pmax(other.pmax),
      initialized_(other.initialized_)
  {}

  void  Rectangle::setup(double x0, double y0, double x1, double y1) {
    if(x0 < x1) {
      pmin.x = x0;
      pmax.x = x1;
    }
    else {
      pmin.x = x1;
      pmax.x = x0;
    }
    if(y0 < y1) {
      pmin.y = y0;
      pmax.y = y1;
    }
    else {
      pmin.y = y1;
      pmax.y = y0;
    }
    initialized_ = true;
  }

  void Rectangle::swallow(const CanvasCoord &p) {
    if(initialized_) {
      if(p.x < pmin.x)
	pmin.x = p.x;
      else if(p.x > pmax.x)
	pmax.x = p.x;
      if(p.y < pmin.y)
	pmin.y = p.y;
      else if(p.y > pmax.y)
	pmax.y = p.y;
    }
    else {
      pmin = p;
      pmax = p;
      initialized_ = true;
    }
  }

  void Rectangle::swallow(const Rectangle &rect) {
    if(rect.initialized()) {
      swallow(rect.pmin);
      swallow(rect.pmax);
    }
  }

  void Rectangle::expand(double delta) {
    // Grow by delta in each direction
    if(initialized_) {
      pmin.x -= delta;
      pmin.y -= delta;
      pmax.x += delta;
      pmax.y += delta;
    }
  }

  void Rectangle::shift(const CanvasCoord &delta) {
    pmin += delta;
    pmax += delta;
  }

  void Rectangle::scale(double xfactor, double yfactor) {
    pmin.x *= xfactor;
    pmax.x *= xfactor;
    pmin.y *= yfactor;
    pmax.y *= yfactor;
  }

  CanvasCoord Rectangle::center() const {
    return 0.5*(pmin + pmax);
  }

  Rectangle user_to_device(const Rectangle &rect,
			   Cairo::RefPtr<Cairo::Context> ctxt)
  {
    return Rectangle(user_to_device(rect.lowerLeft(), ctxt),
		     user_to_device(rect.upperRight(), ctxt));
  }
  
  Rectangle device_to_user(const Rectangle &rect,
			   Cairo::RefPtr<Cairo::Context> ctxt)
  {
    return Rectangle(device_to_user(rect.lowerLeft(), ctxt),
		     device_to_user(rect.upperRight(), ctxt));
  }
  
  const Rectangle &Rectangle::operator=(const Rectangle &other) {
    initialized_ = other.initialized_;
    pmin = other.pmin;
    pmax = other.pmax;
    return *this;
  }

  bool Rectangle::contains(const CanvasCoord &pt) const {
    return initialized_ && (pt.x >= pmin.x && pt.x <= pmax.x &&
			    pt.y >= pmin.y && pt.y <= pmax.y);
  }

  bool Rectangle::operator==(const Rectangle &other) const {
    assert(initialized_ && other.initialized_);
    return pmin == other.pmin && pmax == other.pmax;
  }

  bool Rectangle::operator!=(const Rectangle &other) const {
    assert(initialized_ && other.initialized_);
    return pmin != other.pmin || pmax != other.pmax;
  }
   

  std::ostream &operator<<(std::ostream &os, const Rectangle &rect) {
    os << "Rectangle(";
    if(rect.initialized()) 
      os << rect.pmin << ", " << rect.pmax << ")";
    else
      os << "<uninitialized>";
    os << ")";
    return os;
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // Given a point, compute the normal distance squared from it to the
  // segment and the position 0<alpha<1 along the segment of the
  // normal to the point.  That is, the normal from the point to the
  // segment intersects the segment at
  // (1-alpha)*seg.p0 + alpha*seg.p1.

  void Segment::projection(const CanvasCoord &pt, double &alpha, double &distance2)
    const
  {
    CanvasCoord pp = p1 - p0;
    alpha = ((pt - p0) * pp)/pp.norm2();
    distance2 = ((p0 + alpha*pp) - pt).norm2();
  }

  double Segment::angle() const {
    CanvasCoord delta = p1 - p0;
    return atan2(delta.y, delta.x);
  }

  CanvasCoord Segment::interpolate(double alpha) const {
    return p0 + alpha*(p1 - p0);
  }

  std::ostream &operator<<(std::ostream &os, const Segment &seg) {
    os << "[" << seg.p0 << ", " << seg.p1 << "]";
    return os;
  }

   //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // void CanvasColor::set(Cairo::RefPtr<Cairo::Context> ctxt) const {
  //   if(alpha == 1.0)
  //     ctxt->set_source_rgb(red, green, blue);
  //   else
  //     ctxt->set_source_rgba(red, green, blue, alpha);
  // }

  void setColor(const CanvasColor &color, Cairo::RefPtr<Cairo::Context> ctxt) {
    if(color.alpha == 1.0)
      ctxt->set_source_rgb(color.red, color.green, color.blue);
    else
      ctxt->set_source_rgba(color.red, color.green, color.blue, color.alpha);
  }

  CanvasColor CanvasColor::opacity(double newalpha) const {
    return CanvasColor(red, green, blue, newalpha);
  }

  std::ostream &operator<<(std::ostream &os, const CanvasColor &color) {
    os << "OOFCanvas::CanvasColor(" << color.red << ", " << color.green
       << ", " << color.blue << ")";
    return os;
  }

  const CanvasColor black(0.0, 0.0, 0.0);
  const CanvasColor gray(0.5, 0.5, 0.5);
  const CanvasColor white(1.0, 1.0, 1.0);
  const CanvasColor red(1.0, 0.0, 0.0);
  const CanvasColor green(0.0, 1.0, 0.0);
  const CanvasColor blue(0.0, 0.0, 1.0);
  const CanvasColor yellow(1.0, 1.0, 0.0);
  const CanvasColor cyan(0.0, 1.0, 1.0);
  const CanvasColor magenta(1.0, 0.0, 1.0);

  std::ostream &operator<<(std::ostream &os, const Cairo::Matrix &trans) {
    os << "["
       << trans.xx << ", " << trans.xy << ", "
       << trans.yx << ", " << trans.yy << "; "
       << trans.x0 << ", " << trans.y0 << "]";
    return os;
  }

  bool operator==(const Cairo::Matrix &a, const Cairo::Matrix &b) {
    return (a.xx == b.xx && a.xy == b.xy && a.yx == b.yx && a.yy == b.yy &&
	    a.x0 == b.x0 && a.y0 == b.y0);
  }

  std::ostream &operator<<(std::ostream &os, const std::vector<double> &vec) {
    os << "[";
    bool first = true;
    for(double x : vec) {
      if(!first) os << ", ";
      os << x;
      first = false;
    }
    os << "]";
    return os;
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  Lock::Lock()
    : enabled(false)
  {
    pthread_mutex_init(&lock, NULL);
  }

  Lock::~Lock() {
    pthread_mutex_destroy(&lock);
  }


  void Lock::acquire() {
    if(enabled)
      pthread_mutex_lock(&lock);
  }

  void Lock::release() {
    if(enabled)
      pthread_mutex_unlock(&lock);
  }

  KeyHolder::KeyHolder(Lock &some_lock, const std::string &file, int line)
    : lock(&some_lock),
      file(file), line(line)
  {
    // std::cerr << "KeyHolder acquire: "
    // 	      << file << " (" << line << ")" << std::endl;
    lock->acquire();
  }

  KeyHolder::~KeyHolder() {
    // std::cerr << "KeyHolder release: "
    // 	      << file << " (" << line << ")" << std::endl;
    lock->release();
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // For debugging threading issues. Check to see if functions are
  // being called on the same thread (presumably the main thread being
  // used for gui operations in the calling program.)

  static pthread_t mainthread = 0;

  void set_mainthread() {
    mainthread = pthread_self();
  }

  bool check_mainthread() {
    assert(mainthread != 0);
    return pthread_equal(mainthread, pthread_self());
  }

  void require_mainthread(const char *file, int line) {
#ifdef DEBUG
    if(!check_mainthread()) {
      std::cerr << "OOFCanvas::require_mainthread: Not on main thread! "
		<< file << ", line " << line
		<< ". Aborting." << std::endl;
      abort();
      // throw "Not on main thread!";
    }
#endif	// DEBUG
  }

};				// namespace OOFCanvas
		   

