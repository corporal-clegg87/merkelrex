#include "OrderBook.hpp"
#include "CSVReader.hpp"
#include <map>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <chrono>

/** construct, reading a csv data file */
OrderBook::OrderBook(std::string filename)
{
    orders = CSVReader::readCSV(filename);
}

/** return vector of all know products in the dataset*/
std::vector<std::string> OrderBook::getKnownProducts()
{
    std::vector<std::string> products;

    std::map<std::string, bool> prodMap;

    for (OrderBookEntry &e : orders)
    {
        prodMap[e.product] = true;
    }

    // now flatten the map to a vector of strings
    for (auto const &e : prodMap)
    {
        products.push_back(e.first);
    }

    return products;
}

std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type,
                                                 std::string product,
                                                 std::string timestamp)
{
    std::vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry &e : orders)
    {
        if (e.orderType == type &&
            e.product == product &&
            e.timestamp == timestamp)
        {
            orders_sub.push_back(e);
        }
    }

    return orders_sub;
}

std::string OrderBook::getPreviousTimestamp(std::string timestamp)
{
    int n = getIndex(timestamp);
    std::list<OrderBookEntry>::const_iterator it = orders.cbegin();
    std::advance(it, n - 1);

    return it->timestamp;
}

double OrderBook::getHighPrice(std::list<OrderBookEntry> &orders)
{
    double max = orders.front().price;
    for (OrderBookEntry &e : orders)
    {
        if (e.price > max)
            max = e.price;
    }

    return max;
}

double OrderBook::getLowPrice(std::list<OrderBookEntry> &orders)
{
    double min = orders.front().price;
    for (OrderBookEntry &e : orders)
    {
        if (e.price < min)
            min = e.price;
    }

    return min;
}

std::string OrderBook::getEarliestTime()
{

    return orders.front().timestamp;
}

std::string OrderBook::getFinalTime()
{

    return orders.back().timestamp;
}

std::string OrderBook::getNextTime(std::string timestamp)
{
    std::string next_timestamp = "";
    for (OrderBookEntry &e : orders)
    {
        if (e.timestamp > timestamp)
        {
            next_timestamp = e.timestamp;
            break;
        }
    }
    if (next_timestamp == "")
    {
        next_timestamp = orders.front().timestamp;
    }

    return next_timestamp;
}

void OrderBook::insertOrder(OrderBookEntry &order)
{
    std::list<OrderBookEntry>::const_iterator it2 = orders.cbegin();
    std::advance(it2, getIndex(order.timestamp));
    orders.insert(it2, order);
}

void OrderBook::removeOldOrders()
{
    int counter = 0;
    for (auto &e : orders)
    {

        if ((e.orderType == OrderBookType::bidsale) || (e.orderType == OrderBookType::asksale))
        {
            std::list<OrderBookEntry>::const_iterator it = orders.cbegin();
            std::advance(it, counter);
            orders.erase(it);
        }

        counter++;
    }
}

std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask,
                                                 product,
                                                 timestamp);
    // bids = orderbook.bids
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid,
                                                 product,
                                                 timestamp);

    std::vector<OrderBookEntry> sales;

    // I put in a little check to ensure we have bids and asks
    // to process.
    if (asks.size() == 0 || bids.size() == 0)
    {
        //std::cout << " OrderBook::matchAsksToBids no bids or asks" << std::endl;
        return sales;
    }

    // sort asks lowest first

    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    //next remove anything greater than the highest bid
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);
    //remove anything less than the lowest ask
    // for ask in asks:

    ////////////this is what needs to be optimized
    for (OrderBookEntry &ask : asks)
    {

        //std::cout << "Ask: " << ask.price << std::endl <<std::endl;
        //     for bid in bids:
        for (OrderBookEntry &bid : bids)
        {
            //std::cout << "Bid: " << bid.price << std::endl;
            //         if bid.price >= ask.price # we have a match
            if (bid.price >= ask.price)
            {
                //             sale = new order()
                //             sale.price = ask.price
                OrderBookEntry sale{ask.price, 0, timestamp,
                                    product,
                                    OrderBookType::asksale};

                if (bid.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }
                if (ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::asksale;
                }

                //             # now work out how much was sold and
                //             # create new bids and asks covering
                //             # anything that was not sold
                //             if bid.amount == ask.amount: # bid completely clears ask
                if (bid.amount == ask.amount)
                {
                    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
                    //                 sales.append(sale)
                    sales.push_back(sale);
                    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
                    //                 # can do no more with this ask
                    //                 # go onto the next ask
                    //                 break
                    break;
                }
                //           if bid.amount > ask.amount:  # ask is completely gone slice the bid
                if (bid.amount > ask.amount)
                {
                    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
                    //                 sales.append(sale)
                    sales.push_back(sale);
                    //                 # we adjust the bid in place
                    //                 # so it can be used to process the next ask
                    //                 bid.amount = bid.amount - ask.amount
                    bid.amount = bid.amount - ask.amount;
                    //                 # ask is completely gone, so go to next ask
                    //                 break
                    break;
                }

                //             if bid.amount < ask.amount # bid is completely gone, slice the ask
                if (bid.amount < ask.amount &&
                    bid.amount > 0)
                {
                    //                 sale.amount = bid.amount
                    sale.amount = bid.amount;
                    //                 sales.append(sale)
                    sales.push_back(sale);
                    //                 # update the ask
                    //                 # and allow further bids to process the remaining amount
                    //                 ask.amount = ask.amount - bid.amount
                    ask.amount = ask.amount - bid.amount;
                    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
                    //                 # some ask remains so go to the next bid
                    //                 continue
                    continue;
                }
            }
        }
    }
    // asks = orderbook.asks

    return sales;
}

int OrderBook::getIndex(std::string insertTimestamp)
{
    int counter = 0;
    std::list<OrderBookEntry>::const_iterator it = orders.cbegin();
    while (it->timestamp < insertTimestamp)

    {
        std::advance(it, 1);
        counter++;
    }

    return counter;
}
