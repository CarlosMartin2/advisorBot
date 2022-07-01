#include "OrderBook.h"
#include "CSVReader.h"
#include <map>
#include <algorithm>
#include <iostream>
#include <math.h>

/** construct, reading a csv data file */
OrderBook::OrderBook(std::string filename)
{	
    read_filename = filename;
    orders = CSVReader::readCSV(filename);
    if (orders.size()>0) {
    	read_timestamp_cnt = 1;
    	TimeStamps.push_back(orders[0].timestamp);
    }else read_timestamp_cnt = 0;
    cur_timestamp_index = 0;
}

/** return vector of all know products in the dataset*/
std::vector<std::string> OrderBook::getKnownProducts()
{
    std::vector<std::string> products;

    std::map<std::string,bool> prodMap;

    for (OrderBookEntry& e : orders)
    {
        prodMap[e.product] = true;
    }
    
    prodMap[orders[orders.size()].product] = false;
    
    // now flatten the map to a vector of strings
    for (auto const& e : prodMap)
    {
        products.push_back(e.first);
    }

    return products;
}
/** return vector of Orders according to the sent filters*/
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, 
                                        std::string product, 
                                        std::string timestamp)
{
    std::vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry& e : orders)
    {
        if (e.orderType == type && 
            e.product == product && 
            e.timestamp == timestamp )
            {
                orders_sub.push_back(e);
            }
    }
    return orders_sub;
}

double OrderBook::getPredictPrice(std::string product, std::string type, std::string kind, std::string timestamp)
{
	// We use LPC algorithm predict next price with specified product, type, kind & in current timestamp.
	
	std::string prevTime = getPrevTime_NoLoop(timestamp);
	std::vector<double> avg_array;
	
	double max=0, min=100;
	double max1=0, min1=100;
	double avg1=0, avg=0;
	double result = 0, delta = 0;
	int cnt = 0;
	OrderBookType type_v;
	if (type == "ask") type_v=OrderBookType::ask;
	if (type == "bid") type_v=OrderBookType::bid;
		
	for (OrderBookEntry& e : orders){
	        if (e.orderType == type_v && e.product == product)
		{
			if (e.timestamp == prevTime){
				if(max<e.price) max = e.price;
				if(min>e.price) min = e.price;	
			}
			if (e.timestamp == timestamp){
				if(max1<e.price) max1 = e.price;
				if(min1>e.price) min1 = e.price;	
				cnt++;
			}
		}
	}
	
	avg = (max+min)/2;
	avg1 = (max1+min1)/2;
	if (prevTime == "") avg = avg1;
	result = (avg + avg1)/2;
	
	for (OrderBookEntry& e : orders){
	        if (e.orderType == type_v && e.product == product && e.timestamp == timestamp)
		{
			delta += (e.price-avg1)*(e.price-avg1);
		}
	}
	delta = sqrt(delta / cnt);
	
	
	if (kind=="max") return result+delta;
	if (kind=="min") return result-delta;
	return 0;



}


double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders)
{
    double max = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price > max)max = e.price;
    }
    return max;
}

double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders)
{
    double min = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price < min)min = e.price;
    }
    return min;
}

//calculate max price following type(ask) and product.
double OrderBook::getMaxPrice_ask(std::vector<OrderBookEntry>& orders)
{
    double max = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price > max && e.orderType == OrderBookType::ask)max = e.price;
    }
    return max;
}

//calculate max price following type(bid) and product.
double OrderBook::getMaxPrice_bid(std::vector<OrderBookEntry>& orders)
{
    double max = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price > max && e.orderType == OrderBookType::bid)max = e.price;
    }
    return max;
}

//calculate min price following type(ask) and product.
double OrderBook::getMinPrice_ask(std::vector<OrderBookEntry>& orders)
{
    double min = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price < min && e.orderType == OrderBookType::ask)min = e.price;
    }
    return min;
}

//calculate min price following type(bid) and product.
double OrderBook::getMinPrice_bid(std::vector<OrderBookEntry>& orders)
{
    double min = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price < min && e.orderType == OrderBookType::bid)min = e.price;
    }
    return min;
}

//calculate average price following type, product and timestep.
double OrderBook::getAvgPrice(std::string product, std::string type, std::vector<OrderBookEntry>& orders)
{
    
    double sum = 0.0;
    for (OrderBookEntry& e : orders)
    {
        sum += e.price;
    }
    return sum/orders.size();
}


std::string OrderBook::getEarliestTime()
{
    return orders[0].timestamp;
}

