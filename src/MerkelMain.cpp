//This is the original Merklemain class, modified to act as an api for interacting with the wallet and exchange

#include "MerkelMain.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "OrderBookEntry.hpp"
#include "CSVReader.hpp"

MerkelMain::MerkelMain()
{
}

void MerkelMain::init()
{

    currentTime = orderBook.getEarliestTime();
    wallet.insertCurrency("BTC", 5);
    wallet.insertCurrency("USDT", 25000);
}

void MerkelMain::enterAsk(std::string product, double amount, double price)
{

    OrderBookEntry obe = CSVReader::stringsToOBE(
        std::to_string(price),
        std::to_string(amount),
        currentTime,
        product,
        OrderBookType::ask);
    obe.username = "simuser";

    if (wallet.canFulfillOrder(obe))
    {

        orderBook.insertOrder(obe);
    }
    else
    {
        std::cout << "Wallet has insufficient funds . " << std::endl;
    }
}

void MerkelMain::enterBid(std::string product, double amount, double price)
{

    OrderBookEntry obe = CSVReader::stringsToOBE(
        std::to_string(price),
        std::to_string(amount),
        currentTime,
        product,
        OrderBookType::bid);
    obe.username = "simuser";

    if (wallet.canFulfillOrder(obe))
    {

        orderBook.insertOrder(obe);
    }
    else
    {
        std::cout << "Wallet has insufficient funds . " << std::endl;
    }
}

void MerkelMain::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}

void MerkelMain::gotoNextTimeframe(bool newTrade)

{
    std::vector<OrderBookEntry> sales;
    if (newTrade)
    {
        for (std::string p : orderBook.getKnownProducts())
        {
            sales = orderBook.matchAsksToBids(p, currentTime);

            for (OrderBookEntry &sale : sales)
            {
                if (sale.username == "simuser")
                {
                    // update the wallet

                    wallet.processSale(sale);
                }
            }
        }
    }

    if (currentTime != orderBook.getFinalTime())
        currentTime = orderBook.getNextTime(currentTime);
    wallet.printBalance(currentTime);
}
