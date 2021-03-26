#pragma once

#include "OrderBookEntry.hpp"
#include <vector>
#include <list>
#include <string>

class CSVReader
{
public:
    /** CSV Reader constructor */
    CSVReader();

    /** Reads from CSV file and creates a list of OrderBookENtry */
    static std::list<OrderBookEntry> readCSV(std::string csvFile);

    /** Takes a line of csvs and tokenises them according to the seperator value */
    static std::vector<std::string> tokenise(std::string csvLine, char separator);

    /**  Converts inputs into and OrderBookEntry*/
    static OrderBookEntry stringsToOBE(std::string price,
                                       std::string amount,
                                       std::string timestamp,
                                       std::string product,
                                       OrderBookType OrderBookType);

private:
    static OrderBookEntry stringsToOBE(std::vector<std::string> strings);
};