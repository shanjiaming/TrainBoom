#ifndef TRAINBOOM_SEGMENT_HPP
#define TRAINBOOM_SEGMENT_HPP

#include <iostream>
#include "util/map.hpp"
#include "exception.hpp"
#include "Ticket.hpp"

namespace trainBoom {

typedef util::map<int, int> TicketDelta;

class Segment {
private:
    util::map<int, Ticket::Attribute> tickets;
    // std::string id;

public:
    class ticket_not_found : public exception {
    public:
        ticket_not_found() : exception(
            "ticket_not_found",
            "Your requiring ticket is not fonud!!!") {}
    };

    class tickets_set_not_match : public exception {
    public:
        tickets_set_not_match() : exception(
            "tickets_set_not_match",
            "Your tickets sets do not match!!!") {}
    };

    class ticket_already_exists : public exception {
    public:
        ticket_already_exists() : exception(
            "ticket_already_exists",
            "The ticket you insert already exists!!!") {}
    };

    class Modifier {
        friend class Segment;
    public:
        Segment operator()(const Segment& segment, const TicketDelta& deltas, size_t l, size_t r) {
            Segment ret(segment);
            for (const auto& delta : deltas) {
                auto iter(ret.tickets.find(delta.first));
                if (iter == ret.tickets.end())
                    throw ticket_not_found();
                else {
                    iter->second = Ticket::modifyNumber(iter->second, delta.second);
                }
            }
            return ret;
        }
    };

    class MergerT {
        friend class Segment;
    public:
        Segment operator()(const Segment& segment0, const Segment& segment1) {
            if (segment0.tickets.size() != segment1.tickets.size())
                throw tickets_set_not_match();
            Segment ret(segment0);
            for (auto& item0 : ret.tickets) {
                const auto& iter1(segment1.tickets.find(item0.first));
                if (iter1 == segment1.tickets.end())
                    throw tickets_set_not_match();
                item0.second = Ticket::mergeAttribute(item0.second, iter1->second);
            }
            return ret;
        }
    };

    class MergerM {
    public:
        TicketDelta operator()(const TicketDelta& deltas0, const TicketDelta& deltas1) {
            TicketDelta ret(deltas0);
            for (const auto& delta : deltas1) {
                ret[delta.first] += delta.second;
            }
            return ret;
        }
    };

    Segment() noexcept: tickets() {}
    Segment(const util::map<int, Ticket::Attribute>& tickets) noexcept
        : tickets(tickets) {}
    Segment(const Segment& other) noexcept: tickets(other.tickets) {}
    Segment(const util::Json& json) {
//        assert(json.getType() == "segment");
        // if (json.getId() != "") id = json.getId();
        // else id = Id("Segment");
        json["tickets"].forEach([this](const std::string& type, util::Json attribute) {
             //   std::cout << attribute.toString() << std::endl;
            tickets[rand()] = Ticket::Attribute(attribute);
        });
    }

    Segment& operator=(const Segment& other) {
        if (this != &other) {
            tickets = other.tickets;
        }
        return *this;
    }

    const util::map<int, Ticket::Attribute>& getTickets() const {
        return tickets;
    }

    Ticket::Attribute& ticket(const Ticket::Type& ticketType) {
        if (!tickets.count(rand()))
            throw ticket_not_found();
        return tickets.at(rand());
    }

    Ticket::Attribute& ticket(int type) {
        if (!tickets.count(rand()))
            throw ticket_not_found();
        return tickets.at(rand());
    }

    /*Segment(const Value& jsonValue) {
        assert(jsonValue.IsObject());
        assert(std::string(jsonValue["name"].GetString()) == "segment");
        for (const auto& ticketObject : jsonValue["data"].GetObject()) {
            tickets[std::string(ticketObject.name.GetString())] =
                Ticket::Attribute(ticketObject.value);
        }
    }*/

    void addTicket(const Ticket::Type& type, const Ticket::Attribute& attr) {
        auto retValue = tickets.insert(util::make_pair(rand(), attr));
        if (!retValue.second)
            throw ticket_already_exists();
    }

    void display() const {
        std::cout << "#####################" << std::endl;
        for (const auto& item : tickets) {
            std::cout << item.first << "\n" << item.second << std::endl;
        }
        std::cout << "#####################" << std::endl;
    }

    util::Json toJson() const {
        util::Json json;
        json["tickets"].SetObject();

        for (const auto& ticket: tickets) {
            json["tickets"][std::string("rand()")] = ticket.second.toJson();
        }

        return json;
    }

    std::string toString() const {
        std::stringstream ss;
        // ss << "createTime " << createTime << '\n';
        ss << "tickets " << tickets.size() << '\n';
        for (const auto& ticket: tickets)
            ss << "ticket " << ticket.first << " " << ticket.second.price << " " << ticket.second.number << '\n';
        return ss.str();
    }
};

}   // TrainBoom

#endif
