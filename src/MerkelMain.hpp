#pragma once

#include <vector>
#include "OrderBookEntry.hpp"
#include "OrderBook.hpp"
#include "Wallet.hpp"

class MerkelMain
{
public:
    /** MerkelMain constructor  */
    MerkelMain();
    //The wallet exists on the exchange as in real life.
    Wallet wallet;
    OrderBook orderBook{"20200601.csv"};
    std::string currentTime;

    /** Starts the simulation and adds cryptocurrency to the users wallet*/
    void init();

    /** Takes product, amount and price, creates an ask OBE from them and inserts them into the orderbook */
    void enterAsk(std::string product, double amount, double price);

    /** Takes product, amount and price, creates a bid OBE from them and inserts them into the orderbook */
    void enterBid(std::string product, double amount, double price);

    /** Increments currentTime if newTrade is true */
    void gotoNextTimeframe(bool newTrade);

private:
    void printWallet();
};
