#pragma once

#include "OrderBook.hpp"
#include <string>
#include <iostream>
#include "MerkelMain.hpp"
#include "Bot.hpp"

enum class tradingActivity
{
    open,
    active,
    closed
};

class TradingBot
{

public:
    TradingBot(MerkelMain app);

    void init();

    //double getAveragePrice();

private:


    /** iterates over the order book and returns the average price*/
    double getAveragePrice(OrderBookType obType, std::string timestamp, std::string tradesWhat);
    /** iterates over the order book and returns the average price*/
    double getAveragePrice(std::vector<OrderBookEntry> sales);
    /** get quantity of crypto bought or sold*/
    double getQuantity(std::vector<OrderBookEntry> orders);

    /** returns difference between ema and price*/
    double getDifference(double ema, double price);

    /** Returns the new ema, given price and previous ema*/
    double getEMA(double average, double ema);

    /** Adds sales to the bots runningTrades*/
    void openTrade(std::vector<OrderBookEntry> sales, Bot &bot);

    /** Updates bots runningTrades with sales*/
    void closeTrade(std::vector<OrderBookEntry> sales, Bot &bot);

    /** Looks for trade entries for bot with given askprice*/
    void lookForTrades(Bot bot, double askprice);

    /** Looks for trade exits for bot with given sellprice*/
    void lookForClose(Bot bot, double sellprice);

    /** Manages trading activities*/
    void trade();

    /** Adds bot to bots*/
    void addBots(Bot bot);

    void appendOpenOrders(std::string tradesWhat, double amount,
     double price, std::string orderType);

    void setNewTrade(bool _newTrade);

    MerkelMain app{};
    int history = 10;
    int positionSizeFactor = 20;
    std::vector<Bot> bots;
    bool newTrade;

    //std::string tesQuickTime = "2020/06/01 11:59:00.419047";
    //12:11:36.727369
    //std::string testMaxTime = "2020/06/01 12:11:36.727369";
};