std::string OrderBook::getNextTime_NoLoop(std::string timestamp)
{
    std::string next_timestamp = "";
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp > timestamp) 
        {
            next_timestamp = e.timestamp;
            break;
        }
    }
    /*
    if (next_timestamp == "")
    {
        next_timestamp = orders[0].timestamp;
    }
    */
    return next_timestamp;
}

std::string OrderBook::getNextTime(std::string timestamp)
{
    if (cur_timestamp_index < read_timestamp_cnt - 1){
    	cur_timestamp_index ++;
    	return TimeStamps[cur_timestamp_index];
    }
    std::string next_timestamp = orders[orders.size()-1].timestamp;
    std::vector<OrderBookEntry> temp = CSVReader::readNextTimeStepCSV(read_filename, next_timestamp, orders.size());
    if (temp.size() == 0){
    	cur_timestamp_index = read_timestamp_cnt - 1;
    	return "";
    } 
    TimeStamps.push_back(next_timestamp);
    read_timestamp_cnt ++;
    cur_timestamp_index ++;
    orders.insert(orders.end(), temp.begin(), temp.end());
    return next_timestamp;
}


std::string OrderBook::getPrevTime(std::string timestamp)
{
    std::string prev_timestamp = "";
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp < timestamp) 
        {
            prev_timestamp = e.timestamp;
        }else{
        	break;
        }
    }
    cur_timestamp_index --;
    
    if (prev_timestamp == "")
    {
        prev_timestamp = orders[0].timestamp;
        cur_timestamp_index = 0;
    }
    return prev_timestamp;
}

std::string OrderBook::getPrevTime_NoLoop(std::string timestamp)
{
    std::string prev_timestamp = "";
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp < timestamp) 
        {
            prev_timestamp = e.timestamp;
        }else{
        	break;
        }
    }
    
    return prev_timestamp;
}


void OrderBook::insertOrder(OrderBookEntry& order)
{
    orders.push_back(order);
    std::sort(orders.begin(), orders.end(), OrderBookEntry::compareByTimestamp);
}

std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
// asks = orderbook.asks
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, 
                                                 product, 
                                                 timestamp);
// bids = orderbook.bids
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, 
                                                 product, 
                                                    timestamp);

    // sales = []
    std::vector<OrderBookEntry> sales; 

    // I put in a little check to ensure we have bids and asks
    // to process.
    if (asks.size() == 0 || bids.size() == 0)
    {
        std::cout << " OrderBook::matchAsksToBids no bids or asks" << std::endl;
        return sales;
    }

    // sort asks lowest first
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    // sort bids highest first
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);
    // for ask in asks:
    std::cout << "max ask " << asks[asks.size()-1].price << std::endl;
    std::cout << "min ask " << asks[0].price << std::endl;
    std::cout << "max bid " << bids[0].price << std::endl;
    std::cout << "min bid " << bids[bids.size()-1].price << std::endl;
    
    for (OrderBookEntry& ask : asks)
    {
    //     for bid in bids:
        for (OrderBookEntry& bid : bids)
        {
    //         if bid.price >= ask.price # we have a match
            if (bid.price >= ask.price)
            {
    //             sale = new order()
    //             sale.price = ask.price
            OrderBookEntry sale{ask.price, 0, timestamp, 
                product, 
                OrderBookType::asksale};

                if (bid.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }
                if (ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType =  OrderBookType::asksale;
                }
            
    //             # now work out how much was sold and 
    //             # create new bids and asks covering 
    //             # anything that was not sold
    //             if bid.amount == ask.amount: # bid completely clears ask
                if (bid.amount == ask.amount)
                {
    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
    //                 # can do no more with this ask
    //                 # go onto the next ask
    //                 break
                    break;
                }
    //           if bid.amount > ask.amount:  # ask is completely gone slice the bid
                if (bid.amount > ask.amount)
                {
    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 # we adjust the bid in place
    //                 # so it can be used to process the next ask
    //                 bid.amount = bid.amount - ask.amount
                    bid.amount =  bid.amount - ask.amount;
    //                 # ask is completely gone, so go to next ask                
    //                 break
                    break;
                }


    //             if bid.amount < ask.amount # bid is completely gone, slice the ask
                if (bid.amount < ask.amount && 
                   bid.amount > 0)
                {
    //                 sale.amount = bid.amount
                    sale.amount = bid.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 # update the ask
    //                 # and allow further bids to process the remaining amount
    //                 ask.amount = ask.amount - bid.amount
                    ask.amount = ask.amount - bid.amount;
    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
    //                 # some ask remains so go to the next bid
    //                 continue
                    continue;
                }
            }
        }
    }
    return sales;             
}
