/* -*- mode: C++; indent-tabs-mode: nil; -*-
 *
 * This file is a part of LEMON, a generic C++ optimization library.
 *
 * Copyright (C) 2003-2013
 * Egervary Jeno Kombinatorikus Optimalizalasi Kutatocsoport
 * (Egervary Research Group on Combinatorial Optimization, EGRES).
 *
 * Permission to use, modify and distribute this software is granted
 * provided that this copyright notice appears in all copies. For
 * precise terms see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any kind,
 * express or implied, and with no claim as to its suitability for any
 * purpose.
 *
 */

#ifndef LEMON_UNION_FIND_H
#define LEMON_UNION_FIND_H

//!\ingroup auxdat
//!\file
//!\brief Union-Find data structures.
//!

#include <vector>
#include <list>
#include <utility>
#include <algorithm>
#include <functional>

#include <lemon/core.h>

namespace lemon {

  /// \ingroup auxdat
  ///
  /// \brief A \e Union-Find data structure implementation
  ///
  /// The class implements the \e Union-Find data structure.
  /// The union operation uses rank heuristic, while
  /// the find operation uses path compression.
  /// This is a very simple but efficient implementation, providing
  /// only four methods: join (union), find, insert and size.
  /// For more features, see the \ref UnionFindEnum class.
  ///
  /// It is primarily used in Kruskal algorithm for finding minimal
  /// cost spanning tree in a graph.
  /// \sa kruskal()
  ///
  /// \pre You need to add all the elements by the \ref insert()
  /// method.
  template <typename IM>
  class UnionFind {
  public:

    ///\e
    typedef IM ItemIntMap;
    ///\e
    typedef typename ItemIntMap::Key Item;

  private:
    // If the items vector stores negative value for an item then
    // that item is root item and it has -items[it] component size.
    // Else the items[it] contains the index of the parent.
    std::vector<int> items;
    ItemIntMap& index;

    bool rep(int idx) const {
      return items[idx] < 0;
    }

    int repIndex(int idx) const {
      int k = idx;
      while (!rep(k)) {
        k = items[k] ;
      }
      while (idx != k) {
        int next = items[idx];
        const_cast<int&>(items[idx]) = k;
        idx = next;
      }
      return k;
    }

  public:

    /// \brief Constructor
    ///
    /// Constructor of the UnionFind class. You should give an item to
    /// integer map which will be used from the data structure. If you
    /// modify directly this map that may cause segmentation fault,
    /// invalid data structure, or infinite loop when you use again
    /// the union-find.
    UnionFind(ItemIntMap& m) : index(m) {}

    /// \brief Returns the index of the element's component.
    ///
    /// The method returns the index of the element's component.
    /// This is an integer between zero and the number of inserted elements.
    ///
    int find(const Item& a) {
      return repIndex(index[a]);
    }

    /// \brief Clears the union-find data structure
    ///
    /// Erase each item from the data structure.
    void clear() {
      items.clear();
    }

    /// \brief Inserts a new element into the structure.
    ///
    /// This method inserts a new element into the data structure.
    ///
    /// The method returns the index of the new component.
    int insert(const Item& a) {
      int n = items.size();
      items.push_back(-1);
      index.set(a,n);
      return n;
    }

    /// \brief Joining the components of element \e a and element \e b.
    ///
    /// This is the \e union operation of the Union-Find structure.
    /// Joins the component of element \e a and component of
    /// element \e b. If \e a and \e b are in the same component then
    /// it returns false otherwise it returns true.
    bool join(const Item& a, const Item& b) {
      int ka = repIndex(index[a]);
      int kb = repIndex(index[b]);

      if ( ka == kb )
        return false;

      if (items[ka] < items[kb]) {
        items[ka] += items[kb];
        items[kb] = ka;
      } else {
        items[kb] += items[ka];
        items[ka] = kb;
      }
      return true;
    }

    /// \brief Returns the size of the component of element \e a.
    ///
    /// Returns the size of the component of element \e a.
    int size(const Item& a) {
      int k = repIndex(index[a]);
      return - items[k];
    }

  };


} //namespace lemon

#endif //LEMON_UNION_FIND_H
