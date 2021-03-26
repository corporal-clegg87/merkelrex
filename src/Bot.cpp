#include "Bot.hpp"
#include <string>

Bot::Bot(std::string _tradesWhat, double _takeProfit, double _stopLoss, double _buyDip,
         double _askema, double _bidema)

    : tradesWhat(_tradesWhat),
      takeProfit(_takeProfit),
      buyDip(_buyDip),
      stopLoss(_stopLoss),
      askema(_askema),
      bidema(_bidema)
{
}
