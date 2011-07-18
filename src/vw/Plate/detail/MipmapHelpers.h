// __BEGIN_LICENSE__
// Copyright (C) 2006-2011 United States Government as represented by
// the Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
// __END_LICENSE__

#ifndef __VW_PLATE_DETAIL_MIPMAPHELPERS_H__
#define __VW_PLATE_DETAIL_MIPMAPHELPERS_H__

#include <boost/assign/list_of.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

namespace vw { namespace platefile { namespace detail {

typedef boost::tuple<uint32, uint32, uint32> rowcoltid_t;
typedef boost::tuple<uint32, uint32>         rowcol_t;
typedef boost::tuple<uint32, rowcoltid_t>    tile_order_t;

inline uint32 therow(const TileHeader& h) { return h.row(); }
inline uint32 thecol(const TileHeader& h) { return h.col(); }
inline uint32 thetid(const TileHeader& h) { return h.transaction_id(); }
inline uint32 therow(const rowcoltid_t& h) { return h.get<0>(); }
inline uint32 thecol(const rowcoltid_t& h) { return h.get<1>(); }
inline uint32 thetid(const rowcoltid_t& h) { return h.get<2>(); }
inline uint32 therow(const rowcol_t& h) { return h.get<0>(); }
inline uint32 thecol(const rowcol_t& h) { return h.get<1>(); }

inline BBox2i move_down(const BBox2i& input, uint32 level_change) {
  return input * (1 << level_change);
}

inline rowcol_t parent_tile(const uint32 row, const uint32 col) {
  return rowcol_t(row/2, col/2);
}

// given a parent tile (1 level up) and a hdr, calculate the composite id;
// [0==UL, 1==UR, 2==LL, 3==LR]
template <typename T1, typename T2>
uint32 calc_composite_id(const T1& parent, const T2& hdr) {
  typedef std::map<rowcol_t, uint32> map_t;
  static const map_t lookup = boost::assign::map_list_of
    (rowcol_t(0,0), 0)
    (rowcol_t(0,1), 1)
    (rowcol_t(1,0), 2)
    (rowcol_t(1,1), 3);

  rowcol_t offset(therow(hdr) - therow(parent) * 2,
                  thecol(hdr) - thecol(parent) * 2);

  map_t::const_iterator i = lookup.find(offset);
  VW_ASSERT(i != lookup.end(), LogicErr() << "Cannot determine composite id for hdr " << thecol(hdr) << "," << therow(hdr) << " and parent "
      << thecol(parent) << "," << therow(parent));
  return i->second;
}

struct SortByTidDesc {
  bool operator()(const tile_order_t& a, const tile_order_t& b)
  {
    return b.get<0>() < a.get<0>();
  }
};

}

inline std::ostream& operator<<(std::ostream& o, const detail::rowcoltid_t& hdr) {
  return (o << detail::thecol(hdr) << "," << detail::therow(hdr) << " (t_id = " << detail::thetid(hdr) << ")");
}

}}

#endif