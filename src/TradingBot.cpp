#include "TradingBot.hpp"
#include <algorithm>
#include "MerkelMain.hpp"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <map>
#include <iterator>
#include <chrono>

TradingBot::TradingBot(MerkelMain _app)

    : app(_app)

{
}

void TradingBot::init()

{

    auto start = std::chrono::high_resolution_clock::now();
    Bot btcusdt("BTC/USDT", 0.001, -0.0005, 0.0002, 0.0, 0.0);
    Bot ethusdt("ETH/USDT", 0.001, -0.0005, 0.0002, 0.0, 0.0);
    Bot ethbtc("ETH/BTC", 0.001, -0.0005, 0.0002, 0.0, 0.0);
    Bot dogebtc("DOGE/BTC", 0.001, -0.0005, 0.0002, 0.0, 0.0);

    addBots(Bot ("BTC/USDT", 0.0015, -0.0005, 0.0002, 0.0, 0.0));
    addBots(Bot ("ETH/USDT", 0.0012, -0.0005, 0.0002, 0.0, 0.0));
    addBots(Bot ("ETH/BTC", 0.001, -0.0005, 0.0002, 0.0, 0.0));
    addBots(Bot ("DOGE/BTC", 0.008, -0.0005, 0.0002, 0.0, 0.0));





    for (int i = 0; i <= history; i++)
    {

        app.gotoNextTimeframe(newTrade);
    }

    for (auto &bot : bots)
    {
        double askprice = getAveragePrice(OrderBookType::ask, app.currentTime, bot.tradesWhat);
        double bidprice = getAveragePrice(OrderBookType::bid, app.currentTime, bot.tradesWhat);
        double tempEma = bot.askema;

        bot.askema = getEMA(askprice, bot.askema);
        if (tempEma < bot.askema)
            bot.trendIsBull = true;
        else
            bot.trendIsBull = false;
        bot.bidema = getEMA(bidprice, bot.bidema);
    }

    trade();

    for (auto &bot : bots)
    {
        std::string fileName = bot.tradesWhat + "_tradessummary.txt";
        fileName.erase(std::remove(fileName.begin(), fileName.end(), '/'), fileName.end());
        std::ofstream summary(fileName);
        if (summary.is_open())
        {

            for (auto &e : bot.runningTrades)
            {
                for (auto it = e.cbegin(); it != e.cend(); ++it)
                {
                    summary << it->first << ": " << it->second << ",";
                }
                summary << std::endl;
            }
        }
        else
            std::cout << "Cannot open summary file for " << bot.tradesWhat << std::endl;
    }
}

double TradingBot::getAveragePrice(OrderBookType obType, std::string timestamp, std::string tradesWhat)
{

    double average = 0, totalBase = 0, totalProduct = 0;

    std::vector<OrderBookEntry> currentBids = app.orderBook.getOrders(obType, tradesWhat, timestamp);
    for (auto &e : currentBids)
    {

        totalBase += e.price * e.amount;
        totalProduct += e.amount;
    }

    average = totalBase / totalProduct;

    return average;
}

double TradingBot::getAveragePrice(std::vector<OrderBookEntry> orders)
{

    double average = 0, totalBase = 0, totalProduct = 0;

    for (auto &e : orders)
    {

        totalBase += e.price * e.amount;
        totalProduct += e.amount;
    }

    average = totalBase / totalProduct;

    return average;
}

double TradingBot::getQuantity(std::vector<OrderBookEntry> orders)
{
    double totalProduct = 0;

    for (auto &e : orders)
    {
        totalProduct += e.amount;
    }

    return totalProduct;
}

double TradingBot::getDifference(double ema, double price)
{
    return (price - ema) / price;
}

double TradingBot::getEMA(double average, double ema)
{
    double mult = 2.0 / (history / 2.0 + 1.0);
    double rslt = (average - ema) * mult + ema;
    return rslt;
}

