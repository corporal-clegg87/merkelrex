#pragma once

#include <string>

enum class OrderBookType
{
    bid,
    ask,
    unknown,
    asksale,
    bidsale
};

class OrderBookEntry
{
public:
    /** Constructer for OrderBookEntry */
    OrderBookEntry(double _price,
                   double _amount,
                   std::string _timestamp,
                   std::string _product,
                   OrderBookType _orderType,
                   std::string username = "dataset");

    /** Returns an OrderBookType from a string */
    static OrderBookType stringToOrderBookType(std::string s);

    /** Returns true if e1.timestamp < e2.timestamp*/
    static bool compareByTimestamp(OrderBookEntry &e1, OrderBookEntry &e2)
    {
        return e1.timestamp < e2.timestamp;
    }
    /** Returns true if e1.price < e2.price*/
    static bool compareByPriceAsc(OrderBookEntry &e1, OrderBookEntry &e2)
    {
        return e1.price < e2.price;
    }

    /** Returns true if e1.price > e2.price*/
    static bool compareByPriceDesc(OrderBookEntry &e1, OrderBookEntry &e2)
    {
        return e1.price > e2.price;
    }

    double price;
    double amount;
    std::string timestamp;
    std::string product;
    OrderBookType orderType;
    std::string username;
};
