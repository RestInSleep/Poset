#ifndef poset.h
#define poset.h

#ifdef __cplusplus
#include <iostream>
#endif


#include <stdlib.h>

  #ifdef __cplusplus
namespace cxx {
    extern "C" {
  #endif
        unsigned long poset_new(void);
        void poset_delete(unsigned long id);
        size_t poset_size(unsigned long id);
        bool poset_insert(unsigned long id, char const *value);
        bool poset_remove(unsigned long id, char const *value);
        bool poset_add(unsigned long id, char const *value1, char const *value2);
        bool poset_del(unsigned long id, char const *value1, char const *value2);
        bool poset_test(unsigned long id, char const *value1, char const *value2);
        void poset_clear(unsigned long id);
  
  #ifdef __cplusplus 
    }
}
  #endif

#endif