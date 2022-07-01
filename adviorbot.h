#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"


class Adviorbot
{
    public:
        Adviorbot();
        /** Call this to start the sim */
        void init();
    private: 
    	//function for showing helpful command
        void printHelp(std::string help_cmd);
        //function for showing all products in list
        void printProduct();
        //function for calculating min price  
        void printMin(std::string product, std::string OrderBookType);
        //function for calculating max price  
        void printMax(std::string product, std::string OrderBookType);
        //function for calculating average price  
        void printAvg(std::string product, std::string OrderBookType, std::string timestep);
        //function for predicting price of next timestep.
        void printPredict(std::string kind, std::string product, std::string type);
        //function to display current time. 
        void printTime();
        //function to go to next timestep.
        void printStep();
        //function to go to prev timestep.
        void printPrevStep();
        //function to input data from user.
        std::string getUserOption();
        //function to process input data from user.
        void processUserOption(std::string userOption);
        //check that the product exsist in list.
	int validation_Product(std::string product);
	//check the type.
	int validation_Type(std::string type);
	//display error message.
	void errorMessage();	
	
        std::string currentTime;
        
	OrderBook orderBook{"20200601.csv"};
//	OrderBook orderBook{"20200317.csv"};
	
        Wallet wallet;

};
