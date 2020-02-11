// Authors: Piotr Jasinski and Alicja Ziarko

#include "poset.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <string>
#include <cassert>

using namespace std;

#ifdef NDEBUG
const bool debug = false;
#else
const bool debug = true;
#endif

#define MAKE_STRING(x) "\"" << (x == nullptr ? "NULL" : x) << "\""

/// Macros printing information about repeated error types, assume that x is not NULL.
#define POSET_NOT_EXIST(x) "poset " << x << " does not exist"
#define ELEMENT_NOT_EXIST(x) "element \"" << x << "\" does not exist"
#define INVALID_VALUE(x) "invalid " << #x << " (NULL)"

/// Macro creating information about relation (x,y), assumes that x, y are not NULL.
#define RELATION(x, y) "relation (\"" << x << "\", \"" << y << "\")"

/// Macro printing information if version is Debug, assumes that x is not NULL.
#define DEBUG(x) do {if (debug){cerr << __func__ << x << endl;}} while(0)

/// Macro finding relations and id of a given element.
#define FIND_RELATIONS_AND_ID(elementId, relations, transposedRelations, value) \
do { \
    elementId = get_poset_element_id(id, value); \
    if (elementId == INVALID_POSET_ELEMENT_ID) { \
        DEBUG(": poset " << id << ", " << ELEMENT_NOT_EXIST(value)); \
        return false; \
    } \
\
    auto elementIterator = posetToBeAddedTo.find(elementId);  \
    assert(elementIterator != posetToBeAddedTo.end()); \
    \
    relations = (elementIterator->second).first; \
    transposedRelations = (elementIterator->second).second;  \
} while(0)

namespace {
    using poset_id = unsigned long;
    using poset_element_id = uint32_t;
    using poset_element_name = string;
    using relations = unordered_set<poset_element_id>;
    using poset_relations = pair<relations, relations>;
    using poset = unordered_map<poset_element_id, poset_relations>;
    using name_to_element_id = unordered_map<poset_element_name, poset_element_id>;

    const poset_element_id INVALID_POSET_ELEMENT_ID = 0;
    const poset_element_id INITIAL_POSET_ELEMENT_ID = 1;
    const poset_id INITIAL_POSET_ID = 0;

    ///Structure mapping poset_id to poset.
    unordered_map<poset_id, poset> &posets() {
        static unordered_map<poset_id, poset> posets;
        return posets;
    }

    ///Structure mapping poset_id and name of element to its id.
    unordered_map<poset_id, name_to_element_id> &posetIdToMapOfNames() {
        static unordered_map<poset_id, unordered_map<poset_element_name, poset_element_id>> nameToId;
        return nameToId;
    }

    poset_id nextPosetId = INITIAL_POSET_ID;
    poset_element_id nextPosetElementId = INITIAL_POSET_ELEMENT_ID;

    /*
     * INVALID_POSET_ELEMENT_ID if the given id doesn't belong
     * to any poset or if there is no element in that poset with the given name.
     * Otherwise, returns id of the poset element.
     */
    poset_element_id get_poset_element_id(unsigned long id, char const *value) {
        auto posetIdToMapIterator = posetIdToMapOfNames().find(id);
        if (posetIdToMapIterator == posetIdToMapOfNames().end()) {
            return INVALID_POSET_ELEMENT_ID;
        }

        auto nameToIdIterator = (posetIdToMapIterator->second).find(value);
        if (nameToIdIterator == ((posetIdToMapIterator->second)).end()) {
            return INVALID_POSET_ELEMENT_ID;
        }

        return nameToIdIterator->second;
    }

    /*
     * If the parametr transpose has value true, returns the first element of
     * posetRelations, otherwise returns the second element.
     */
    inline relations &getRelations(poset_relations &posetRelations, bool transpose) {
        if (transpose) {
            return posetRelations.first;
        }

        return posetRelations.second;
    }

    /*
     * Iterates over relations of the elements of toIterate and removes the
     * element with the given id from those relations.
     */
    void iterate_and_remove(poset_element_id id, const relations &toIterate, poset &posetRemoveFrom, bool transpose) {
        for (poset_element_id i : toIterate) {
            assert(posetRemoveFrom.find(i) != posetRemoveFrom.end());

            relations &tmpRelations = getRelations(posetRemoveFrom[i], transpose);

            auto tmpRemove = tmpRelations.find(id);
            assert(tmpRemove != tmpRelations.end());

            tmpRelations.erase(tmpRemove);
        }
    }

