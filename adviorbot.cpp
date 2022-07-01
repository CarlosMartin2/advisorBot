#include "adviorbot.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "OrderBookEntry.h"
#include "CSVReader.h"

Adviorbot::Adviorbot()
{

}

void Adviorbot::init()
{
    std::string input;
    //this variable keep time that we are looking now. But it has earliest time because it's first.
    currentTime = orderBook.getEarliestTime();
   
    while(true)
    {
    	//input command line 
        input = getUserOption();
        //process from input data
        processUserOption(input);
    }
}

//Display helpful command for user
void Adviorbot::printHelp(std::string help_cmd)
{
	if(help_cmd == "")
		std::cout << "The available commands are help, min, max, avg, predict, prod, time, step, prevstep." << std::endl;
	else if(help_cmd == "help")
		std::cout << "Output help for the specified command." << std::endl;
	else if(help_cmd == "avg")
		std::cout << "avg ETH/BTC bid 10 -> average ETH/BTC bid over last 10 time steps." << std::endl;
	else if(help_cmd == "min")
		std::cout << "min ETH/BTC ask -> The min ask for ETH/BTC is 1.0." << std::endl;
	else if(help_cmd == "max")
		std::cout << "max ETH/BTC ask -> The max ask for ETH/BTC is 1.0." << std::endl;
	else if(help_cmd == "predict")
		std::cout << "predict max ETH/BTC bid -> The price of ETH/BTC bid will be 1.0." << std::endl;
	else if(help_cmd == "prod")
		std::cout << "prod -> list available products." << std::endl;
	else if(help_cmd == "time")
		std::cout << "time -> display currentTime on current timestep." << std::endl;
	else if(help_cmd == "step")
		std::cout << "step -> go to next time frame." << std::endl;
	else if(help_cmd == "prevstep")
		std::cout << "prevstep -> go to prev time frame." << std::endl;
	else
		errorMessage();
}

//Display all known products
void Adviorbot::printProduct()
{
	for (std::string const& p : orderBook.getKnownProducts())
	{
		std::cout << p << std::endl;
	}
    
}

//Display Min price about specified product and type
void Adviorbot::printMin(std::string product, std::string OrderBookType)
{
	if(OrderBookType == "ask"){
		std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, product, currentTime);
		std::cout << "The min ask for " << product<< " is " <<OrderBook::getMinPrice_ask(entries) << std::endl;
	}else if(OrderBookType == "bid"){
		std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid, product, currentTime);
		std::cout << "The min bid for " << product<< " is " <<OrderBook::getMinPrice_bid(entries) << std::endl;
	}
    	
}

//Display Max price about specified product and type
void Adviorbot::printMax(std::string product, std::string OrderBookType)
{
	if(OrderBookType == "ask"){
		std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, product, currentTime);
		std::cout << "The max ask for " << product<< " is " <<OrderBook::getMaxPrice_ask(entries) << std::endl;
	}else if(OrderBookType == "bid"){
		std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid, product, currentTime);
		std::cout << "The max bid for " << product<< " is " <<OrderBook::getMaxPrice_bid(entries) << std::endl;
	}
    
}

//Display Average price about specified product and type.
void Adviorbot::printAvg(std::string product, std::string OrderBookType, std::string timestep)
{
	int step_cnt = stoi(timestep), i;
    	std::string TempTime = currentTime, prevTime;
    
	//declare total sum of all products with type and timestamp.
	double sum = 0.0;
	for(i = 0; i < step_cnt; i ++){
		if(OrderBookType == "ask"){
			std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, product, TempTime);
			sum += orderBook.getAvgPrice(product, OrderBookType, entries);
			
		}else if(OrderBookType == "bid"){
			std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid, product, TempTime);
			sum += orderBook.getAvgPrice(product, OrderBookType, entries);
		}
		prevTime = orderBook.getPrevTime_NoLoop(TempTime);
		if(prevTime == "") break;
	}
	//If timesteps are less than timestep, calculate average with limited timesteps, 	
	if((i+1) < step_cnt){	
		std::cout << "The average is calculated on limited space because you can't go prev timestep" << std::endl;
	}
	std::cout << "The Average " << product << " " << OrderBookType << " price over the last " << timestep << " was " << sum/(i+1) << std::endl;
}