void TradingBot::trade()
{

    setNewTrade(false);
    while (app.currentTime < app.orderBook.getFinalTime())
    {

        app.orderBook.removeOldOrders();
        for (auto &bot : bots)
        {
            double askprice = getAveragePrice(OrderBookType::ask, app.currentTime, bot.tradesWhat);
            double bidprice = getAveragePrice(OrderBookType::bid, app.currentTime, bot.tradesWhat);
            double tempEma = bot.askema;

            bot.askema = getEMA(askprice, bot.askema);
            if (tempEma < bot.askema)
                bot.trendIsBull = true;
            else
                bot.trendIsBull = false;
            bot.bidema = getEMA(bidprice, bot.bidema);

            if (!bot.runningTrade)
                lookForTrades(bot, askprice);
            else
            {
                for (auto &e : bot.runningTrades)
                {
                    if (e["status"] == "OPEN")
                    {

                        lookForClose(bot, bidprice);
                    }
                }
            }
        }

        app.gotoNextTimeframe(newTrade);

        std::vector<OrderBookEntry> sales = app.wallet.getSales(app.orderBook.getPreviousTimestamp(app.currentTime));

        if (sales.size() != 0)
        {

            for (auto &bot : bots)
            {
                std::vector<OrderBookEntry> mySales;
                for (auto &sale : sales)
                {
                    if (sale.product == bot.tradesWhat)
                    {
                        mySales.push_back(sale);
                    }
                }
                if (mySales.size() != 0)
                {
                    if ((mySales[0].orderType == OrderBookType::asksale) && (sales[0].username == "simuser"))
                        closeTrade(mySales, bot);
                    else if ((mySales[0].orderType == OrderBookType::bidsale) && (sales[0].username == "simuser"))
                        openTrade(mySales, bot);
                }
            }
        }
    }
}

void TradingBot::openTrade(std::vector<OrderBookEntry> sales, Bot &bot)
{

    std::map<std::string, std::string> trade;

    trade["status"] = "OPEN";
    trade["product"] = bot.tradesWhat;
    trade["buytime"] = sales[0].timestamp;
    trade["buyprice"] = std::to_string(getAveragePrice(sales));
    trade["buyamount"] = std::to_string(getQuantity(sales));
    trade["selltime"] = "";
    trade["sellprice"] = "";
    trade["profit"] = "";
    bot.runningTrades.push_back(trade);

    bot.runningTrade = true;
}

void TradingBot::closeTrade(std::vector<OrderBookEntry> sales, Bot &bot)
{

    for (auto &e : bot.runningTrades)
    {
        if (e["status"] == "OPEN")
        {
            if (e["product"] == bot.tradesWhat)
            {
                e["status"] = "CLOSED";
                //newOrder["buytime"] = oldOrder["buytime"];
                //newOrder["buyprice"] = oldOrder["buyprice"];
                //newOrder["buyamount"] = oldOrder["buyamount"];
                e["selltime"] = sales[0].timestamp;
                e["sellprice"] = std::to_string(getAveragePrice(sales));
                e["profit"] = std::to_string((std::stod(e["sellprice"]) - std::stod(e["buyprice"])) / std::stod(e["buyprice"]));
            }
        }
    }
    bot.runningTrade = false;
}

void TradingBot::lookForTrades(Bot bot, double askprice)
{

    double difference = getDifference(bot.askema, askprice);
    std::string delimiter = "/";
    std::string baseCurrency = bot.tradesWhat.substr(bot.tradesWhat.find(delimiter) + 1);

    double amount = ((app.wallet.currencies.at(baseCurrency)) / this->positionSizeFactor) / askprice;

    //buy product on dips only if it's bullish
    if ((bot.trendIsBull) && (difference < bot.buyDip))
    {
        app.enterBid(bot.tradesWhat, amount, askprice);
        appendOpenOrders(bot.tradesWhat, amount, askprice, "bid");
        setNewTrade(true);
    }
}

void TradingBot::lookForClose(Bot bot, double bidprice)

{

    double difference = getDifference(bot.bidema, bidprice);

    std::string delimiter = "/";
    std::string baseCurrency = bot.tradesWhat.substr(bot.tradesWhat.find(delimiter) + 1);

    double amount = ((app.wallet.currencies.at(baseCurrency)) / this->positionSizeFactor) / bidprice;

    if ((difference >= bot.takeProfit) || (difference <= bot.stopLoss))
    {
        app.enterAsk(bot.tradesWhat, amount, bidprice);
        appendOpenOrders(bot.tradesWhat, amount, bidprice, "ask");
        setNewTrade(true);
    }
}

void TradingBot::addBots(Bot bot)
{
    bots.push_back(bot);
}

void TradingBot::setNewTrade(bool _newTrade)
{
    newTrade = _newTrade;
}

void TradingBot::appendOpenOrders(std::string tradesWhat, double amount,
 double price, std::string orderType)
{
    std::ofstream fout;
    fout.open("openedOrders", std::ios_base::app);
    std::ofstream balance();
    if (fout.is_open())
    {
        fout << app.currentTime << ": " << tradesWhat << " " << orderType
        << " " << amount << " @ " << price << std::endl << std::endl;
    }
    else
        std::cout << "Cannot open balance file in walllet " << std::endl;
}