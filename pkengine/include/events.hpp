#pragma once
#include <vector>
#include <functional>

// user MUSTST make sure to delete their 
namespace pk {
    template <typename... A> class EventPort;
    template <typename... A> class EventLink;

    template <typename... A> class Event {
        using elink = EventLink<A...>;
        using eport = EventPort<A...>;
        friend elink;
        friend eport;

        private:
            std::vector<elink*> links;
        public:
            std::function<elink*(std::function<void(A...)>)> filter = nullptr;
            EventPort<A...> port{this};

            void invoke(A&... items) { 
                auto links_copy = links;
                for (elink* link : links_copy) { link->call(items...); }
            }

            void lock(A&... items) {
                invoke(items...);
                filter = [items...](auto cb){ cb(items...); return new elink{nullptr}; };
            }

            ~Event() { for (elink* link : links) link->event = nullptr; }
    };

    template <typename... A> class EventPort {
        using elink = EventLink<A...>;
        friend Event<A...>;
        Event<A...>* event;
        public:
            EventPort(Event<A...>* ev): event(ev) {}
            EventPort() = delete;

            elink* connect(std::function<void(A...)> callback) const {
                if (event->filter) { return event->filter(callback); }

                elink* link = new elink(event, event->links.size(), callback);
                event->links.push_back(link);
                return link;
            };

            // cool pk::engine::window::began << [](){} sybtax...
            elink* operator<<(std::function<void(A...)> callback) const { return connect(callback); }
    };

    template <typename... A> struct EventLink {
        friend Event<A...>;
        friend EventPort<A...>;
        private:   
            Event<A...>* event;
            unsigned short ref;
            std::function<void(A...)> call;
            EventLink(Event<A...>* ev): event(ev) {}
            EventLink(Event<A...>* ev, unsigned short I, std::function<void(A...)> C): event(ev), ref(I), call(C) {};
        public:
            void disconnect() {
                if (!event) return;
                auto& links = event->links;
                links.back()->ref = ref;
                links[ref] = links.back();
                links.pop_back();
                event = nullptr;
            };

            ~EventLink() { 
                disconnect();
             };

            EventLink() = delete;
    };
}