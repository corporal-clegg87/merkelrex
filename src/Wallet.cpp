#include "Wallet.hpp"
#include <iostream>
#include <fstream>
#include "CSVReader.hpp"
#include <vector>

Wallet::Wallet()
{
}

void Wallet::insertCurrency(std::string type, double amount)
{
    double balance;
    if (amount < 0)
    {
        throw std::exception{};
    }
    if (currencies.count(type) == 0) // not there yet
    {
        balance = 0;
    }
    else
    { // is there
        balance = currencies[type];
    }
    balance += amount;
    currencies[type] = balance;
}

bool Wallet::removeCurrency(std::string type, double amount)
{
    if (amount < 0)
    {
        return false;
    }
    if (currencies.count(type) == 0) // not there yet
    {
        return false;
    }
    else
    {                                       // is there - do  we have enough
        if (containsCurrency(type, amount)) // we have enough
        {
            currencies[type] -= amount;
            return true;
        }
        else // they have it but not enough.
            return false;
    }
}

bool Wallet::containsCurrency(std::string type, double amount)
{
    if (currencies.count(type) == 0) // not there yet
        return false;
    else
        return currencies[type] >= amount;
}

std::string Wallet::toString()
{
    std::string s;
    for (std::pair<std::string, double> pair : currencies)
    {
        std::string currency = pair.first;
        double amount = pair.second;
        s += currency + " : " + std::to_string(amount) + "\n";
    }
    return s;
}

bool Wallet::canFulfillOrder(OrderBookEntry order)
{
    std::vector<std::string> currs = CSVReader::tokenise(order.product, '/');
    // ask
    if (order.orderType == OrderBookType::ask)
    {
        double amount = order.amount;
        std::string currency = currs[0];
        std::cout << "Wallet::canFulfillOrder " << currency << " : " << amount << std::endl;

        return containsCurrency(currency, amount);
    }
    // bid
    if (order.orderType == OrderBookType::bid)
    {
        double amount = order.amount * order.price;
        std::string currency = currs[1];
        return containsCurrency(currency, amount);
    }

    return false;
}

void Wallet::processSale(OrderBookEntry &sale)
{
    orderHistory.push_back(sale);
    std::vector<std::string> currs = CSVReader::tokenise(sale.product, '/');
    // ask
    if (sale.orderType == OrderBookType::asksale)
    {
        double outgoingAmount = sale.amount;
        std::string outgoingCurrency = currs[0];
        double incomingAmount = sale.amount * sale.price;
        std::string incomingCurrency = currs[1];

        currencies[incomingCurrency] += incomingAmount;
        currencies[outgoingCurrency] -= outgoingAmount;
        logChanges(sale.timestamp, incomingCurrency, incomingAmount, outgoingCurrency, outgoingAmount);
    }
    // bid
    if (sale.orderType == OrderBookType::bidsale)
    {
        double incomingAmount = sale.amount;
        std::string incomingCurrency = currs[0];
        double outgoingAmount = sale.amount * sale.price;
        std::string outgoingCurrency = currs[1];

        currencies[incomingCurrency] += incomingAmount;
        currencies[outgoingCurrency] -= outgoingAmount;
        logChanges(sale.timestamp, incomingCurrency, incomingAmount, outgoingCurrency, outgoingAmount);
    }
}
std::ostream &operator<<(std::ostream &os, Wallet &wallet)
{
    os << wallet.toString();
    return os;
}

std::vector<OrderBookEntry> Wallet::getSales(std::string timestamp)
{
    std::vector<OrderBookEntry> theseSales;
    for (auto &e : orderHistory)
    {
        if (e.timestamp == timestamp)
            theseSales.push_back(e);
    }

    return theseSales;
}

//log trades in format product, quantity, price, exchange price
void Wallet::logChanges(std::string timestamp, std::string incomingCurrency, double incomingAmount, std::string outgoingCurrency, double outgoingAmount)
{
    std::ofstream fout;
    fout.open(fileName, std::ios_base::app);
    std::ofstream balance();
    if (fout.is_open())
    {
        fout << incomingAmount << " " << incomingCurrency << " in, " << outgoingAmount << " " << outgoingCurrency << " out" << std::endl;
    }
    else
        std::cout << "Cannot open balance file in walllet " << std::endl;
}

void Wallet::printBalance(std::string timestamp)
{

    std::ofstream fout;
    fout.open(fileName, std::ios_base::app);

    if (fout.is_open())
    {
        fout << timestamp << " totals: " << std::endl
             << toString() << std::endl
             << std::endl;
    }
    else
        std::cout << "Cannot open balance file in walllet " << std::endl;
}