    /*
     * Adds the elements of relationsToBeAdded to relationsToBeAddedTo so that the
     * relations remain trasnitive.
     */
    void iterate_and_add_relations(const relations &relationsToBeAdded, poset &posetToBeAddedTo,
                                   const relations &relationsToBeAddedTo, bool transpose) {
        for (poset_element_id i : relationsToBeAdded) {
            auto tmpIterator = posetToBeAddedTo.find(i);
            assert(tmpIterator != posetToBeAddedTo.end());

            relations &tmpRelations = getRelations(tmpIterator->second, transpose);

            for (poset_element_id j : relationsToBeAddedTo) {
                tmpRelations.insert(j);
            }
        }
    }

    /*
     * Removes element's id from the map posetIdToMapOfNames.
     * Is called in purpose to preserve the invariant, that
     * the element is present in poset iff some id belongs to
     * its name and poset.
     */
    void remove_element_id(poset_id id, char const *value) {
        auto mapIterator1 = posetIdToMapOfNames().find(id);
        unordered_map<string, poset_element_id> &map = mapIterator1->second;
        auto mapIterator2 = map.find(value);

        map.erase(mapIterator2);
    }

    /*
     * Function that inserts a new element to a given poset.
     */
    bool poset_insert_aux(poset &toInsert) {
        assert(toInsert.find(nextPosetElementId) == toInsert.end());

        toInsert[nextPosetElementId];

        toInsert[nextPosetElementId].first.insert(nextPosetElementId);
        toInsert[nextPosetElementId].second.insert(nextPosetElementId);

        return true;
    }

}

namespace jnp1 {

    unsigned long poset_new(void) {
        DEBUG("()");

        posets()[nextPosetId];
        posetIdToMapOfNames()[nextPosetId];

        DEBUG(": poset " << nextPosetId << " created");

        return nextPosetId++;
    }

    void poset_delete(unsigned long id) {
        DEBUG("(" << id << ")");

        auto posetToBeDeletedIterator = posets().find(id);
        if (posetToBeDeletedIterator == posets().end()) {
            DEBUG(": " << POSET_NOT_EXIST(id));
            return;
        }

        auto idMapIterator = posetIdToMapOfNames().find(id);
        assert(idMapIterator != posetIdToMapOfNames().end());

        posetIdToMapOfNames().erase(idMapIterator);
        posets().erase(posetToBeDeletedIterator);

        DEBUG(": poset " << id << " deleted");
    }

    size_t poset_size(unsigned long id) {
        DEBUG("(" << id << ")");

        auto posetIterator = posets().find(id);

        if (posetIterator == posets().end()) {
            DEBUG(": " << POSET_NOT_EXIST(id));

            return 0;
        }

        size_t size = (posetIterator->second).size();
        DEBUG(": poset " << id << " contains " << size << " element(s)");

        return size;
    }

    bool poset_insert(unsigned long id, char const *value) {
        DEBUG("(" << id << ", " << MAKE_STRING(value) << ")");

        if (value == nullptr) {
            DEBUG(": " << INVALID_VALUE(value));

            return false;
        }

        auto posetToBeInsertedIterator = posets().find(id);
        if (posetToBeInsertedIterator == posets().end()) {
            DEBUG(": " << POSET_NOT_EXIST(id));

            return false;
        }

        if (get_poset_element_id(id, value) != INVALID_POSET_ELEMENT_ID) {
            DEBUG (": poset " << id << ", element \"" << value << "\" already exists");

            return false;
        }

        poset &posetToBeInserted = posetToBeInsertedIterator->second;
        poset_insert_aux(posetToBeInserted);

        assert(posetIdToMapOfNames().find(id) != posetIdToMapOfNames().end());
        assert(posetIdToMapOfNames()[id].find(value) == posetIdToMapOfNames()[id].end());

        posetIdToMapOfNames()[id][value] = nextPosetElementId;

        nextPosetElementId++;

        DEBUG(": poset " << id << ", element \"" << value << "\" inserted");

        return true;
    }

