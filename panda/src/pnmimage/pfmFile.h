// Filename: pfmFile.h
// Created by:  drose (23Dec10)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) Carnegie Mellon University.  All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#ifndef PFMFILE_H
#define PFMFILE_H

#include "pandabase.h"
#include "pnmImageHeader.h"
#include "luse.h"
#include "boundingHexahedron.h"
#include "vector_float.h"

class PNMImage;
class PNMReader;
class PNMWriter;

////////////////////////////////////////////////////////////////////
//       Class : PfmFile
// Description : Defines a pfm file, a 2-d table of floating-point
//               numbers, either 3-component or 1-component, or with a
//               special extension, 4-component.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDA_PNMIMAGE PfmFile : public PNMImageHeader {
PUBLISHED:
  PfmFile();
  PfmFile(const PfmFile &copy);
  void operator = (const PfmFile &copy);

  void clear();
  void clear(int x_size, int y_size, int num_channels);

  BLOCKING bool read(const Filename &fullpath);
  BLOCKING bool read(istream &in, const Filename &fullpath = Filename());
  BLOCKING bool read(PNMReader *reader);
  BLOCKING bool write(const Filename &fullpath);
  BLOCKING bool write(ostream &out, const Filename &fullpath = Filename());
  BLOCKING bool write(PNMWriter *writer);

  BLOCKING bool load(const PNMImage &pnmimage);
  BLOCKING bool store(PNMImage &pnmimage) const;
  BLOCKING bool store_mask(PNMImage &pnmimage) const;

  INLINE bool is_valid() const;

  INLINE PN_float32 get_scale() const;
  INLINE void set_scale(PN_float32 scale);

  INLINE bool has_point(int x, int y) const;
  INLINE PN_float32 get_component(int x, int y, int c) const;
  INLINE void set_component(int x, int y, int c, PN_float32 value);
  INLINE PN_float32 get_point1(int x, int y) const;
  INLINE void set_point1(int x, int y, PN_float32 point);
  INLINE const LPoint2f &get_point2(int x, int y) const;
  INLINE void set_point2(int x, int y, const LVecBase2f &point);
  INLINE void set_point2(int x, int y, const LVecBase2d &point);
  INLINE LPoint2f &modify_point2(int x, int y);
  INLINE const LPoint3f &get_point(int x, int y) const;
  INLINE void set_point(int x, int y, const LVecBase3f &point);
  INLINE void set_point(int x, int y, const LVecBase3d &point);
  INLINE LPoint3f &modify_point(int x, int y);
  INLINE const LPoint3f &get_point3(int x, int y) const;
  INLINE void set_point3(int x, int y, const LVecBase3f &point);
  INLINE void set_point3(int x, int y, const LVecBase3d &point);
  INLINE LPoint3f &modify_point3(int x, int y);
  INLINE const LPoint4f &get_point4(int x, int y) const;
  INLINE void set_point4(int x, int y, const LVecBase4f &point);
  INLINE void set_point4(int x, int y, const LVecBase4d &point);
  INLINE LPoint4f &modify_point4(int x, int y);

  INLINE void fill(PN_float32 value);
  INLINE void fill(const LPoint2f &value);
  INLINE void fill(const LPoint3f &value);
  void fill(const LPoint4f &value);

  BLOCKING bool calc_average_point(LPoint3f &result, PN_float32 x, PN_float32 y, PN_float32 radius) const;
  BLOCKING bool calc_bilinear_point(LPoint3f &result, PN_float32 x, PN_float32 y) const;
  BLOCKING bool calc_min_max(LVecBase3f &min_points, LVecBase3f &max_points) const;
  BLOCKING bool calc_autocrop(int &x_begin, int &x_end, int &y_begin, int &y_end) const;
  BLOCKING INLINE bool calc_autocrop(LVecBase4f &range) const;
  BLOCKING INLINE bool calc_autocrop(LVecBase4d &range) const;
 
  bool is_row_empty(int y, int x_begin, int x_end) const;
  bool is_column_empty(int x, int y_begin, int y_end) const;

  INLINE void set_zero_special(bool zero_special);
  INLINE void set_no_data_chan4(bool chan4);
  void set_no_data_value(const LPoint4f &no_data_value);
  INLINE void set_no_data_value(const LPoint4d &no_data_value);
  INLINE void clear_no_data_value();
  INLINE bool has_no_data_value() const;
  INLINE const LPoint4f &get_no_data_value() const;

  BLOCKING void resize(int new_x_size, int new_y_size);
  BLOCKING void box_filter_from(double radius, const PfmFile &copy);
  BLOCKING void gaussian_filter_from(double radius, const PfmFile &copy);
  BLOCKING void quick_filter_from(const PfmFile &copy);

  BLOCKING void reverse_rows();
  BLOCKING void flip(bool flip_x, bool flip_y, bool transpose);
  BLOCKING void xform(const LMatrix4f &transform);
  INLINE BLOCKING void xform(const LMatrix4d &transform);
  BLOCKING void forward_distort(const PfmFile &dist, PN_float32 scale_factor = 1.0);
  BLOCKING void reverse_distort(const PfmFile &dist, PN_float32 scale_factor = 1.0);
  BLOCKING void merge(const PfmFile &other);
  BLOCKING void copy_channel(int to_channel, const PfmFile &other, int from_channel);
  BLOCKING void apply_crop(int x_begin, int x_end, int y_begin, int y_end);
  BLOCKING void clear_to_texcoords(int x_size, int y_size);

  bool calc_tight_bounds(LPoint3f &min_point, LPoint3f &max_point) const;
  BLOCKING PT(BoundingHexahedron) compute_planar_bounds(const LPoint2f &center, PN_float32 point_dist, PN_float32 sample_radius, bool points_only) const;
  INLINE BLOCKING PT(BoundingHexahedron) compute_planar_bounds(const LPoint2d &center, PN_float32 point_dist, PN_float32 sample_radius, bool points_only) const;
  void compute_sample_point(LPoint3f &result,
                            PN_float32 x, PN_float32 y, PN_float32 sample_radius) const;

  void copy_sub_image(const PfmFile &copy, int xto, int yto,
                      int xfrom = 0, int yfrom = 0,
                      int x_size = -1, int y_size = -1);

  void output(ostream &out) const;

