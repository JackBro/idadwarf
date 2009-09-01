#ifndef IDADWARF_DIE_CACHE_HPP
#define IDADWARF_DIE_CACHE_HPP

// additional libs headers
#include <dwarf.h>
#include <libdwarf.h>

// local headers
#include "defs.hpp"
#include "ida_utils.hpp"

using namespace std;

// DIE caching struct

enum die_type { DIE_USELESS, DIE_TYPE, DIE_FUNC, DIE_VAR };

struct die_cache
{
  die_type type; // see above
  union
  {
    // DIE_TYPE specific member
    struct
    {
      ulong ordinal; // type ordinal
      bool second_pass; // cannot get the complete type
      ulong base_ordinal; // ordinal of the type without any modifiers
    };
    // DIE_FUNC/DIE_VAR specific member
    ea_t startEA;
  };
};

// caching stuff
class DieCache
{
public:
  DieCache(void) throw()
    : m_dies_node("$ " PLUGIN_NAME, 0, true)
  {

  }

  virtual ~DieCache(void) throw()
  {
    m_dies_node.kill();
  }

  // cache predicates

  bool in_cache(Dwarf_Off const offset) throw()
  {
    return (m_dies_node.supval(static_cast<sval_t>(offset), NULL,
                                sizeof(die_cache)) != -1);
  }

  // cache getters

  bool get_cache(Dwarf_Off const offset, die_cache *cache) throw();

  bool get_cache_type(Dwarf_Off const offset, die_cache *cache) throw();

  bool get_offset(sval_t const reverse, die_type const type,
                  Dwarf_Off *offset) throw();

  bool get_type_offset(ulong const ordinal, Dwarf_Off *offset) throw()
  {
    return get_offset(static_cast<sval_t>(ordinal), DIE_TYPE, offset);
  }

  bool get_cache_by_ordinal(ulong const ordinal, die_cache *cache) throw();

  nodeidx_t get_first_offset(void) throw()
  {
    return m_dies_node.sup1st();
  }

  nodeidx_t get_next_offset(nodeidx_t idx) throw()
  {
    return m_dies_node.supnxt(idx);
  }

  // cache setters

  void cache_useless(Dwarf_Off const offset) throw();

  void cache_func(Dwarf_Off const offset, ea_t const startEA) throw();

  void cache_type(Dwarf_Off const offset, ulong const ordinal,
                  bool second_pass=false, ulong base_ordinal=0) throw();

private:
// DIEs cache (ordered by offset in .debug_info)
  netnode m_dies_node;

  void cache_useful(Dwarf_Off const offset, sval_t const reverse,
                    die_cache const *cache) throw();
};

#endif // IDADWARF_CACHE_HPP
