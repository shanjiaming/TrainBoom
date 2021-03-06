#ifndef TRAINBOOM_ROUTE_HPP
#define TRAINBOOM_ROUTE_HPP

#include "util/Json.hpp"
#include "util/stupid_ptr.hpp"
#include "util/IntervalManip.hpp"
#include "DataManager2.hpp"
#include "Order.hpp"
#include "Segment.hpp"
#include "Information.hpp"
#include <iostream>
#include <sstream>
#include <string>

namespace trainBoom {

/*
typedef util::IntervalManip<
    Segment,
    TicketDelta,
    Segment::Modifier,
    Segment::MergerT,
    Segment::MergerM
> SegmentsInvervalManip;*/

class Information;

class Route {
public:
    static const Datetime startDate;
    static const int lastDays = 31;
private:
    std::string name;

    unsigned n;

    // Datetime startDate;
    util::stupid_array<util::stupid_ptr<Information>> informations;

    util::stupid_array<util::stupid_ptr<Segment>> segments[lastDays];
    // util::stupid_ptr<SegmentsInvervalManip> segmentsIntervalManip[lastDays];

    std::string id;

    bool running;

    bool selling[lastDays];

    bool beenSold;

public:

    class station_number_too_small : public exception {
    public:
    	station_number_too_small() : exception(
    		"station_number_too_small",
    		"Your station number is too small!!!") {}
    };

    class information_missing : public exception {
    public:
    	information_missing(const std::string& info) : exception(
    		"information_missin",
    		"Your " + info + " is missing!!!") {}
    };

    class station_number_not_consistent : public exception {
    public:
    	station_number_not_consistent() : exception(
    		"station_number_not_consistent",
    		"Your station number is not consistent among all information!!!") {}
    };

    class last_days_not_consistent : public exception {
    public:
    	last_days_not_consistent() : exception(
    		"last_days_not_consistent",
    		"Your last days is not " + std::to_string(lastDays) + "!!!") {}
    };

    class station_not_found : public exception {
    public:
    	station_not_found() : exception(
    		"station_not_found",
    		"Your station is not found!!!") {}
    };

    class index_out_of_range : public exception {
    public:
    	index_out_of_range() : exception(
    		"index_out_of_range",
    		"Your index is out of range!!!") {}
    };

    class interval_invalid : public exception {
    public:
    	interval_invalid() : exception(
    		"interval_invalid",
    		"Your requested interval is not valid!!!") {}
    };

    class not_enough_tickets_left: public exception {
    public:
    	not_enough_tickets_left() : exception(
    		"not_enough_tickets_left",
    		"Tickets left are not enough for you to book!!!") {}
    };

    class nonstop_station: public exception {
    public:
        nonstop_station(std::string pos, std::string type): exception (
            "nontstop_station",
            "Your " + type + " in " + pos + " is nonstop!!!"
        ) {}
    };

    class already_selling: public exception {
    public:
        already_selling(Datetime date): exception (
            "already_selling",
            "Tickets on " + date.format() + " are already selling!!!"
        ) {}
    };

    class not_selling: public exception {
    public:
        not_selling(Datetime date): exception (
            "not_selling",
            "Tickets on " + date.format() + " are not selling!!!"
        ) {}
    };

    class date_out_of_bound: public exception {
    public:
        date_out_of_bound(Datetime date): exception (
            "date_out_of_bound",
            date.format() + " is out of bound!!!"
        ) {}
    };

    explicit Route(): id(Id("Route")), running(false) {}

    /*Route(Id id, unsigned n,
        const util::stupid_array<Id>& stations,
        const util::stupid_array<unsigned>& distance,
        const util::stupid_array<util::Datetime::Datetime>& arriveTime,
        const util::stupid_array<util::Datetime::Datetime>& leaveTime,
        const util::stupid_array<Segment>& segments
    ): id(id), n(n), stations(stations), distance(distance),
        arriveTime(arriveTime), leaveTime(leaveTime), segments(segments) {
            if (n < 2) {
                throw station_number_too_small();
            }
            if (stations.size() != n || distance.size() != n - 1 ||
                arriveTime.size() != n - 1 || leaveTime.size() != n - 1 ||
                segments.size() != n - 1) {
                    throw station_number_not_consistent();
                }

            for (unsigned i = 0; i < n; ++ i)
                stationsMap[stations[i]] = i;

            segmentsIntervalManip = new SegmentsInvervalManip(
                segments, n - 1
            );
        }*/