    bool poset_remove(unsigned long id, char const *value) {
        DEBUG("(" << id << ", " << MAKE_STRING(value) << ")");

        if (value == nullptr) {
            DEBUG(": " << INVALID_VALUE(value));

            return false;
        }

        auto posetRemoveFromIterator = posets().find(id);
        if (posetRemoveFromIterator == posets().end()) {
            DEBUG(": " << POSET_NOT_EXIST(id));

            return false;
        }
        poset &posetRemoveFrom = posetRemoveFromIterator->second;


        poset_element_id elementToBeRemovedId = get_poset_element_id(id, value);
        if (elementToBeRemovedId == 0) {
            DEBUG(": poset " << id << ", " << ELEMENT_NOT_EXIST(value));

            return false;
        }
        remove_element_id(id, value);

        auto elementToBeRemovedIterator = posetRemoveFrom.find(elementToBeRemovedId);
        assert(posetRemoveFrom.find(elementToBeRemovedId) != posetRemoveFrom.end());

        poset_relations &elementToBeRemovedRelations = posetRemoveFrom[elementToBeRemovedId];

        //Deleting all the relations that the element to be deleted is in
        iterate_and_remove(elementToBeRemovedId, elementToBeRemovedRelations.first, posetRemoveFrom, false);

        //Deleting all the transposed relations that the element to be deleted is in
        iterate_and_remove(elementToBeRemovedId, elementToBeRemovedRelations.second, posetRemoveFrom, true);

        posetRemoveFrom.erase(elementToBeRemovedIterator);

        DEBUG(": poset " << id << ", element \"" << value << "\" removed");

        return true;
    }

    bool poset_add(unsigned long id, char const *value1, char const *value2) {
        DEBUG("(" << id << ", " << MAKE_STRING(value1) << ", " << MAKE_STRING(value2) << ")");

        if (value1 == nullptr || value2 == nullptr) {
            if (value1 == nullptr) {
                DEBUG(": " << INVALID_VALUE(value1));
            }
            if (value2 == nullptr) {
                DEBUG(": " << INVALID_VALUE(value2));
            }
            return false;
        }

        auto posetToBeAddedToIterator = posets().find(id);
        if (posetToBeAddedToIterator == posets().end()) {
            DEBUG(": " << POSET_NOT_EXIST(id));
            return false;
        }
        poset &posetToBeAddedTo = posetToBeAddedToIterator->second;

        ///Finding and setting the right values for the 3 variables declared below
        poset_element_id firstElementId;
        relations firstElementRelations, firstElementTransposedRelations;
        FIND_RELATIONS_AND_ID(firstElementId, firstElementRelations, firstElementTransposedRelations, value1);

        ///Finding and setting the right values for the 3 variables declared below
        poset_element_id secondElementId;
        relations secondElementRelations, secondElementTransposedRelations;
        FIND_RELATIONS_AND_ID(secondElementId, secondElementRelations, secondElementTransposedRelations, value2);

        if (firstElementRelations.find(secondElementId) != firstElementRelations.end()) {
            DEBUG(": poset " << id << ", " << RELATION(value1, value2) << " already exists");

            return false;
        }
        assert(secondElementTransposedRelations.find(firstElementId) == secondElementTransposedRelations.end());

        if (firstElementTransposedRelations.find(secondElementId) != firstElementTransposedRelations.end()) {
            DEBUG(": poset " << id << ", " << RELATION(value1, value2) << " cannot be added");

            return false;
        }
        assert(secondElementRelations.find(firstElementId) == secondElementRelations.end());

        iterate_and_add_relations(firstElementTransposedRelations, posetToBeAddedTo, secondElementRelations, true);
        iterate_and_add_relations(secondElementRelations, posetToBeAddedTo, firstElementTransposedRelations, false);

        DEBUG(": poset " << id << ", " << RELATION(value1, value2) << " added");

        return true;
    }


