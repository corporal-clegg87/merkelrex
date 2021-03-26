
#include "Wallet.hpp"
#include <iostream>
#include "MerkelMain.hpp"
#include "TradingBot.hpp"

int main()
{

    MerkelMain app{};
    app.init();
    TradingBot bot(app);
    bot.init();
}
