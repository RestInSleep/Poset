#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <stdbool.h>
#include <utility>
#include <cassert>
#include <tuple>

namespace {
//poset = 
// 1. mapa stringow na longi(id)
// 2. mapa z longow na 2 sety longow
// 3. wartosc nowego id
using smaller = std::unordered_set<unsigned long>;
using bigger =  std::unordered_set<unsigned long>;

using poset_relations =  std::pair<smaller, bigger>;
using poset_t = std::tuple<std::unordered_map<std::string, unsigned long>, std::unordered_map<unsigned long, poset_relations>, unsigned long>;
std::unordered_map<unsigned long, poset_t>& poset_set() {
std::unordered_map<unsigned long, poset_t> initMap;
static std::unordered_map<unsigned long, poset_t> poset_set = initMap;
return poset_set;
}


bool is_poset_present(unsigned long id) {
    return poset_set().find(id) != poset_set().end();
}

bool isStringAnElement(const std::string& str, const poset_t& poset) {
   return std::get<0>(poset).find(str) != std::get<0>(poset).end();
}
// true iff s1 <= s2
bool compareStrings(const std::string& s1, std::string& s2, poset_t& poset) {
    if (s1 == s2)
        return true;
    unsigned long id_1 = std::get<0>(poset).at(s1);
    unsigned long id_2 = std::get<0>(poset).at(s2);
    return std::get<1>(poset).at(id_1).second.find(id_2) != std::get<1>(poset).at(id_1).second.end();
    }
}
#ifdef __cplusplus
namespace cxx {
extern "C" {
#endif

unsigned long poset_new(void) {
    static unsigned long id_count{0}; // static init fiasco!
    poset_t new_poset;
    std::get<2>(new_poset) = 0;
    poset_set().insert({id_count, new_poset});
    return id_count++;
}

void poset_delete(unsigned long id) {
    if(is_poset_present(id)) {
        poset_set().erase(id);
    }
}

size_t poset_size(unsigned long id) {
    if(is_poset_present(id)) {
       return std::get<0>(poset_set().at(id)).size();
    }
    return 0;
}

bool poset_insert(unsigned long id, char const *value) {
    if(!is_poset_present(id)) {
       return false;
    }
    if (value == NULL) {
        return false;
    }
   std::string str{value};
   if(isStringAnElement(str, poset_set().at(id))) {
    return false;
   }
   std::unordered_set<unsigned long> bigger;
   std::unordered_set<unsigned long> smaller;
   unsigned long strId = std::get<2>(poset_set().at(id));
   std::get<2>(poset_set().at(id))++;
   std::get<0>(poset_set().at(id)).insert({str, strId});
   std::get<1>(poset_set().at(id)).insert({strId, std::make_pair(smaller, bigger)});
   return true;
}

bool poset_remove(unsigned long id, char const *value) {
    if(!is_poset_present(id)) {
        return false;
    }
    if(value == NULL) {
        return false;
    }
    std::string str{value};
    if(!isStringAnElement(str, poset_set().at(id))) {
        return false;
   }
   // id usuwanego elementu
   unsigned long strId = std::get<0>(poset_set().at(id)).at(str);
   for(auto i : std::get<1>(poset_set().at(id)).at(strId).first) {
        std::get<1>(poset_set().at(id)).at(i).second.erase(strId);
   }
    for(auto i : std::get<1>(poset_set().at(id)).at(strId).second) {
        std::get<1>(poset_set().at(id)).at(i).first.erase(strId);
   }
 
 // usuwamy z map stringa i jego id.
   std::get<0>(poset_set().at(id)).erase(str);
   std::get<1>(poset_set().at(id)).erase(strId);

   return true;
}

bool poset_add(unsigned long id, char const *value1, char const *value2) {
    if (!is_poset_present(id)) {
        return false;
   }
    if (value1 == NULL) {
        return false;
   }
   if (value2 == NULL) {
        return false;
   }
    std::string str1 {value1};
    std::string str2 {value2};
    if (!isStringAnElement(str1, poset_set().at(id))) {
        return false;
    }
    if (!isStringAnElement(str2, poset_set().at(id))) {
        return false;
    }
     unsigned long strId1 = std::get<0>(poset_set().at(id)).at(str1);
     unsigned long strId2 = std::get<0>(poset_set().at(id)).at(str2);
   //check if elements are in a relation
    if(compareStrings(str1, str2, poset_set().at(id)) || compareStrings(str2, str1, poset_set().at(id))) {
        return false;
    }
   //every element bigger than the second element
   // is now bigger than the first
   for(auto i : std::get<1>(poset_set().at(id)).at(strId2).second) {
    std::get<1>(poset_set().at(id)).at(strId1).second.insert(i);
    std::get<1>(poset_set().at(id)).at(i).first.insert(strId1);
   }
    for(auto i : std::get<1>(poset_set().at(id)).at(strId1).first) {
    std::get<1>(poset_set().at(id)).at(strId2).first.insert(i);
    std::get<1>(poset_set().at(id)).at(i).second.insert(strId2);
   }
   // every element smaller than the first element
   // is now smaller than the second
   std::get<1>(poset_set().at(id)).at(strId2).first.insert(strId1);
  std::get<1>(poset_set().at(id)).at(strId1).second.insert(strId2);
   return true;
}


bool poset_del(unsigned long id, char const *value1, char const *value2) {
    if(!is_poset_present(id)) {
        return false;
    }
    if(value1 == NULL) {
        return false;
    }
    if(value2 == NULL) {
        return false;
   }
    std::string str1 {value1};
    std::string str2 {value2};   
    if (!isStringAnElement(str1, poset_set().at(id))) {
        return false;
    }
    if (!isStringAnElement(str2, poset_set().at(id))) {
        return false;
    }
    unsigned long strId1 = std::get<0>(poset_set().at(id)).at(str1);
    unsigned long strId2 = std::get<0>(poset_set().at(id)).at(str2);
   // sprawdzamy czy są w relacji
    if(!compareStrings(str1, str2, poset_set().at(id))) {
        return false;
    }
    // sprawdzamy, czy usuniecie relacji miedzy elementami
    // nie zaburzy zwrotnosci, przechodniosci i antysymetrycznosci
    if(strId1 == strId2) {
        return false;
    }
    // checking if there exists an element x such that
    // str1 < x < str2
    for (auto i : std::get<1>(poset_set().at(id)).at(strId1).second) {
        if (std::get<1>(poset_set().at(id)).at(i).second.find(strId2) != std::get<1>(poset_set().at(id)).at(i).second.end()) {
            return false;
        }
    }
    std::get<1>(poset_set().at(id)).at(strId1).second.erase(strId2);
    std::get<1>(poset_set().at(id)).at(strId2).first.erase(strId1);
    return true;
    }

    bool poset_test(unsigned long id, char const *value1, char const *value2) {
        if (!is_poset_present(id)) {
            return false;
        }
        if(value1 == NULL) {
            return false;
        }
        if(value2 == NULL) {
            return false;
        }
    std::string str1 {value1};
    std::string str2 {value2};   
    if (!isStringAnElement(str1, poset_set().at(id))) {
        return false;
    }
    if (!isStringAnElement(str2, poset_set().at(id))) {
        return false;
    }
    if (!compareStrings(str1, str2, poset_set().at(id))) {
        return false;
    }
    return true;
    }
    void poset_clear(unsigned long id) {
        if (!is_poset_present(id)) {
            return;
        }
        std::unordered_map<std::string, unsigned long> strMap;
        std::get<0>(poset_set().at(id)) = strMap;
        std::unordered_map<unsigned long, poset_relations> relMap;
        std::get<1>(poset_set().at(id)) = relMap;
        std::get<2>(poset_set().at(id)) = 0;
        return;
    }
#ifdef __cplusplus
}
}
#endif

