#pragma once
#include <vector>
#include <stack>
#include <functional>
#include <thread>

namespace pk {
    template <typename... A> class EventPort;
    template <typename... A> class EventLink;

    template <typename... A> class Event {
        using elink = EventLink<A...>;
        using eport = EventPort<A...>;
        friend elink;
        friend eport;

        private:
            int active_invokes = 0;
            std::stack<elink*> stack;
            std::vector<elink*> links;
        public:
            std::function<elink*(std::function<void(A...)>)> filter = nullptr;

            EventPort<A...> port{this};

            void invoke(A&... items) { 
                active_invokes++;
                for (elink* link : links) { link->callback(items...); }
                active_invokes--;
                if (0 == active_invokes) {
                    while (!stack.empty()) {
                        links.push_back(stack.top());
                        stack.pop();
                    }
                }
            }

            void invoke_async(A&... items) {
                std::thread([this, items...](){ invoke(items...); }).detach();
            }

            void lock(A&... items) {
                invoke(items...);
                filter = [items...](auto cb){ cb(items...); return nullptr; };
            }

            void unlock() { filter = nullptr; }

            void clear() { 
                for (elink* link : links) link->disconnect();
                links.clear();

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
            [[nodiscard]] elink connect(std::function<void(A...)> callback) const {
                if (event->filter) { return event->filter(callback); }

                elink* link = new elink(event);

                if (event->active_invokes) { event->stack.push(link); } else { event->links.push_back(link); }
                return *link;
            };
    };

    template <typename... T> struct EventLink {
        friend Event<T...>;
        friend EventPort<T...>;
        private:   
            Event<T...>* event;
            unsigned short ref;
            std::function<void(T...)> call;
            EventLink(Event<T...>* ev, unsigned short I, std::function<void(T...)> C): event(ev), ref(I), call(C) {};
        public:
            void disconnect() {
                if (event) {
                    auto& links = event->links;
                    links.back()->ref = ref;
                    links[ref] = links.back();
                    links.pop_back();
                    event = nullptr;
                }
            };

            ~EventLink() { disconnect(); }

            EventLink() = delete;
    };
}