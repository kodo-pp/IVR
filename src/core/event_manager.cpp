#include <modbox/core/event_manager.hpp>

EventHandler::EventHandler(
        uint64_t _id,
        const std::function<void(const std::unordered_map<std::string, std::string>&)>& _func)
        : id(_id), func(_func)
{
}

bool EventHandler::operator<(const EventHandler& other) const
{
    return id < other.id;
}

void EventHandler::operator()(const std::unordered_map<std::string, std::string>& args) const
{
    func(args);
}

uint64_t EventManager::addEventHandler(
        const std::string& event,
        const std::function<void(const std::unordered_map<std::string, std::string>&)>& handler)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    eventHandlers[event].emplace(current_id, handler);
    auto ret = current_id;
    ++current_id;
    return ret;
}

void EventManager::removeEventHandler(const std::string& event, uint64_t id)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    auto& handlers = eventHandlers.at(event);
    auto whatToLookFor = EventHandler(id,
                                      [](const std::unordered_map<std::string, std::string>&) {});
    auto iterator = handlers.lower_bound(whatToLookFor);
    handlers.erase(iterator);
}

void EventManager::removeAllEventHandlers(const std::string& event)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    eventHandlers.erase(event);
}

void EventManager::raiseEvent(const std::string& event,
                              const std::unordered_map<std::string, std::string>& args) const
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    for (const auto& handler : eventHandlers.at(event)) {
        handler(args);
    }
}

EventManager& getEventManager()
{
    static EventManager eventManager;
    return eventManager;
}
