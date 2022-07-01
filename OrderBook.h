#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <string>
#include <vector>

class OrderBook
{
    public:
    /** construct, reading a csv data file */
        OrderBook(std::string filename);
    /** return vector of all know products in the dataset*/
        std::vector<std::string> getKnownProducts();
    /** return vector of Orders according to the sent filters*/
        std::vector<OrderBookEntry> getOrders(OrderBookType type, 
                                              std::string product, 
                                              std::string timestamp);

        /** returns the earliest time in the orderbook*/
        std::string getEarliestTime();
        /** returns the next time after the 
         * sent time in the orderbook  
         * If there is no next timestamp, wraps around to the start
         * */
        std::string getNextTime(std::string timestamp);
        std::string getPrevTime(std::string timestamp);
        std::string getNextTime_NoLoop(std::string timestamp);
        std::string getPrevTime_NoLoop(std::string timestamp);
                       
        void insertOrder(OrderBookEntry& order);

        std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);

        static double getHighPrice(std::vector<OrderBookEntry>& orders);
        static double getLowPrice(std::vector<OrderBookEntry>& orders);
        
        
        static double getMaxPrice_ask(std::vector<OrderBookEntry>& orders);
        static double getMaxPrice_bid(std::vector<OrderBookEntry>& orders);
        static double getMinPrice_ask(std::vector<OrderBookEntry>& orders);
        static double getMinPrice_bid(std::vector<OrderBookEntry>& orders);
        static double getAvgPrice(std::string product, std::string type, std::vector<OrderBookEntry>& orders);
        static double getAvgPrice_bid(std::string product, std::string type);
        double getPredictPrice(std::string product, std::string type, std::string kind, std::string timestamp);

    private:
        std::vector<OrderBookEntry> orders;
        std::vector<std::string> TimeStamps;
        std::string read_filename;
	int read_timestamp_cnt;
	int cur_timestamp_index;

};
