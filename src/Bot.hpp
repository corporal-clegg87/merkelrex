#pragma once

#include <vector>
#include <map>
#include <string>

class Bot
{

public:
    Bot(std::string tradesWhat, double takeProfit,
        double stopLoss, double buyDip, double askema, double sellema);

    std::string tradesWhat;
    double takeProfit;
    double buyDip;
    double stopLoss;
    std::vector<std::map<std::string, std::string>> runningTrades;
    bool runningTrade = false;
    double askema;
    double bidema;
    bool trendIsBull;
};