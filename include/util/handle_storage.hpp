#ifndef UTIL_HANDLE_STORAGE_HPP
#define UTIL_HANDLE_STORAGE_HPP

#include <map>
#include <set>

// Located in header file, because it is a template
template <typename Handle, typename Value> class HandleStorage {
public:
    HandleStorage() = default;
    virtual ~HandleStorage() = default;

    HandleStorage(const HandleStorage<Handle, Value>& other) : storageMap(other.storageMap) {}

    HandleStorage(HandleStorage<Handle, Value>&& other) : storageMap(std::move(other.storageMap)) {}

    Handle insert(Value v) {
        Handle h = allocateHandle();
        storageMap.insert(std::make_pair(h, v));
        if (freeHandles.count(h)) {
            freeHandles.erase(h);
        }
        return h;
    }

    const Value& access(Handle h) { return storageMap.at(h); }

    Value& mutableAccess(Handle h) { return storageMap.at(h); }

    void remove(Handle h) {
        maybeAddFreeHandle(h);
        storageMap.erase(h);
    }

    auto begin() { return storageMap.begin(); }
    auto end() { return storageMap.end(); }
    auto rbegin() { return storageMap.rbegin(); }
    auto rend() { return storageMap.rend(); }

    auto cbegin() { return storageMap.cbegin(); }
    auto cend() { return storageMap.cend(); }
    auto crbegin() { return storageMap.crbegin(); }
    auto crend() { return storageMap.crend(); }

protected:
    void maybeAddFreeHandle(Handle h) {
        if (storageMap.size() <= 0) {
            throw std::logic_error("storageMap.size() <= 0, which isn't expected");
        }
        auto iter = storageMap.crbegin();
        Handle maxUsedHandle = iter->first;
        if (h < maxUsedHandle) {
            // If not last element was deleted
            freeHandles.insert(h);
        }
    }

    Handle allocateHandle() {
        if (freeHandles.empty()) {
            return Handle(storageMap.size());
        } else {
            return *freeHandles.begin();
        }
    }

    std::map<Handle, Value> storageMap;
    std::set<Handle> freeHandles;
};

#endif /* end of include guard: UTIL_HANDLE_STORAGE_HPP */
