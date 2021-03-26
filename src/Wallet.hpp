#pragma once

#include <string>
#include "OrderBookEntry.hpp"
#include <iostream>
#include <map>
#include <vector>

class Wallet
{
public:
    Wallet();
    /** insert currency to the wallet */
    void insertCurrency(std::string type, double amount);

    /** remove currency from the wallet */
    bool removeCurrency(std::string type, double amount);

    /** check if the wallet contains this much currency or more */
    bool containsCurrency(std::string type, double amount);

    /** checks if the wallet can cope with this ask or bid.*/
    bool canFulfillOrder(OrderBookEntry order);

    /** update the contents of the wallet
         * assumes the order was made by the owner of the wallet
         * writes sale to file
        */
    void processSale(OrderBookEntry &sale);

    /** Returns all sales for the timestamp*/
    std::vector<OrderBookEntry> getSales(std::string timestamp);

    /** generate a string representation of the wallet */
    std::string toString();
    friend std::ostream &operator<<(std::ostream &os, Wallet &wallet);

    /** Print wallet balance to file*/
    void printBalance(std::string timestamp);

    std::map<std::string, double> currencies;
    std::vector<OrderBookEntry> orderHistory;

private:
    std::string fileName = "wallet.txt";
    void logChanges(std::string timestamp, std::string incomingCurrency, double incomingAmount, std::string outgoingCurrency, double outgoingAmount);
};