    bool poset_del(unsigned long id, char const *value1, char const *value2) {
        DEBUG("(" << id << ", " << MAKE_STRING(value1) << ", " << MAKE_STRING(value2) << ")");
        if (value1 == nullptr || value2 == nullptr) {
            if (value1 == nullptr) {
                DEBUG(": " << INVALID_VALUE(value1));
            }
            if (value2 == nullptr) {
                DEBUG(": " << INVALID_VALUE(value2));
            }

            return false;
        }

        auto posetToBeRemovedFromIterator = posets().find(id);
        if (posetToBeRemovedFromIterator == posets().end()) {
            DEBUG(": " << POSET_NOT_EXIST(id));

            return false;
        }
        poset &posetToBeRemovedFrom = posetToBeRemovedFromIterator->second;

        poset_element_id firstElementId = get_poset_element_id(id, value1);
        if (firstElementId == INVALID_POSET_ELEMENT_ID) {
            DEBUG(": poset " << id << ", " << ELEMENT_NOT_EXIST(value1));

            return false;
        }
        assert(posetToBeRemovedFrom.find(firstElementId) != posetToBeRemovedFrom.end());

        poset_element_id secondElementId = get_poset_element_id(id, value2);
        if (secondElementId == INVALID_POSET_ELEMENT_ID) {
            DEBUG(": poset " << id << ", " << ELEMENT_NOT_EXIST(value2));

            return false;
        }
        assert(posetToBeRemovedFrom.find(secondElementId) != posetToBeRemovedFrom.end());

        //Every poset element must be in relation with itself
        if (firstElementId == secondElementId) {
            DEBUG(": poset " << id << ", " << RELATION(value1, value2) << " cannot be deleted");

            return false;
        }

        poset_relations &firstElement = posetToBeRemovedFrom[firstElementId];
        poset_relations &secondElement = posetToBeRemovedFrom[secondElementId];

        relations &firstElementRelations = firstElement.first;
        relations &secondElementTransposedRelations = secondElement.second;

        auto relationToBeRemovedIterator = firstElementRelations.find(secondElementId);
        auto transposedRelationToBeRemovedIterator = secondElementTransposedRelations.find(firstElementId);

        if (relationToBeRemovedIterator == firstElementRelations.end()) {
            DEBUG(": poset " << id << ", " << RELATION(value1, value2) << " cannot be deleted");

            return false;
        }
        assert(transposedRelationToBeRemovedIterator != secondElementTransposedRelations.end());

        for (poset_element_id i : firstElementRelations) {
            if (i == firstElementId || i == secondElementId) {
                continue;
            }

            auto tmpIterator = posetToBeRemovedFrom.find(i);
            relations tmpRelations = (tmpIterator->second).first;
            if (tmpRelations.find(secondElementId) != tmpRelations.end()) {
                DEBUG(": poset " << id << ", " << RELATION(value1, value2) << " cannot be deleted");

                return false;
            }
        }

        firstElementRelations.erase(relationToBeRemovedIterator);
        secondElementTransposedRelations.erase(transposedRelationToBeRemovedIterator);

        DEBUG(": poset " << id << ", " << RELATION(value1, value2) << " deleted");

        return true;
    }

    bool poset_test(unsigned long id, char const *value1, char const *value2) {
        DEBUG("(" << id << ", " << MAKE_STRING(value1) << ", " << MAKE_STRING(value2) << ")");
        if (value1 == nullptr || value2 == nullptr) {
            if (value1 == nullptr) {
                DEBUG(": " << INVALID_VALUE(value1));
            }
            if (value2 == nullptr) {
                DEBUG(": " << INVALID_VALUE(value2));
            }

            return false;
        }

        if (posets().find(id) == posets().end()) {
            DEBUG(": " << POSET_NOT_EXIST(id));

            return false;
        }
        poset posetToBeTested = posets()[id];

        poset_element_id firstElementId = get_poset_element_id(id, value1);
        if (firstElementId == INVALID_POSET_ELEMENT_ID) {
            DEBUG(": poset " << id << ", " << ELEMENT_NOT_EXIST(value1));

            return false;
        }

        poset_element_id secondElementId = get_poset_element_id(id, value2);
        if (secondElementId == INVALID_POSET_ELEMENT_ID) {
            DEBUG(": poset " << id << ", " << ELEMENT_NOT_EXIST(value2));

            return false;
        }

        assert(posetToBeTested.find(firstElementId) != posetToBeTested.end());
        poset_relations firstElement = posetToBeTested[firstElementId];


        assert(posetToBeTested.find(secondElementId) != posetToBeTested.end());

        relations firstElementRelations = firstElement.first;

        if (firstElementRelations.find(secondElementId) != firstElementRelations.end()) {
            DEBUG(": poset " << id << ", " << RELATION(value1, value2) << " exists");

            return true;
        } else {
            DEBUG(": poset " << id << ", " << RELATION(value1, value2) << " does not exist");

            return false;
        }
    }

    void poset_clear(unsigned long id) {
        DEBUG("(" << id << ")");

        if (posets().find(id) == posets().end()) {
            DEBUG(": " << POSET_NOT_EXIST(id));

            return;
        }

        assert(posetIdToMapOfNames().find(id) != posetIdToMapOfNames().end());

        unordered_map<string, poset_element_id> &namesToBeCleared = posetIdToMapOfNames()[id];
        poset &posetToBeCleared = posets()[id];

        namesToBeCleared.clear();
        posetToBeCleared.clear();

        DEBUG(": poset " << id << " cleared");
    }
}