//Predict price of next time.
void Adviorbot::printPredict(std::string kind, std::string product, std::string type)
{
	std::cout << "The price of "<<kind<<" "<<product<< " "<<type<<" will be " << orderBook.getPredictPrice(product, type, kind , currentTime) <<  std::endl;
}

//Display current Time we are looking at.
void Adviorbot::printTime()
{
    std::cout << currentTime << std::endl;
}

//Go to next timestep.
void Adviorbot::printStep()
{
	std::string temp = orderBook.getNextTime(currentTime);
	std::cout << "  Next time step is : ";
	if(temp == ""){
		  std::cout << currentTime << ",  It's last timestep!"<<std::endl;	
	}else{
		currentTime = temp;
	    std::cout << currentTime << std::endl;		
	}
}


//Go to prev timestep.
void Adviorbot::printPrevStep()
{
    currentTime = orderBook.getPrevTime(currentTime);
    std::cout << currentTime << std::endl;
}

//seperate string by delimator.
void tokenize(std::string const &str, const char delim, std::vector<std::string> &out)
{
    // construct a stream from the string
    std::stringstream ss(str);
 
    std::string s;
    while (std::getline(ss, s, delim)) {
        out.push_back(s);
    }
}
   
//input user's command 
std::string Adviorbot::getUserOption()
{
    std::cout << "user> ";
    std::string line;
    std::getline(std::cin, line);
    return line;
}

//Process input data from user.
void Adviorbot::processUserOption(std::string userOption)
{
    if(userOption =="") return;
    
    userOption.erase(0, userOption.find_first_not_of(' '));
    std::string s = userOption;
    const char delim = ' ';
 
    std::vector<std::string> out;
    out = CSVReader::tokenise(s, ' ');

    if (out.size() == 0) return;
    
    std::cout << "advisorbot> ";
    if (out[0] == "help") 
    {	
	if(out.size() == 1)
    		printHelp("");    
	else
		printHelp(out[1]);
    }
    else if (out.size() == 1 && out[0] == "prod") 
    {
        printProduct();
    }
    else if (out.size() == 3 && out[0] == "min") 
    { 	//check to product key and type key.
    	if(validation_Product(out[1]) > 0 && validation_Type(out[2]) > 0){
    		printMin(out[1], out[2]);
    	}else
    		errorMessage();	
        
    }
    else if (out.size() == 3 && out[0] == "max") 
    {	//check to product key and type key.
        if(validation_Product(out[1]) > 0 && validation_Type(out[2]) > 0){
    		printMax(out[1], out[2]);
    	}else
    		errorMessage();	
    }
    else if (out.size() == 4 && out[0] == "avg") 
    {	//check to product key and type key and validate that timestep is number.
    	if(validation_Product(out[1]) > 0 && validation_Type(out[2]) > 0 && stoi(out[3]) > 0){
    		printAvg(out[1], out[2], out[3]);
    	}else
    		errorMessage();	
        
    }
    else if (out.size() == 4 && out[0] == "predict") 
    {
    	if((out[1] == "min" || out[1] == "max") && validation_Product(out[2]) > 0 && validation_Type(out[3]) > 0){
    		printPredict(out[1], out[2], out[3]);
    	}else
    		errorMessage();	
    }//current time
    else if (out.size() == 1 && out[0] == "time") 
    {
        printTime();
    }//go to next timestep.
    else if (out.size() == 1 && out[0] == "step") 
    {
        printStep();
    }//go to prev timestep.
    else if (out.size() == 1 && out[0] == "prevstep") 
    {
        printPrevStep();
    }
    else{
    	errorMessage();		
    }
}

//check that the product exsist in list.
int Adviorbot::validation_Product(std::string product){
	for (std::string const& p : orderBook.getKnownProducts())
	{
		if(p == product) return 1;
	}
	return 0;
}

//check that the type is ask or bid.
int Adviorbot::validation_Type(std::string type){

	if(type == "ask" || type == "bid")
		return 1;
	else
		return 0;
}

void Adviorbot::errorMessage(){
	std::cout << "Enter correct command." << std::endl;	
}
