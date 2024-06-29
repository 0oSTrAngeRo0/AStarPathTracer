#ifndef PATHTRACER_EVENT_REGISTRY_H
#define PATHTRACER_EVENT_REGISTRY_H

#include <vector>

template<typename TEvent>
class IEventHandler {
public:
    virtual void OnExecute(TEvent e) = 0;
};

template<typename TEvent>
class EventRegistry {
private:
    static std::vector<IEventHandler<TEvent> *> handlers;
public:
    static void Register(IEventHandler<TEvent> *handler);
    static void Unregister(IEventHandler<TEvent> *handler);
    static void Broadcast(TEvent event);
};

template<typename TEvent>
std::vector<IEventHandler<TEvent> *> EventRegistry<TEvent>::handlers;

template<typename TEvent>
void EventRegistry<TEvent>::Register(IEventHandler<TEvent> *handler) {
    handlers.push_back(handler);
}

template<typename TEvent>
void EventRegistry<TEvent>::Broadcast(TEvent event) {
    for (IEventHandler<TEvent> *handler: handlers) {
        handler->OnExecute(event);
    }
}

template<typename TEvent>
void EventRegistry<TEvent>::Unregister(IEventHandler<TEvent> *handler) {
    for (int i = 0, end = handlers.size(); i < end; ++i) {
        if(handlers[i] != handler) continue;
        handlers[i] = handlers[end - 1];
        handlers.pop_back();
    }
}

#endif //PATHTRACER_EVENT_REGISTRY_H
