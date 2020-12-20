#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdio.h>
#include <thread>
#include <deque>
#include <set>


using namespace std;


struct order 
{
	int identificator_of_lift; //id of lisft
	int time; //time counter

	int before;//from which floor
	int after; //after which floor
};

enum ACTION {EMPTY, WAIT, MOVE, OPEN, IDLE, IN, OUT, CLOSE};//Let's define actions for lifts and doors

enum DOORS {OPENED, CLOSED};

struct building_stats { //Let's take KPM for example and students in sturct with data
	
	int N = 9; //number of floors
	int K = 4;//number of elevators
	int C = 12; //elevator capacity, checked experimentally)
	int T_STAGE = 1;//time quantor for moving between stages
	int T_OPEN = 1;//time quantor for opening 
	int T_IDLE = 10; //time for standing empty
	int T_CLOSE = 1; //time for door closing
	int T_IN = 1;//time for enter a one man
	int T_OUT = 1; //time for exit a one man
};

class hellevator {
	public:
		int identificator_of_lift;
		int doors = CLOSED;// at first doors are closed
		int action = EMPTY;        //possible states: empty (default), move, open, wait, idle, close
		int action_start = 0;      //time of start
		int direction = 1;         //possible states: 1 (up, default), -1 (down), 0 (not moving)
		int floor = 0;             //floor number
		set<int> destinations;            // actually destinations
		building_stats parameters;
		
	
		void add_dest(int i) 
		{
			destinations.insert(i);
		};

		void remove_dest(int i) 
		{
			destinations.erase(i);
		}
		hellevator(int el_id, building_stats &stats)
		{
			identificator_of_lift = el_id;

			parameters = stats;
		}

		void step(int gtime) 
		{
			switch(action) 
			{
				case EMPTY:
					if (gtime - action_start == parameters.T_IDLE) 
					{
						action = CLOSE;
						action_start = gtime;
					}
					break;
				case MOVE:
					if (gtime - action_start == parameters.T_STAGE) 
					{
						floor += direction;

						if(destinations.find(floor) != destinations.end()) 
						{
							action = OPEN;
							action_start = gtime;
						}
					}
					break;
				case OPEN:
					if (gtime - action_start == parameters.T_OPEN) 
					{
						action = WAIT;
						action_start = gtime;
						doors = OPENED;
	
					}
					break;
				case CLOSE:
					if (gtime - action_start == parameters.T_CLOSE) 
					{
		
						action_start = gtime;
						doors = CLOSED;
					}
					break;
				 
				default:
					break;
			}
		}
};

void dispatcher(vector<hellevator> hell_vect, int identificator_of_lift) 
{
	
}

int main() 
{
	building_stats stats;

	int gtime = 0;

	vector<hellevator> hell_vect;

	for (int i = 0; i < stats.K; i++) 
	{
		hellevator el(i, stats);
		hell_vect.push_back(el);
	}

	deque<order> orders;
	ifstream input("input.txt");
	string line;
	int identificator_of_lift = 0;

	while(getline(input, line))
	{
		int h, m, s, before, after;

		sscanf(line.c_str(), "%02d:%02d:%02d %d %d", &h, &m, &s, &before, &after);

		int time = h * 3600 + m * 60 + s;

		order ord{ identificator_of_lift++, time, before, after };

		orders.push_back(ord);
	}
	
	vector<thread> el_threads;

	for (int i = 0; i < stats.K; i++) 
	{
		el_threads.push_back(thread(dispatcher, hell_vect, i));
	}

	for(gtime = 0; gtime < 86400 && !orders.empty(); gtime++) 
	{
		if(orders[0].time == gtime) 
		{
	
			orders.pop_front();
		}

	}

	for(auto &t : el_threads) 
	{
		t.join();
	}
}