    Route(std::string name, unsigned n,
        const util::stupid_array<util::stupid_ptr<Information>>& informations,
        const util::stupid_array<util::stupid_ptr<Segment>>& _segments
    ): name(name), n(n), informations(informations), id(Id("Route")), running(false), beenSold(false) {
            if (n < 2) {
                throw station_number_too_small();
            }
            if (informations.size() != n || _segments.size() != n - 1) {
                throw station_number_not_consistent();
            }
            // Datetime curDate = startDate;
            for (int i = 0; i < lastDays; ++ i) {
                stupid_array<stupid_ptr<Segment>> cur_segments(new stupid_ptr<Segment>[n - 1], n - 1);
                for (unsigned j = 0; j < n - 1; ++ j)
                    cur_segments[j] = make_stupid<Segment>(*(_segments[j]));
                segments[i] = cur_segments;
                // segmentsIntervalManip[i] = make_stupid<SegmentsInvervalManip>(cur_segments, n - 1);
                selling[i] = true;
                // segments.insert(util::make_pair(curDate, cur_segments));
                // segmentsIntervalManip.insert(util::make_pair(curDate, make_stupid<SegmentsInvervalManip>(cur_segments, n - 1)));
                // selling.insert(util::make_pair(curDate, true));
                // segmentsIntervalManip = new SegmentsInvervalManip(
                //     segments, n - 1
                // );
                // curDate = curDate.incDay();
            }
        }

    Route(const Json& json) {
        // std::cout << "here!" << std::endl;
        if (json.getId() == "") {
            id = Id("Route");
        }
        else {
            id = json.getId();
        }

        if (!json.HasMember("n")) throw information_missing("n");
        // if (!json.HasMember("startDate")) throw information_missing("startDate");
        if (!json.HasMember("name")) throw information_missing("name");
        if (!json.HasMember("informations")) throw information_missing("informations");
        // if (!json.HasMember("segments")) throw information_missing("segments");

        n = json["n"].as<unsigned>();
        if (n < 2) {
            throw station_number_too_small();
        }
        name = json["name"].as<std::string>();
        running = false;
        if (json.HasMember("running"))
            running = json["running"].as<bool>();
        if (json["informations"].Size() != n) {
            throw station_number_not_consistent();
        }
        informations = stupid_array<stupid_ptr<Information>>(new stupid_ptr<Information>[n], n);
        // segments = stupid_array<stupid_ptr<Segment>>(new stupid_ptr<Segment>[n - 1], n - 1);
        for (unsigned i = 0; i < n; ++ i) {
            informations[i] = make_stupid<Information>(json["informations"][i]);
        }

        // std::cout << "!" << std::endl;
        // std::cout << Json(json["segments"]).toString() << std::endl;
        // std::cout << json["segments"].getValue().IsObject() << std::endl;
        if (json.HasMember("dateSegments")) {
            if (json["dateSegments"].Size() != lastDays)
                throw last_days_not_consistent();
            for (int i = 0; i < lastDays; ++ i) {
                Json tmp = json["dateSegments"][i];
                // std::cout << json.toString() << std::endl;
                if (tmp.Size() != n - 1)
                    throw station_number_not_consistent();
                // std::cout << date << std::endl;
                auto _segments = stupid_array<stupid_ptr<Segment>>(new stupid_ptr<Segment>[n - 1], n - 1);
                for (unsigned i = 0; i < n - 1; ++ i) {
                    _segments[i] = make_stupid<Segment>(tmp[i]);
                }
                segments[i] = _segments;
            }
        }
        else {
            if (!json.HasMember("segments")) throw information_missing("segments");
            if (json["segments"].Size() != n - 1)
                throw station_number_not_consistent();
            for (int i = 0; i < lastDays; ++ i) {
                auto _segments = stupid_array<stupid_ptr<Segment>>(new stupid_ptr<Segment>[n - 1], n - 1);
                for (unsigned j = 0; j < n - 1; ++ j) {
                    _segments[j] = make_stupid<Segment>(json["segments"][j]);
                }
                segments[i] = _segments;
            }
        }

        if (json.HasMember("selling")) {
            if (json["selling"].Size() != lastDays)
                throw last_days_not_consistent();
            // std::cout << Json(json["selling"]).toString() << std::endl;
            // Datetime curDate = startDate;
            for (int i = 0; i < lastDays; ++ i) {
                // std::cout << curDate << " " << json["selling"][curDate.format()].as<bool>() << std::endl;
                selling[i] = json["selling"][i].as<bool>();
                // selling.insert(util::make_pair(curDate, json["selling"][curDate.format()].as<bool>()));
            }
        }
        else {
            for (int i = 0; i < lastDays; ++ i) {
                selling[i] = true;
                // selling.insert(util::make_pair(curDate, true));
            }
        }

        if (json.HasMember("beenSold")) {
            beenSold = json["beenSold"].as<bool>();
        }
        else {
            beenSold = false;
        }
    }

