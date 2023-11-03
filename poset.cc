#include "poset.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <cstdbool>
#include <utility>
#include <cassert>
#include <tuple>



#ifdef NDEBUG
    constexpr bool debug = false;
#else
    constexpr bool debug = true;
#endif


using std::unordered_set;
using std::pair;
using std::tuple;
using std::unordered_map;
using std::string;
using std::get;
using std::cout;
using std::make_pair;



namespace {

using element_id = unsigned long;
using smaller = unordered_set<element_id>;
using bigger =  unordered_set<element_id>;
using poset_relations =  pair<smaller, bigger>;
using id_map = unordered_map<string, element_id>;
using relation_map = unordered_map<element_id, poset_relations>;

/*
 poset_t = 
 1. id_map - map from element (string) to its id (unsigned long)
 2. relation_map - map from element's id to a pair of sets: the set of 
 elements smaller than the element ('smaller') and the set of elements bigger
 than the element ('bigger')
 3. next unused id for potential new poset element 
 (deleting elements from poset does not change this value)
*/
using poset_t = tuple <
                      id_map,  
                      relation_map,
                      element_id
                      >;

using poset_id = unsigned long;
using poset_set_t = unordered_map<poset_id, poset_t>;

poset_set_t& poset_set() {
    poset_set_t initMap;
    static poset_set_t poset_set = initMap;
    return poset_set;
}


bool is_poset_present(unsigned long id) {
    return poset_set().find(id) != poset_set().end();
}

bool is_string_an_element(const string& str, const poset_t& pos) {
   return get<0>(pos).find(str) != get<0>(pos).end();
}
// true if s1 <= s2
bool compareStrings(const string& s1, string& s2, poset_t& poset) {
    if (s1 == s2)
        return true;

    unsigned long id_1 = get<0>(poset).at(s1);
    unsigned long id_2 = get<0>(poset).at(s2);
    
    return get<1>(poset).at(id_1).second.find(id_2) != 
           get<1>(poset).at(id_1).second.end();
    }

/*
 Adds a relation between existing elements with id's 'str_id_1', 'str_id_2'
 in poset with id 'pos_id' in a transitive manner:
 - first element is smaller than second
 - every element smaller than the first is smaller than the second
 - every element bigger than the second is bigger than the first
 - every element smaller than the first is smaller 
   than any element bigger than the second
*/
void set_elements_relations(unsigned long pos_id, unsigned long str_id_1, unsigned long str_id_2) {

    for (auto i : get<1>(poset_set().at(pos_id)).at(str_id_2).second) {
        get<1>(poset_set().at(pos_id)).at(str_id_1).second.insert(i);
        get<1>(poset_set().at(pos_id)).at(i).first.insert(str_id_1);
        
        for (auto j : get<1>(poset_set().at(pos_id)).at(str_id_1).first) {
            get<1>(poset_set().at(pos_id)).at(j).second.insert(i);
            get<1>(poset_set().at(pos_id)).at(i).first.insert(j);
        }
    }
    
    for (auto i : get<1>(poset_set().at(pos_id)).at(str_id_1).first) {
        get<1>(poset_set().at(pos_id)).at(str_id_2).first.insert(i);
        get<1>(poset_set().at(pos_id)).at(i).second.insert(str_id_2);
    }

    get<1>(poset_set().at(pos_id)).at(str_id_2).first.insert(str_id_1);
    get<1>(poset_set().at(pos_id)).at(str_id_1).second.insert(str_id_2);
    
}

void rem_aux(unsigned long pos_id, const string& str, unsigned long str_id) {

    for (auto i : get<1>(poset_set().at(pos_id)).at(str_id).first) {
        get<1>(poset_set().at(pos_id)).at(i).second.erase(str_id);
    }
    for (auto i : get<1>(poset_set().at(pos_id)).at(str_id).second) {
        get<1>(poset_set().at(pos_id)).at(i).first.erase(str_id);
    }
 
    // usuwamy z map stringa i jego id.
    get<0>(poset_set().at(pos_id)).erase(str);
    get<1>(poset_set().at(pos_id)).erase(str_id);

}

void insert_aux(unsigned long pos_id, const string& str) {
   
    unordered_set<unsigned long> bigger;
    unordered_set<unsigned long> smaller;
    unsigned long str_id = get<2>(poset_set().at(pos_id));

    get<2>(poset_set().at(pos_id))++;
    get<0>(poset_set().at(pos_id)).insert({str, str_id});
    get<1>(poset_set().at(pos_id)).insert({str_id, make_pair(smaller, bigger)});
}


bool check_null_two_args(const string& function_name, char const *value1, char const *value2) {

    bool res = true;
   
    if (value1 == NULL) {
        if constexpr (debug) {
            cout << function_name << ": invalid value1 (NULL)\n";
        }
       res = false;
    }
    if (value2 == NULL) {
        if constexpr (debug) {
            cout << function_name << ": invalid value2 (NULL)\n"; 
        }
        res = false;
    }
    return res;
 }



}



 namespace cxx {
    
unsigned long poset_new(void) {
    
    if constexpr (debug) {
        cout << "poset_new()\n";
    }

    static unsigned long id_count{0}; 
    poset_t new_poset;
    get<2>(new_poset) = 0;
    poset_set().insert({id_count, new_poset});

    if constexpr (debug) {
        cout << "poset_new: poset " << id_count << " created\n";
    }

    return id_count++;
}



void poset_delete(unsigned long id) {
    
    if constexpr (debug) {
        cout << "poset_delete(" << id << ")\n";
    }

    if (!is_poset_present(id)) {
        if constexpr (debug) {
              cout << "poset_delete: poset " << id << " does not exist\n";
    }
        
    }
    else {
      if constexpr (debug) {
            cout << "poset_delete: poset " << id << " deleted\n";
        }
        poset_set().erase(id);
    }
        
}



size_t poset_size(unsigned long id) {
    
    if constexpr (debug) {
        cout << "poset_size(" << id << ")\n";
    }
    size_t size = 0;

    if (is_poset_present(id)) {
         size = get<0>(poset_set().at(id)).size();

        if constexpr (debug) {
            cout << "poset_size: poset " << id << " contains " << size << " element(s)\n";
        }
    }
    else {
        if constexpr (debug) {
            cout << "poset_size: poset " << id << " does not exist\n";
    }
    }
    return size;
}



bool poset_insert(unsigned long id, char const *value) {
    
    if (value == NULL) {
            if constexpr (debug) {
                cout << "poset_insert(" << id << ", " << "NULL)\n";
                cout << "poset_insert: invalid value (NULL)\n";
            }

            return false;
        }

    if constexpr (debug) {
        cout << "poset_insert(" << id << ", \"" << value << "\")\n";
    }
    
    if (!is_poset_present(id)) {
        
        if constexpr (debug) {
            cout << "poset_insert: poset " << id <<" does not exist\n";
        }

        return false;
    }

    string str{value};

    if (is_string_an_element(str, poset_set().at(id))) {
       
        if constexpr (debug) {
            cout << "poset_insert: poset " << id << ", element \"" << str <<"\" already exists\n";
        }

        return false;
     }
     insert_aux(id, str);

    if constexpr (debug) {
        cout << "poset_insert: poset " << id << ", element \"" << str << "\" inserted\n";
    }

    return true;
}



bool poset_remove(unsigned long id, char const *value) {
    
    if constexpr (debug) {
        cout << "poset_remove(" << id << ", \"" << value << "\")\n";
    }

    if (!is_poset_present(id)) {
        if constexpr (debug) {
            cout << "poset_remove: poset " << id << " does not exist\n";
        }

        return false;
    }
    if (value == NULL) {
        if constexpr (debug) {
            cout << "poset_remove: invalid value (NULL)\n";
        }

        return false;
    }

    string str{value};

    if (!is_string_an_element(str, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_remove: poset " << id <<", element \"" << str << "\" does not exist\n";
        }

        return false;
    }

    // the id of element being deleted
    unsigned long str_id = get<0>(poset_set().at(id)).at(str);
    
    rem_aux(id, str, str_id);

    if constexpr (debug) {
        cout << "poset_remove: poset " << id << ", element \"" << str << "\" removed\n";
    }

    return true;
}

bool poset_add(unsigned long id, char const *value1, char const *value2) {
    
    if constexpr (debug) {
        cout << "poset_add(" << id << 
        ", " << (value1 != NULL ? ("\"" + string(value1) + "\"" ) : "NULL") << 
        ", " << (value2 != NULL ? ("\"" + string(value2) + "\"" ) : "NULL") << ")\n"; 
    }

    if (!is_poset_present(id)) {
        if constexpr (debug) {
            cout << "poset_add: poset " << id << " does not exist\n"; 
        }

        return false;
    }

    string function_name = "poset add";
    if (!check_null_two_args(function_name, value1, value2)) {
        return false;
    }
    string str1 {value1};
    string str2 {value2};

    if (!is_string_an_element(str1, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_add: poset " << id << ", element \"" << str1 << "\" does not exist\n"; 
        }

        return false;
    }
   
    if (!is_string_an_element(str2, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_add: poset " << id << ", element \"" << str2 << "\" does not exist\n"; 
        }

        return false;
    }

    unsigned long str_id1 = get<0>(poset_set().at(id)).at(str1);
    unsigned long str_id2 = get<0>(poset_set().at(id)).at(str2);

    //check if elements are in a relation
    if (compareStrings(str1, str2, poset_set().at(id)) or
        compareStrings(str2, str1, poset_set().at(id))) {

        if constexpr (debug) {
            cout << "poset_add: poset " << id << ", relation (\"" <<
                    str1 << "\", \"" << str2 << "\") cannot be added\n";
        }

        return false;
    }

    set_elements_relations(id, str_id1, str_id2);

    if constexpr (debug) {
        cout << "poset_add: poset " << id <<
                ", relation (\"" << str1 << "\", \"" << str2 << "\") added\n";
    }

    return true;
}



bool poset_del(unsigned long id, char const *value1, char const *value2) {
    
    if constexpr (debug) {
        cout << "poset_del(" << id <<
            ", " << (value1 != NULL ? ("\"" + string(value1) + "\"" ) : "NULL") <<
            ", " << (value2 != NULL ? ("\"" + string(value2) + "\"" ) : "NULL") <<
            ")\n"; 
    }

    if (!is_poset_present(id)) {
        if constexpr (debug) {
            cout << "poset_del: poset " << id << " does not exist\n"; 
        }

        return false;
    }

     string function_name = "poset del";
    if (!check_null_two_args(function_name, value1, value2)) {
        return false;
    }
    
    string str1 {value1};
    string str2 {value2};   
    
    if (!is_string_an_element(str1, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_del: poset " << id << ", element \"" <<
                    str1 << "\" does not exist\n"; 
        }

        return false;
    }

    if (!is_string_an_element(str2, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_del: poset " << id << ", element \"" << 
                    str2 << "\" does not exist\n"; 
        }

        return false;
    }
    
    unsigned long str_id1 = get<0>(poset_set().at(id)).at(str1);
    unsigned long str_id2 = get<0>(poset_set().at(id)).at(str2);
    
    // checking if they are in relation
    if (!compareStrings(str1, str2, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_del: poset " << id << ", relation (\"" <<
                    str1 << "\", \"" << str2 << "\") cannot be deleted\n";
        }

        return false;
    }

    // checking if removing relation between them
    //  wont disrupt given conditions
    if (str_id1 == str_id2) {
        if constexpr (debug) {
            cout << "poset_del: poset " << id << ", relation (\"" <<
                    str1 << "\", \"" << str2 << "\") cannot be deleted\n";
        }

        return false;
    }

    // checking if there exists an element x such that
    // str1 < x < str2
    for (auto i : get<1>(poset_set().at(id)).at(str_id1).second) {
        if (get<1>(poset_set().at(id)).at(i).second.find(str_id2) != 
            get<1>(poset_set().at(id)).at(i).second.end()) {
            
            if constexpr (debug) {
                cout << "poset_del: poset " << id << ", relation (\"" <<
                        str1 << "\", \"" << str2 << "\") cannot be deleted\n";
            }
            return false;
        }
    }

    get<1>(poset_set().at(id)).at(str_id1).second.erase(str_id2);
    get<1>(poset_set().at(id)).at(str_id2).first.erase(str_id1);

    if constexpr (debug) {
        cout << "poset_del: poset " << id << ", relation (\"" <<
                str1 << "\", \"" << str2 << "\") deleted\n";
    }

    return true;
}

bool poset_test(unsigned long id, char const *value1, char const *value2) {

    if constexpr (debug) {
        cout << "poset_test(" << id <<
            ", " << (value1 != NULL ? ("\"" + string(value1) + "\"" ) : "NULL") <<
            ", " << (value2 != NULL ? ("\"" + string(value2) + "\"" ) : "NULL") << ")\n"; 
    }

    if (!is_poset_present(id)) {
        if constexpr (debug) {
            cout << "poset_test: poset " << id << " does not exist\n"; 
        }

        return false;
    }

    string function_name = "poset test";
    if (!check_null_two_args(function_name, value1, value2)) {
        return false;
    }
    string str1 {value1};
    string str2 {value2};   

    if (!is_string_an_element(str1, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_test: poset " << id << ", element \"" << str1 <<
                    "\" does not exist\n"; 
        }

        return false;
    }
    if (!is_string_an_element(str2, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_test: poset " << id << ", element \"" << str2 << 
                    "\" does not exist\n"; 
        }

        return false;
    }
    if (!compareStrings(str1, str2, poset_set().at(id))) {
        if constexpr (debug) {
                cout << "poset_test: poset " << id << ", relation (\"" <<
                        str1 << "\", \"" << str2 << "\") does not exist\n";
        }

        return false;
    }

    if constexpr (debug) {
                cout << "poset_test: poset " << id << ", relation (\"" << 
                        str1 << "\", \"" << str2 << "\") exists\n";
    }

    return true;
}



void poset_clear(unsigned long id) {
    
    if constexpr (debug) {
        cout << "poset_clear(" << id << ")\n";
    }

    if (!is_poset_present(id)) {
        if constexpr (debug) {
            cout << "poset_clear: poset " << id << " does not exist\n";
        }
        
    }
    else {
    unordered_map<string, unsigned long> strMap;
    unordered_map<unsigned long, poset_relations> relMap;

    get<0>(poset_set().at(id)) = strMap;
    get<1>(poset_set().at(id)) = relMap;
    get<2>(poset_set().at(id)) = 0;
    
    if constexpr (debug) {
            cout << "poset_clear: poset " << id << " cleared\n";
        }
    }    
}
 }

