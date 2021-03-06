/**\file*********************************************************************
 *                                                                     \brief
 *  Class template set [23.3.3 lib.list]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_SET
#define NTL__STLX_SET
#pragma once

#include "algorithm.hxx"
#include "ext/rbtree.hxx"
#include "range.hxx"

namespace std {

/**\addtogroup  lib_containers ********* 23 Containers library [containers]
 *@{*/

/**\addtogroup  lib_associative *********** 23.3 Associative containers [associative]
 *@{*/

  /// Class template set [23.3.3]
  template <class Key, class Compare = less<Key>, class Allocator = allocator<Key> >
  class set:
    public std::ext::tree::rb_tree<Key, Compare, Allocator>
  {
    typedef std::ext::tree::rb_tree<Key, Compare, Allocator> tree_type;
    using tree_type::node;
  public:
    // types:
    typedef Key                                   key_type;
    typedef Key                                   value_type;
    typedef Compare                               key_compare;
    typedef Compare                               value_compare;

    typedef Allocator                             allocator_type;
    typedef typename  
      Allocator::template rebind<value_type>::other allocator;
    typedef typename  allocator::pointer          pointer;
    typedef typename  allocator::const_pointer    const_pointer;
    typedef       value_type&                     reference;
    typedef const value_type&                     const_reference;
    typedef typename  allocator::size_type        size_type;
    typedef typename  allocator::difference_type  difference_type;

    typedef tree_type::iterator                   iterator;
    typedef tree_type::const_iterator             const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  public:
    // 23.3.3.1 construct/copy/destroy:
    explicit set(const Compare& comp = Compare(),
      const Allocator& a = Allocator())
      :tree_type(comp, a)
    {}

    template <class InputIterator>
    set(InputIterator first, InputIterator last,
      const Compare& comp = Compare(), const Allocator& a = Allocator())
      :tree_type(first, last, comp, a)
    {}

    set(const set<Key,Compare,Allocator>& x)
      :tree_type(x)
    {}
    #ifdef NTL_CXX_RV
    set(set<Key,Compare,Allocator>&& x)
      :tree_type(x)
    {}
    #endif

    explicit set(const Allocator& a)
      :tree_type(a)
    {}
    set(const set& x, const Allocator& a)
      :tree_type(x, a)
    {}

    #ifdef NTL_CXX_RV
    set(set&& x, const Allocator& a)
      :tree_type(x, a)
    {}
    #endif

    set(initializer_list<value_type> il, const Compare& comp = Compare(), const Allocator& a = Allocator())
      :tree_type(il.begin(), il.end(), comp, a)
    {}

    ~set()
    {}

    set<Key,Compare,Allocator>& operator= (const set<Key,Compare,Allocator>& x)
    {
      clear();
      assign(x);
      return *this;
    }

    #ifdef NTL_CXX_RV
    set<Key,Compare,Allocator>& operator= (set<Key,Compare,Allocator>&& x)
    {
      clear();
      assign(x);
      return *this;
    }

#ifdef NTL_STLX_RANGE
    ///\name Range extension
    template<class Iter>
    explicit set(std::range<Iter>&& R, const Compare& comp = Compare(), const Allocator& a = Allocator())
      :tree_type(comp, a)
    {
      insert(forward<Range>(R));
    }
    template<class Iter>
    set& operator=(std::range<Iter>&& R)
    {
      clear();
      insert(forward<Range>(R));
      return *this;
    }
    template<class Iter>
    void insert(std::range<Iter>&& R)
    {
      insert_range(__::ranged::adl_begin(R), __::ranged::adl_end(R));
    }
    ///\}
#endif // NTL_STLX_RANGE
    #endif

    // modifiers:
    #ifdef NTL_CXX_VT
    template <class... Args> pair<iterator, bool> emplace(Args&&... args)
    {
      return tree_type::emplace_hint(end(), std::forward<Args>(args)...);
    }
    template <class... Args> iterator emplace(const_iterator position, Args&&... args)
    {
      return tree_type::emplace_hint(position, std::forward<Args>(args)...).first;
    }
    #endif

    std::pair<iterator, bool> insert(const value_type& x)
    {
      bool greater;
      std::pair<node*, node*> place = find_node(x, greater);
      if(place.first)
        return std::make_pair(make_iterator(place.first), false);
      return std::make_pair(insert_impl(place.second, construct_node(x), greater), true);
    }

    iterator insert(const_iterator /*position*/, const value_type& x)
    {
      // TODO: implement fast insert function based on position
      return insert(x).first;
    }

#ifdef NTL_CXX_RV
    std::pair<iterator, bool> insert(value_type&& x)
    {
      return tree_type::insert_reference(std::forward<value_type>(x));
    }
    iterator insert(const_iterator /*position*/, value_type&& x)
    {
      // TODO: implement fast insert function based on position
      return tree_type::insert_reference(std::forward<value_type>(x)).first;
    }
#endif

    // observers:
    key_compare key_comp() const { return tree_type::value_comp(); }
    value_compare value_comp() const { return tree_type::value_comp(); }

    // set operations:
    size_type count(const key_type& x) const
    {
      return find(x) != end() ? 1 : 0;
    }
#ifdef NTL_CXX_TYPEOF
    template<typename K>
    typename enable_if<__::is_transparent<Compare, K>::value, size_type>::type count(const K& x)
    {
      return find(x) != end() ? 1 : 0;
    }
#endif

    iterator        lower_bound(const key_type& x)        { return equal_range(x)->second; }
    const_iterator  lower_bound(const key_type& x) const  { return equal_range(x)->second; }
    iterator        upper_bound(const key_type& x)        { return equal_range(x)->first;  }
    const_iterator  upper_bound(const key_type& x) const  { return equal_range(x)->first;  }

    pair<iterator,iterator> equal_range(const key_type& x)
    {
      // find a node with value which are equal or nearest to the x
      node* p = tree_type::root_;
      while(p){
        if(tree_type::elem_less(x, p->elem)){
          if(p->child[tree_type::left]){
            p = p->child[tree_type::left];
          }else{
            return make_pair(tree_type::make_iterator(p), tree_type::make_iterator(p)); // is a closest nodes
          }
        }else if(tree_type::elem_greater(x, p->elem))
          p = p->child[tree_type::right];
        else
          return make_pair(tree_type::make_iterator(p),
              tree_type::make_iterator(tree_type::next(p, tree_type::right)));
      }
      return make_pair(tree_type::make_iterator(p), tree_type::make_iterator(p));
    }

    pair<const_iterator,const_iterator> equal_range(const key_type& x) const
    {
      return equal_range(x);
    }
  };

  // specialized algorithms:
  template <class Key, class Compare, class Allocator>
  inline void swap(set<Key,Compare,Allocator>& x, set<Key,Compare,Allocator>& y) { x.swap(y); }

  ///////////////////////////////////////////////////////////////////////////

} //namespace std

#endif //#ifndef NTL__STLX_SET