    // Route(std::string id, stupid_ptr<BinaryFile> bfp): Route(Json().read(id, bfp)) {}
    static Route load(std::string id) {
        return Route(DataManager::getJson(id));
    }

    /*void rebuild(unsigned _n,
        const util::stupid_array<Id>& _stations,
        const util::stupid_array<unsigned>& _distance,
        const util::stupid_array<util::Datetime::Datetime>& _arriveTime,
        const util::stupid_array<util::Datetime::Datetime>& _leaveTime,
        const util::stupid_array<Segment>& _segments) {
            if (_n < 2) {
                throw station_number_too_small();
            }
            if (_stations.size() != n || _distance.size() != n - 1 ||
                _arriveTime.size() != n - 1 || _leaveTime.size() != n - 1 ||
                _segments.size() != n - 1) {
                    throw station_number_not_consistent();
                }

            n = _n;
            stations = _stations;
            distance = _distance;
            arriveTime = _arriveTime;
            leaveTime = _leaveTime;
            segments = _segments;

            stationsMap.clear();
            for (unsigned i = 0; i < n; ++ i)
                stationsMap[stations[i]] = i;

            segmentsIntervalManip = new SegmentsInvervalManip(
                segments, n - 1
            );
        }*/

    std::string getId() const {
        return id;
    }

    std::string getName() const {
        return name;
    }

    unsigned size() const {
        return n;
    }

    bool isNonstop(int date, unsigned index, std::string ticketType) {
        if (index == 0) return false;
        return segments[date][index - 1]->ticket(ticketType).nonstop;
    }

    int getIDate(Datetime& date, unsigned l) {
        Duration dayShift = informations[l]->getLeaveTime().setToDay();
        date = date - dayShift;
        if (date < startDate)
            throw not_selling(date);
        int idate = (date - startDate).countDay();
        if (idate < 0 || idate >= lastDays)
            throw date_out_of_bound(date);
        if (!selling[idate]) {
            throw not_selling(date);
        }
        return idate;
    }

    Segment queryTickets(Datetime date, unsigned l, unsigned r) {
        int idate = getIDate(date, l);
        auto segment = *(segments[idate][l]);
        for (unsigned i = l + 1; i < r; ++ i) {
            segment = segment + *(segments[idate][i]);
        }
        for (auto& item: segment.getTickets()) {
            item.second.nonstop = isNonstop(idate, l, item.first) || isNonstop(idate, r, item.first);
        }
        return segment;
    }

    Segment queryTickets(int idate, unsigned l, unsigned r) {
        auto segment = *(segments[idate][l]);
        for (unsigned i = l + 1; i < r; ++ i) {
            segment = segment + *(segments[idate][i]);
        }
        for (auto& item: segment.getTickets()) {
            item.second.nonstop = isNonstop(idate, l, item.first) || isNonstop(idate, r, item.first);
        }
        return segment;
    }

    void modifyTickets(Datetime date, unsigned l, unsigned r, const TicketDelta& deltas) {
        // auto interval = getInterval(startStation, endStation);
        int idate = getIDate(date, l);
        for (unsigned i = l; i < r; ++ i) {
            for (const auto& delta: deltas) {
                segments[idate][i]->ticket(delta.first).number += delta.second;
            }
        }
        // segmentsIntervalManip[(date - startDate).countDay()]->modify(l, r - 1, deltas);
    }

    void modifyTickets(int idate, unsigned l, unsigned r, const TicketDelta& deltas) {
        // auto interval = getInterval(startStation, endStation);
        for (unsigned i = l; i < r; ++ i) {
            for (const auto& delta: deltas) {
                segments[idate][i]->ticket(delta.first).number += delta.second;
            }
        }
        // segmentsIntervalManip[(date - startDate).countDay()]->modify(l, r - 1, deltas);
    }

    Order bookTickets(Datetime date, unsigned l, unsigned r, const std::string& ticketType, unsigned ticketNumber) {
        int idate = getIDate(date, l);
        if (isNonstop(idate, l, ticketType))
            throw nonstop_station("start station", ticketType);
        if (isNonstop(idate, r, ticketType))
            throw nonstop_station("end station", ticketType);
        Segment segment = queryTickets(idate, l, r);
        if (segment.ticket(ticketType).number < ticketNumber)
            throw not_enough_tickets_left();
        Order order(RouteInterval(id, name, l, r), informations[l]->getStationName(), informations[r]->getStationName(), ticketType, segment.ticket(ticketType).price * ticketNumber, ticketNumber, date);
        TicketDelta ticketDelta;
        ticketDelta[ticketType] = - (int)ticketNumber;
        modifyTickets(idate, l, r, ticketDelta);
        beenSold = true;
        // segmentsIntervalManip[idate]->modify(l, r - 1, ticketDelta);
        return order;
    }

