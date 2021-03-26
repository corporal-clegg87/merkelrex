#pragma once
#include "OrderBookEntry.hpp"
#include "CSVReader.hpp"
#include <string>
#include <vector>
#include <list>
#include <iterator>

class OrderBook
{
public:
    /** construct, reading a csv data file */
    OrderBook(std::string filename);
    /** return vector of all know products in the dataset*/
    std::vector<std::string> getKnownProducts();

    /** return vector of Orders according to the sent filters*/
    std::vector<OrderBookEntry> getOrders(OrderBookType type,
                                          std::string product,
                                          std::string timestamp);

    /** returns the earliest time in the orderbook*/
    std::string getEarliestTime();

    /** returns the final time in the orderbook*/
    std::string getFinalTime();

    /** returns the next time after timestamp. 
         * If there is no next timestamp, wraps around to the start
         * */
    std::string getNextTime(std::string timestamp);

    /** Inserts a new order into appropriate place in the orderbook */
    void insertOrder(OrderBookEntry &order);

    /** Matches asks to bids. Only called if user placed an order. Returns matched orders */
    std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);

    /** Retunrs the time before timestamp */
    std::string getPreviousTimestamp(std::string timestamp);

    /** Returns the highest price in orders */
    double getHighPrice(std::list<OrderBookEntry> &orders);

    /** Returns the lowest price in orders */
    double getLowPrice(std::list<OrderBookEntry> &orders);

    /** Removes all orders from simuser */
    void removeOldOrders();

private:
    std::list<OrderBookEntry> orders;

    /** Returns the index of the first appearance of the timestamp */
    int getIndex(std::string insertTimestamp);
};
