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
using std::endl;
using std::to_string;


namespace {
//poset = 
// 1. mapa stringow na longi(id)        mapping string to long:             string ->       long
// 2. mapa z longow na 2 sety longow    mapping long to pair of sets:       long   -> set, set
// 3. wartosc nowego id                 new id value
using smaller = unordered_set<unsigned long>;
using bigger =  unordered_set<unsigned long>;
using poset_relations =  pair<smaller, bigger>;
using poset_t = tuple<unordered_map<string, unsigned long>, 
                      unordered_map<unsigned long, poset_relations>, 
                      unsigned long
                     >;

unordered_map<unsigned long, poset_t>& poset_set() {
    unordered_map<unsigned long, poset_t> initMap;
    static unordered_map<unsigned long, poset_t> poset_set = initMap;
    return poset_set;
}


bool is_poset_present(unsigned long id) {
    return poset_set().find(id) != poset_set().end();
}

bool isStringAnElement(const string& str, const poset_t& poset) {
   return get<0>(poset).find(str) != get<0>(poset).end();
}
// true if s1 <= s2
bool compareStrings(const string& s1, string& s2, poset_t& poset) {
    if (s1 == s2)
        return true;
    unsigned long id_1 = get<0>(poset).at(s1);
    unsigned long id_2 = get<0>(poset).at(s2);
    return get<1>(poset).at(id_1).second.find(id_2) != get<1>(poset).at(id_1).second.end();
    }
}
#ifdef __cplusplus
namespace cxx {
extern "C" {
#endif

unsigned long poset_new(void) {
    

    if constexpr (debug) {
        cout << "poset_new()\n";
    }

    static unsigned long id_count{0}; // static init fiasco!
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


    if (is_poset_present(id)) {
        if constexpr (debug) {
            cout << "poset_delete: poset " << id << " deleted\n";
        }
        poset_set().erase(id);
        return;
    }

    if constexpr (debug) {
        cout << "poset_delete: poset " << id << " does not exist\n";
    }
}

size_t poset_size(unsigned long id) {
    

    if constexpr (debug) {
        cout << "poset_size(" << id << ")\n";
    }


    if (is_poset_present(id)) {

        size_t size = get<0>(poset_set().at(id)).size();

        if constexpr (debug) {
            cout << "poset_size: poset " << id << " contains " << size << " element(s)\n";
        }

        return size;
    }
    if constexpr (debug) {
        cout << "poset_size: poset " << id << " does not exist\n";
    }

    return 0;
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

    if (isStringAnElement(str, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_insert: poset " << id << ", element \"" << str <<"\" already exists\n";

        }
        return false;
     }

    unordered_set<unsigned long> bigger;
    unordered_set<unsigned long> smaller;
    unsigned long strId = get<2>(poset_set().at(id));

    get<2>(poset_set().at(id))++;
    get<0>(poset_set().at(id)).insert({str, strId});
    get<1>(poset_set().at(id)).insert({strId, std::make_pair(smaller, bigger)});

    if constexpr (debug) {
        cout << "poset_insert: poset " << 0 << ", element \"" << str << "\" inserted\n";
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

    if (!isStringAnElement(str, poset_set().at(id))) {
        
        if constexpr (debug) {
            cout << "poset_remove: poset " << id <<", element \"" << str << "\" does not exist\n";
        }
        return false;
    }

    // id usuwanego elementu
    unsigned long strId = get<0>(poset_set().at(id)).at(str);

    for (auto i : get<1>(poset_set().at(id)).at(strId).first) {
        get<1>(poset_set().at(id)).at(i).second.erase(strId);
    }
    for(auto i : get<1>(poset_set().at(id)).at(strId).second) {
        get<1>(poset_set().at(id)).at(i).first.erase(strId);
    }
 
    // usuwamy z map stringa i jego id.
    get<0>(poset_set().at(id)).erase(str);
    get<1>(poset_set().at(id)).erase(strId);

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

    if (value1 == NULL and value2 == NULL) {
        if constexpr (debug) {
            cout << "poset_add: invalid value1 (NULL)\n";
            cout << "poset_add: invalid value2 (NULL)\n"; 
        }
        return false;
    }


    if (value1 == NULL) {
        if constexpr (debug) {
            cout << "poset_add: invalid value1 (NULL)\n";
        }
        return false;
    }
    if (value2 == NULL) {
        if constexpr (debug) {
            cout << "poset_add: invalid value2 (NULL)\n"; 
        }
        return false;
    }

    string str1 {value1};
    string str2 {value2};

    if (!isStringAnElement(str1, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_add: poset " << id << ", element \"" << str1 << "\" does not exist\n"; 
        }
        return false;
    }
    if (!isStringAnElement(str2, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_add: poset " << id << ", element \"" << str2 << "\" does not exist\n"; 
        }
        return false;
    }

    unsigned long strId1 = get<0>(poset_set().at(id)).at(str1);
    unsigned long strId2 = get<0>(poset_set().at(id)).at(str2);

    //check if elements are in a relation
    if (compareStrings(str1, str2, poset_set().at(id)) or
        compareStrings(str2, str1, poset_set().at(id))) {
        
        if constexpr (debug) {
            cout << "poset_add: poset " << id << ", relation (\"" << str1 << "\", \"" << str2 << "\") cannot be added\n";
        }

        return false;
    }

    //every element bigger than the second element
    // is now bigger than the first
    for (auto i : get<1>(poset_set().at(id)).at(strId2).second) {
        get<1>(poset_set().at(id)).at(strId1).second.insert(i);
        get<1>(poset_set().at(id)).at(i).first.insert(strId1);
    }
    for(auto i : get<1>(poset_set().at(id)).at(strId1).first) {
        get<1>(poset_set().at(id)).at(strId2).first.insert(i);
        get<1>(poset_set().at(id)).at(i).second.insert(strId2);
    }
    
    // every element smaller than the first element
    // is now smaller than the second
    get<1>(poset_set().at(id)).at(strId2).first.insert(strId1);
    get<1>(poset_set().at(id)).at(strId1).second.insert(strId2);
    
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
            ", " << (value2 != NULL ? ("\"" + string(value2) + "\"" ) : "NULL") << ")\n"; 
    }


    if (!is_poset_present(id)) {
        if constexpr (debug) {
            cout << "poset_del: poset " << id << " does not exist\n"; 
        }
        return false;
    }

    if (value1 == NULL and value2 == NULL) {
        if constexpr (debug) {
            cout << "poset_del: invalid value1 (NULL)\n";
            cout << "poset_del: invalid value2 (NULL)\n"; 
        }
        return false;
    }

    if (value1 == NULL) {
        if constexpr (debug) {
            cout << "poset_del: invalid value1 (NULL)\n";
        }
        return false;
    }
    if (value2 == NULL) {
        if constexpr (debug) {
            cout << "poset_del: invalid value2 (NULL)\n"; 
        }
        return false;
    }
    
    string str1 {value1};
    string str2 {value2};   
    
    if (!isStringAnElement(str1, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_del: poset " << id << ", element \"" << str1 << "\" does not exist\n"; 
        }
        return false;
    }
    if (!isStringAnElement(str2, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_del: poset " << id << ", element \"" << str2 << "\" does not exist\n"; 
        }
        return false;
    }
    unsigned long strId1 = get<0>(poset_set().at(id)).at(str1);
    unsigned long strId2 = get<0>(poset_set().at(id)).at(str2);
    
    // checking if they are in relation
    if (!compareStrings(str1, str2, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_del: poset " << id << ", relation (\"" << str1 << "\", \"" << str2 << "\") cannot be deleted\n";
        }
        return false;
    }

    // sprawdzamy, czy usuniecie relacji miedzy elementami == checking if removing relation between them
    // nie zaburzy zwrotnosci, przechodniosci i antysymetrycznosci == wont disrupt given conditions
    if (strId1 == strId2) {
        if constexpr (debug) {
            cout << "poset_del: poset " << id << ", relation (\"" << str1 << "\", \"" << str2 << "\") cannot be deleted\n";
        }
        return false;
    }

    // checking if there exists an element x such that
    // str1 < x < str2
    for (auto i : get<1>(poset_set().at(id)).at(strId1).second) {
        if (get<1>(poset_set().at(id)).at(i).second.find(strId2) != 
            get<1>(poset_set().at(id)).at(i).second.end()) {
            if constexpr (debug) {
                cout << "poset_del: poset " << id << ", relation (\"" << str1 << "\", \"" << str2 << "\") cannot be deleted\n";
            }
            return false;
        }
    }

    get<1>(poset_set().at(id)).at(strId1).second.erase(strId2);
    get<1>(poset_set().at(id)).at(strId2).first.erase(strId1);

    if constexpr (debug) {
        cout << "poset_del: poset " << id << ", relation (\"" << str1 << "\", \"" << str2 << "\") deleted\n";
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

    if (value1 == NULL and value2 == NULL) {
        if constexpr (debug) {
            cout << "poset_test: invalid value1 (NULL)\n";
            cout << "poset_test: invalid value2 (NULL)\n"; 
        }
        return false;
    }

    if (value1 == NULL) {
        if constexpr (debug) {
            cout << "poset_test: invalid value1 (NULL)\n";
        }
        return false;
    }
    if (value2 == NULL) {
        if constexpr (debug) {
            cout << "poset_test: invalid value2 (NULL)\n"; 
        }
        return false;
    }

    string str1 {value1};
    string str2 {value2};   

    if (!isStringAnElement(str1, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_test: poset " << id << ", element \"" << str1 << "\" does not exist\n"; 
        }
        return false;
    }
    if (!isStringAnElement(str2, poset_set().at(id))) {
        if constexpr (debug) {
            cout << "poset_test: poset " << id << ", element \"" << str2 << "\" does not exist\n"; 
        }
        return false;
    }
    if (!compareStrings(str1, str2, poset_set().at(id))) {
        if constexpr (debug) {
                cout << "poset_test: poset " << id << ", relation (\"" << str1 << "\", \"" << str2 << "\") does not exist\n";
        }
        return false;
    }

    if constexpr (debug) {
                cout << "poset_test: poset " << id << ", relation (\"" << str1 << "\", \"" << str2 << "\") exists\n";
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
        return;
    }

    unordered_map<string, unsigned long> strMap;
    unordered_map<unsigned long, poset_relations> relMap;

    get<0>(poset_set().at(id)) = strMap;
    get<1>(poset_set().at(id)) = relMap;
    get<2>(poset_set().at(id)) = 0;
    
    if constexpr (debug) {
            cout << "poset_clear: poset " << id << " cleared\n";
        }
    return;
}    

#ifdef __cplusplus
    }
}
#endif