    void refundTickets(Datetime date, unsigned l, unsigned r, const std::string& ticketType, unsigned ticketNumber) {
        int idate = getIDate(date, l);
        // std::cout << idate << std::endl;
        if (isNonstop(idate, l, ticketType))
            throw nonstop_station("start station", ticketType);
        if (isNonstop(idate, r, ticketType))
            throw nonstop_station("end station", ticketType);
        TicketDelta ticketDelta;
        ticketDelta[ticketType] = (int)ticketNumber;
        modifyTickets(idate, l, r, ticketDelta);
    }

    void startSelling(Datetime date) {
        int idate = (date - startDate).countDay();
        if (selling[idate])
            throw already_selling(date);
        selling[idate] = true;
    }

    void stopSelling(Datetime date) {
        int idate = (date - startDate).countDay();
        if (!selling[idate])
            throw not_selling(date);
        selling[idate] = false;
    }

    Information& information(unsigned pos) {
        if (pos >= n) {
            throw index_out_of_range();
        }
        return *(informations[pos]);
    }

    Information information(unsigned pos) const {
        if (pos >= n) {
            throw index_out_of_range();
        }
        return *(informations[pos]);
    }

    // util::Datetime::Datetime getRunningDay() const noexcept {
    //     return informations[0]->getLeaveTime().clearTime();
    // }
    //
    // void display() {
    //     segmentsIntervalManip->forceApply();
    //     std::cout << "\n----\n" << std::endl;
    //     std::cout << "id: " << id << std::endl;
    //     std::cout << "nStation: " << n << std::endl;
    //     for (unsigned i = 0; i < n; ++ i) {
    //         std::cout << "\nStation #" << i << ": " << std::endl;
    //         informations[i]->display();
    //         if (i < n - 1) {
    //             std::cout << "\tTicket information: \n" << std::endl;
    //             segments[i]->display();
    //             // std::cout << "#####################" << std::endl;
    //         }
    //     }
    //     std::cout << "\n---\n" << std::endl;
    // }

    bool getRunning() const {
        return running;
    }

    void setRunning(bool x) {
        running = x;
    }

    bool getBeenSold() const {
        return beenSold;
    }

    util::Json toJson(bool simple = false) {
        // segmentsIntervalManip->forceApply();
        util::Json json("route", id);

        json["name"] = name;
        json["n"] = n;
        // json["startDate"] = startDate.format();
        json["running"] = running;
        json["informations"].SetArray();
        json["dateSegments"].SetArray();
        json["selling"].SetArray();
        // json["stationsMap"].SetObject();

        for (unsigned int i = 0; i < n; ++ i) {
            json["informations"].PushBack(informations[i]->toJson());
            // std::cout << "!!asdasd!" << std::endl;
            // if (i + 1 < n) {
            //     for (const auto& _segments: segments) {
            //         json["segments"][_segments.first].SetArray();
            //         json["segments"][_segments.first].PushBack(_segments.second[i]->toJson());
            //     }
            // }
            // std::cout << "!!asdasd!" << i << std::endl;
        }

        for (int j = 0; j < lastDays; ++ j) {
            // std::cout << "!" << std::endl;
            // segmentsIntervalManip[j]->forceApply();
            Json tmp; tmp.getData().SetArray();
            for (unsigned int i = 0; i < n - 1; ++ i) {
                if (!simple)
                    tmp.getData().PushBack(segments[j][i]->toJson());
                else
                    tmp.getData().PushBack(segments[j][i]->toJsonSimp());
            }
            json["dateSegments"].PushBack(tmp);
            json["selling"].PushBack(selling[j]);
        }

        json["beenSold"] = beenSold;

        /* for (const auto& item: stationsMap) {
            json["stationsMap"][item.first] = item.second;
        } */

        return json;
    }

    void save() {
        // std::cout << id << std::endl;
        // toJson().write(DataManager::getFile(id));
        DataManager::save(toJson(true));
    }

    // std::string toString() {
    //     segmentsIntervalManip->forceApply();
    //     std::stringstream ss;
    //     ss << "name " << name << '\n';
    //     ss << "n " << n << '\n';
    //     ss << "informations " << n << '\n';
    //     for (unsigned i = 0; i < n; ++ i)
    //         ss << "information " << informations[i]->getId() << '\n';
    //     ss << "segments " << n - 1 << '\n';
    //     for (unsigned i = 0; i < n - 1; ++ i)
    //         ss << "segment " << segments[i]->getId() << '\n';
    //     return ss.str();
    // }
};

const Datetime Route::startDate = Datetime(2017, 3, 28);

}   // TrainBoom

#endif