public:
  INLINE const vector_float &get_table() const;
  INLINE void swap_table(vector_float &table);

private:
  INLINE void setup_sub_image(const PfmFile &copy, int &xto, int &yto,
                              int &xfrom, int &yfrom, int &x_size, int &y_size,
                              int &xmin, int &ymin, int &xmax, int &ymax);

  void box_filter_region(PN_float32 &result,
                         PN_float32 x0, PN_float32 y0, PN_float32 x1, PN_float32 y1) const;
  void box_filter_region(LPoint3f &result,
                         PN_float32 x0, PN_float32 y0, PN_float32 x1, PN_float32 y1) const;
  void box_filter_region(LPoint4f &result,
                         PN_float32 x0, PN_float32 y0, PN_float32 x1, PN_float32 y1) const;
  void box_filter_line(PN_float32 &result, PN_float32 &coverage,
                       PN_float32 x0, int y, PN_float32 x1, PN_float32 y_contrib) const;
  void box_filter_line(LPoint3f &result, PN_float32 &coverage,
                       PN_float32 x0, int y, PN_float32 x1, PN_float32 y_contrib) const;
  void box_filter_line(LPoint4f &result, PN_float32 &coverage,
                       PN_float32 x0, int y, PN_float32 x1, PN_float32 y_contrib) const;
  void box_filter_point(PN_float32 &result, PN_float32 &coverage,
                        int x, int y, PN_float32 x_contrib, PN_float32 y_contrib) const;
  void box_filter_point(LPoint3f &result, PN_float32 &coverage,
                        int x, int y, PN_float32 x_contrib, PN_float32 y_contrib) const;
  void box_filter_point(LPoint4f &result, PN_float32 &coverage,
                        int x, int y, PN_float32 x_contrib, PN_float32 y_contrib) const;

  class MiniGridCell {
  public:
    MiniGridCell() : _sxi(-1), _syi(-1), _dist(-1) { }
    int _sxi, _syi;
    int _dist;
  };

  void fill_mini_grid(MiniGridCell *mini_grid, int x_size, int y_size, 
                      int xi, int yi, int dist, int sxi, int syi) const;

  static bool has_point_noop(const PfmFile *file, int x, int y);
  static bool has_point_1(const PfmFile *file, int x, int y);
  static bool has_point_3(const PfmFile *file, int x, int y);
  static bool has_point_4(const PfmFile *file, int x, int y);
  static bool has_point_chan4(const PfmFile *file, int x, int y);

private:
  typedef vector_float Table;
  Table _table;

  PN_float32 _scale;

  bool _has_no_data_value;
  LPoint4f _no_data_value;

  typedef bool HasPointFunc(const PfmFile *file, int x, int y);
  HasPointFunc *_has_point;

  friend class PfmVizzer;
};

#include "pfmFile.I"

#endif