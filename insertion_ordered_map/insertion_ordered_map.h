// Authors: Piotr Jasinski and Andrzej Malek

#ifndef INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_H

#include <memory>
#include <unordered_map>
#include <list>


// Lookup_error exception class.
class lookup_error : public std::exception {
public:
    virtual const char* what() const throw(){
        return "lookup_error";
    }
} exc;

// Insertion ordered map template.
template <class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map{
private:
    // Support class. Implements all operations without copy-on-write.
    class implementation{
    private:
        // Class implementing hashing operator().
        class HashPtr{
        public:
            auto operator() (const K *key) const noexcept {
                return Hash()(*key);
            }
        };

        // Class implementing equal operator() for K* .
        class PtrCmp{
        public:
            bool operator()(const K *key1, const K *key2) const {
                return *key1 == *key2;
            }
        };

    public:
        // Implementation contail list o <key, value> pairs and map for each
        // key pointer k provides iterator to list at position with key k.
        using LinkedList = std::list<std::pair<K, V>>;
        using HashMap = std::unordered_map<const K*, typename LinkedList::iterator,
        HashPtr, PtrCmp>;

        HashMap map;
        LinkedList insertion_order_list;

        // Default implementation constructor.
        implementation() = default;

        // Implementation copy constructor.
        implementation(const implementation &other): insertion_order_list(other.insertion_order_list){
            for (auto it = insertion_order_list.begin();
                 it != insertion_order_list.end(); ++it){
                map.insert({&it->first, it});
            }
        }

        // Default implementation move constructor.
        implementation(implementation &&other) noexcept = default;

        // Inserting pair <k,v>. Returns true only if k didn't exist in map
        // and inserting was made succesfully.
        bool insert(K const &k, V const &v){
            auto it = map.find(&k);
            if (it != map.end()){
                auto itl = it->second;
                insertion_order_list.splice(insertion_order_list.end(),
                                            insertion_order_list, itl);

                return false;
            }
            bool list_inserted = false;
            auto itl = insertion_order_list.insert(insertion_order_list.end(), {k, v});
            try {
                list_inserted = true;
                auto p = map.insert({&itl->first, itl});
                return p.second;
            }
            catch (...){
                if (list_inserted)
                    insertion_order_list.pop_back();
                throw;
            }
        }

        // Erasing given element or throws exception if doesn't exist.
        void erase(K const &k){
            auto it = map.find(&k);
            if (it == map.end())
                throw exc;
            map.erase(it);
            insertion_order_list.erase(it->second);
        }

        // Merging, by inserting all elements.
        void merge(const implementation &other){
            for (const auto &it: other.insertion_order_list)
                insert(it.first, it.second);
        }

        // Giving reference.
        V &at(K const &k){
            auto it = map.find(&k);
            if (it == map.end()){
                throw exc;
            }
            return it->second->second;
        }

        // Giving reference to value under given key.
        V &operator[](K const &k){
            auto it = map.find(&k);
            if (it != map.end()){
                return it->second->second;
            }
            bool list_inserted = false;
            try {
                auto itl = insertion_order_list.insert(insertion_order_list.end(), {k, V()});
                list_inserted = true;
                map.insert({&itl->first, itl});
                return itl->second;
            }
            catch (...){
                if (list_inserted)
                    insertion_order_list.pop_back();
                throw;
            }
        }

        [[nodiscard]] size_t size() const noexcept{
            return map.size();
        }

        [[nodiscard]] bool empty() const noexcept{
            return map.empty();
        }

        // Clearing structure.
        void clear() noexcept{
            map.clear();
            insertion_order_list.clear();
        }

        bool contains(K const &k) const{
            return map.find(&k) != map.end();
        }
    };

    // Flag shareable inform that structure can be shared, doesn't need
    // to be copied. Imp -> pointer to implementation.
    using dataPtr = std::shared_ptr<implementation>;
    bool shareable;
    dataPtr imp;

    // Class that copy implementation if necessary and perform changes only in
    // case of operation success.
    class Guard {
    private:
        bool rollBack;
        dataPtr *orginal;
        dataPtr copy;
    public:
        explicit Guard(dataPtr &data): orginal(&data), copy(data){
            if (data == nullptr){
                data = std::make_shared<implementation>();
                rollBack = true;
            }
            else if (data.use_count() > 2){
                data = std::make_shared<implementation>(*data);
                rollBack = true;
            }
            else {
                rollBack = false;
            }
        }

        void succes() noexcept{
            rollBack = false;
        }

        ~Guard() noexcept{
            if (rollBack)
                *orginal = std::move(copy);
        }
    };

public:
    using iterator = typename implementation::LinkedList::const_iterator;

    // Default constructor.
    insertion_ordered_map(){
        imp = std::make_shared<implementation>();
        shareable = true;
    }

    // Copy constructor.
    insertion_ordered_map(const insertion_ordered_map &other){
        shareable = true;
        if (other.shareable){
            imp = other.imp;
        } else {
            imp = std::make_shared<implementation>(*other.imp);
        }
    }

    // Default move constructor.
    insertion_ordered_map(insertion_ordered_map &&other) noexcept = default;

    // Assign operator.
    insertion_ordered_map &operator=(insertion_ordered_map other){
        shareable = other.shareable;
        imp = std::move(other.imp);
        return *this;
    }

    // Insert with copy-on-write semantic.
    bool insert(K const &k, V const &v){
        Guard guard(imp);
        bool res = (*imp).insert(k, v);
        guard.succes();
        shareable = true;
        return res;
    }

    // Erase with copy-on-write semantic.
    void erase(K const &k){
        Guard guard(imp);
        (*imp).erase(k);
        shareable = true;
        guard.succes();
    }

    // Merge with copy-on-write semantic.
    void merge(insertion_ordered_map &other){
        if (other.imp == imp)
            return;
        Guard guard(imp);
        (*imp).merge(*(other.imp));
        shareable = true;
        guard.succes();
    }

    // Giving reference with copy-on-write semantic.
    V &at(K const &k){
        Guard guard(imp);
        V &res = (*imp).at(k);
        shareable = false;
        guard.succes();
        return res;
    }

    // Giving const reference with copy-on-write semantic.
    V const &at(K const &k) const{
        if (imp == nullptr)
            throw exc;
        return (*imp).at(k);
    }

    // Giving reference to value under key with copy-on-write semantic.
    V &operator[](K const &k){
        Guard guard(imp);
        V &res = (*imp)[k];
        shareable = false;
        guard.succes();
        return res;
    }

    // Returning map size.
    size_t size() const noexcept{
        if (imp == nullptr)
            return 0;
        return (*imp).size();
    }

    // Checks if map is empty.
    bool empty() const noexcept{
        if (imp == nullptr)
            return true;
        return (*imp).empty();
    }

    // Clearing map.
    void clear() noexcept{
        if (imp == nullptr)
            return;
        else if (imp.use_count() > 1)
            imp.reset();
        else
            (*imp).clear();
        shareable = true;
    }

    // Checks if key is presented in map.
    bool contains(K const &k) const{
        if (imp == nullptr)
            return false;
        return (*imp).contains(k);
    }

    // Giving iterator to beginnig of map.
    iterator begin() const noexcept{
        if (imp == nullptr)
            return iterator();
        return imp->insertion_order_list.begin();
    }

    // Giving iterator to end of map.
    iterator end() const noexcept{
        if (imp == nullptr)
            return iterator();
        return imp->insertion_order_list.end();
    }

};

#endif // INSERTION_ORDERED_MAP_H

