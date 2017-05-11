#ifndef TRAINBOOM_ORDER_HPP
#define TRAINBOOM_ORDER_HPP

#include <string>
#include "util/Json.hpp"
#include "Id.hpp"
#include "DataManager.hpp"

namespace trainBoom {
    struct Order {
        std::string routeId;
        std::string startStationId, endStationId;
        std::string ticketType;
        double ticketPrice;  // Total price.
        unsigned ticketNumber;

        std::string id;

        Order(std::string routeId,
                std::string startStationId, std::string endStationId,
                std::string ticketType,
                double ticketPrice, unsigned ticketNumber):
            routeId(routeId), startStationId(startStationId),
            endStationId(endStationId), ticketType(ticketType),
            ticketPrice(ticketPrice), ticketNumber(ticketNumber),
            id(Id("Order")) {}

        Order(const Json& json) {
            if (json.getId() != "") {
                id = json.getId();
            }
            else {
                id = Id("Order");
            }
            routeId = json["routeId"].as<std::string>();
            startStationId = json["startStationId"].as<std::string>();
            endStationId = json["endStationId"].as<std::string>();
            ticketType = json["ticketType"].as<std::string>();
            ticketPrice = json["ticketPrice"].as<double>();
            ticketNumber = json["ticketNumber"].as<unsigned>();
        }

        Order(std::string id, stupid_ptr<BinaryFile> bfp): Order(Json().read(id, bfp)) {}

        std::string getId() const {
            return id;
        }

        Json toJson() const {
            Json json("order", id);
            json["routeId"] = routeId;
            json["startStationId"] = startStationId;
            json["endStationId"] = endStationId;
            json["ticketType"] = ticketType;
            json["ticketPrice"] = ticketPrice;
            json["ticketNumber"] = ticketNumber;
            return json;
        }

        void save() const {
            toJson().write(DataManager::getFile(id));
        }
    };
}

#endif
