#ifndef CORE_EVENT_MANAGER_HPP
#define CORE_EVENT_MANAGER_HPP

#include <any>
#include <functional>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>

class EventHandler
{
public:
    EventHandler(
            uint64_t _id,
            const std::function<void(const std::unordered_map<std::string, std::string>&)>& _func);
    EventHandler(const EventHandler& other) = default;
    EventHandler(EventHandler&& other) = default;
    virtual ~EventHandler() = default;

    EventHandler& operator=(const EventHandler& other) = default;
    EventHandler& operator=(EventHandler&& other) = default;

    bool operator<(const EventHandler& other) const;

    void operator()(const std::unordered_map<std::string, std::string>& args) const;

private:
    uint64_t id;
    std::function<void(const std::unordered_map<std::string, std::string>&)> func;
};

class EventManager
{
public:
    EventManager() = default;
    EventManager(const EventManager& other) = delete;
    EventManager(EventManager&& other) = delete;
    virtual ~EventManager() = default;

    EventManager& operator=(const EventManager& other) = delete;
    EventManager& operator=(EventManager&& other) = delete;

    uint64_t addEventHandler(
            const std::string& event,
            const std::function<void(const std::unordered_map<std::string, std::string>&)>& handler);
    void removeEventHandler(const std::string& event, uint64_t id);
    void removeAllEventHandlers(const std::string& event);

    void raiseEvent(const std::string& event,
                    const std::unordered_map<std::string, std::string>& args = {}) const;

private:
    mutable std::recursive_mutex mutex;
    std::unordered_map<std::string, std::set<EventHandler>> eventHandlers;
    uint64_t current_id = 0;
};

EventManager& getEventManager();

#endif /* end of include guard: CORE_EVENT_MANAGER_HPP */
