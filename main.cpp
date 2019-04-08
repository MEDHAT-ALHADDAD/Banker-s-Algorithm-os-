#define _CRT_SECURE_NO_WARNINGS

#include "iostream"

#include "string"

#include "vector"

#include "tuple"

#include "ctime"

#include "iomanip"

using namespace std;



#pragma region prototypes

void init();	//resizes vectors to the needed sizes

void random_fill();	//fills in the Max matrix randomly

void available_fill();	//input for available matrix & determining max available resources

bool safe_state(tuple<string, vector<int>, vector<int>, vector<int>, bool> job);	//checks whether the created matrices are in safe state to begin or not



vector<int> generate_resources();	//generates a random resource matrix

void assign_request();	//assigns a request matrix to a process in the global data process_request

void assign_release();	//assigns a release matrix to a process in the global data process_release



bool accepted_request();	//checks whether the request is accepted or not(causes unsafe state)

bool accepted_release();	//checks whether the release is accepted or not(causes unsafe state)



void print_jobs(vector<tuple<string, vector<int>, vector<int>, vector<int>, bool>> jobs);	//prints all processes and their prespective matrices

void print_request();	//prints the request matrix and it's prespective process

void print_release();	//prints the release matrix and it's prespective process

#pragma endregion



#pragma region Global data

						//Termination_limit: no. of requests before the process is terminated, Limit: Max no. of resources

const int termination_limit = 5, available_limit = 1000;

int tests, rTypes, till_termination, release_counter, max_sum_available, show, Resources_limit, reason_request, reason_release;



//tuple guide:: (0:Process name, 1:Allocation, 2:Max, 3:Need, 4:Working state)

vector<tuple<string, vector<int>, vector<int>, vector<int>, bool>> jobs;	//vector of tuples linking: process name, allocation, Max, Need, Working state together



vector<int> available, freq, lim;	//available resources, vector determining how many times a process has requested resources

vector<string> safe_route;	//for printing safe route



pair<string, vector<int>> process_request, process_release;

#pragma endregion





int main()

{

	srand(time(0));

#pragma region Input

	//Max process:10, Max resource types: 6

	do

	{

		printf("Enter number of processes: ");

		cin >> tests;

		while (cin.fail())

		{

			cin.clear(); // clear input buffer to restore cin to a usable state

			cin.ignore(100, '\n'); // ignore last input

			cout << "You can only enter numbers.\n";

			cout << "Enter number of processes: ";

			cin >> tests;

		}

		if (tests > 10)

			printf("Number of processes too huge! Please try again(Max: 10)\n");

		else if (tests < 1)

			printf("Number of processes too small! Please try again(Min: 1)\n");

	} while (tests > 10 || tests < 1);

	do

	{

		printf("Enter number of resource types: ");

		cin >> rTypes;

		while (cin.fail())

		{

			cin.clear(); // clear input buffer to restore cin to a usable state

			cin.ignore(100, '\n'); // ignore last input

			cout << "You can only enter numbers.\n";

			cout << "Enter number of resource types: ";

			cin >> rTypes;

		}

		if (rTypes > 6)

			printf("Number of resource types too huge! Please try again(Max: 6)\n");

		else if (rTypes < 1)

			printf("Number of resource too small! Please try again(Min: 1)\n");

	} while (rTypes > 6 || rTypes < 1);

	init();

	available_fill();

	do

	{

		printf("do you want to show unsafe state requests?(1 == Yes, 0 == No) [low performance] : ");

		cin >> show;

		while (cin.fail())

		{

			cin.clear(); // clear input buffer to restore cin to a usable state

			cin.ignore(100, '\n'); // ignore last input

			cout << "You can only enter numbers.\n";

			cout << "do you want to show unsafe state requests?(1 == Yes, 0 == No): ";

			cin >> show;

		}

		if (show != 0 && show != 1)

			printf("You can only enter 1 or 0\n");

	} while (show != 0 && show != 1);



	vector<int> temp = available;

	bool safe = false;

	do

	{

		random_fill();

		for (int i = 0; i < tests; ++i)

		{

			safe = safe_state(jobs[i]);

			if (!safe)

			{

				printf("Unsafe state! Trying new initialization.\n");

				break;

			}

		}

	} while (!safe);



	do

	{

		assign_request();

	} while (!accepted_request());



	printf("Initial state:");

	print_jobs(jobs);

	printf("____________________________________________________________________\n");

	print_request();

	printf("____________________________________________________________________\n");

#pragma endregion



#pragma region Functionality

	int process_index = -1;

	int release_index = -1;

	release_counter = 0;

	while (till_termination < tests * 5)

	{

		process_index = (int)(process_request.first.at(1) - '0');

		safe_route[till_termination] = process_request.first;



		//if process didn't do (termination_limit) of requests yet complete the request

		if (freq[process_index] < termination_limit)

		{

			for (int i = 0; i < rTypes; ++i)

			{

				get<1>(jobs[process_index])[i] += process_request.second[i];

				get<3>(jobs[process_index])[i] -= process_request.second[i];

				available[i] -= process_request.second[i];

			}

			freq[process_index]++;

			till_termination++;

		}



		//Redeem the process terminated and release the resources allocated in it

		else

		{

			get<4>(jobs[process_index]) = false;

			for (int i = 0; i < rTypes; ++i)

			{

				available[i] += get<1>(jobs[process_index])[i];

				get<2>(jobs[process_index])[i] = 0;

				get<3>(jobs[process_index])[i] = get<2>(jobs[process_index])[i];

			}

		}

		print_jobs(jobs);



		//make a valid request

		do

		{

			assign_request();

			if (!accepted_request() && show == 1)

			{

				print_request();

				printf("Unsafe state!  Ignoring request. ");

				switch (reason_request)

				{

				case 1: printf("(Reason: Request > Need)\n");

					break;

				case 2: printf("(Reason: Request > Available)\n");

					break;

				case 3: printf("(Reason: Request of zeros)\n");

					break;

				default: printf("\n");
					break;

				}

			}

			//only release when available isn't maxed

			int sum = 0;

			for (auto av : available) sum += av;

			if (sum != max_sum_available)

			{

				//release randomly

				if (rand() % 2)

				{

					//make a valid release

					do

					{

						assign_release();

						if (!accepted_release() && show == 1)

						{

							print_release();

							printf("Unsafe state!  Ignoring release. ");

							switch (reason_release)

							{

							case 1: printf("(Reason: Release + Available > Max Available)\n");

								break;

							case 2: printf("(Reason: Release + Need > Max)\n");

								break;

							case 3: printf("(Reason: Allocation - Release < 0)\n");

								break;

							case 4: printf("(Reason: Release of zeros)\n");

								break;

							default: printf("\n");
								break;

							}

						}

					} while (!accepted_release());

					printf("________________________________________________________\n");

					print_release();



					//Release operations

					release_index = (int)(process_release.first.at(1) - '0');

					for (int i = 0; i < rTypes; ++i)

					{

						get<1>(jobs[release_index])[i] -= process_release.second[i];

						get<3>(jobs[release_index])[i] += process_release.second[i];

						available[i] += process_release.second[i];

					}

				}

			}

		} while (!accepted_request());

		print_request();

		printf("________________________________________________________________\n");

	}

	get<4>(jobs[process_index]) = false;

	printf("Process %s freed all resources at termination", safe_route[till_termination - 1]);

	available = temp;

	print_jobs(jobs);

#pragma endregion



#pragma region Safe route printing

	//printing of the safe state route the processes took

	cout << "\nSafe route: ";

	for (int i = 0; i < safe_route.size(); ++i)

	{

		if (i < safe_route.size() - 1) cout << safe_route[i] << " -> ";

		else cout << safe_route[i];

	}

	cout << endl;

#pragma endregion

	return 0;

}



vector<int> generate_resources()

{
	vector<int> resources(rTypes);

	for (int i = 0; i < rTypes; ++i)

	{

		resources[i] = rand() % lim[i];

	}

	return resources;

}

void assign_request()

{

	int random_process = rand() % tests;



	//avoid requests of terminated processes

	bool working = get<4>(jobs[random_process]);

	while (!working)

	{

		random_process = rand() % tests;

		working = get<4>(jobs[random_process]);

	}



	process_request.first = get<0>(jobs[random_process]);

	process_request.second = generate_resources();

}

void assign_release()

{

	int releasing_process = rand() % tests;



	//avoid release of terminated processes

	bool working = get<4>(jobs[releasing_process]);

	while (!working)

	{

		releasing_process = rand() % tests;

		working = get<4>(jobs[releasing_process]);

	}



	process_release.first = get<0>(jobs[releasing_process]);

	process_release.second = generate_resources();

}



bool accepted_request()

{

	int requesting_process = (int)(process_request.first.at(1) - '0');



	//1. Request < Need(Max- Alloc)

	for (int i = 0; i < rTypes; ++i)

	{

		if (process_request.second[i] > get<3>(jobs[requesting_process])[i])

		{

			if (show == 1) reason_request = 1;

			return false;

		}

	}

	//2. Request < Available

	for (int i = 0; i < rTypes; ++i)

	{

		if (process_request.second[i] > available[i])

		{

			if (show == 1) reason_request = 2;

			return false;

		}

	}

	//3. Request is Zeros

	int sum = 0;

	for (auto req : process_request.second) sum += req;

	if (sum == 0)

	{

		if (show == 1) reason_request = 3;

		return false;

	}

	//4. Available - Request > 0

	vector<int> unsafe(rTypes);

	for (int i = 0; i < rTypes; i++) unsafe[i] = available[i] - process_request.second[i];



	//abailable <= zeroes then roll back

	int sum_unsafe = 0;

	for (auto uns : unsafe) sum_unsafe += uns;

	if (sum_unsafe <= 0)

	{

		//printf("Rolling back! unsafe Request < ");

		for (auto req : process_request.second) printf("%i ", req);

		printf(">\n");

		return false;

	}

	return true;

}

bool accepted_release()

{

	int releasing_process = (int)(process_release.first.at(1) - '0');



	//if rand fails to get correct release then just release all allocation of the process after 20 tries

	if (release_counter == 20)

	{

		process_release.second = get<1>(jobs[releasing_process]);

		release_counter = 0;

	}



	//1. Release + available <= limit

	for (int i = 0; i < rTypes; ++i)

	{

		if (process_release.second[i] + available[i] > lim[i])

		{

			//printf("Available exceeded limit\n");

			if (show == 1) reason_release = 1;

			release_counter++;

			return false;

		}

	}

	//2. Release + Need <= Max

	for (int i = 0; i < rTypes; ++i)

	{

		if (process_release.second[i] + get<3>(jobs[releasing_process])[i] > get<2>(jobs[releasing_process])[i])

		{

			//printf("Need > Max\n");

			if (show == 1) reason_release = 2;

			release_counter++;

			return false;

		}

	}

	//3. Allocation - Release >= 0

	for (int i = 0; i < rTypes; ++i)

	{

		if (get<1>(jobs[releasing_process])[i] - process_release.second[i] < 0)

		{

			//printf("Allocation < 0\n");

			if (show == 1) reason_release = 3;

			release_counter++;

			return false;

		}

	}



	//4. Release is Zeros

	int sum = 0;

	for (auto rel : process_release.second) sum += rel;

	if (sum <= 0)

	{

		//printf("Release is zeroes\n");

		if (show == 1) reason_release = 4;

		release_counter++;

		return false;

	}



	release_counter = 0;

	return true;

}



void init()

{

	int temp = tests;

	jobs.resize(tests);

	freq.resize(tests);

	safe_route.resize(tests * 5);

	available.resize(rTypes);

	lim.resize(rTypes);



	//Setting processes names and resizing the vectors

	while (temp--)

	{

		get<0>(jobs[temp]) = "P" + to_string(temp);

		get<1>(jobs[temp]).resize(rTypes);

		get<2>(jobs[temp]).resize(rTypes);

		get<3>(jobs[temp]).resize(rTypes);

		get<4>(jobs[temp]) = true;

	}

}

void random_fill()

{

	//Allocation

	for (int jobIterator = 0; jobIterator < jobs.size(); ++jobIterator)

	{

		for (int i = 0; i < rTypes; ++i)

		{

			get<1>(jobs[jobIterator])[i] = 0;

		}

	}



	//Max

	for (int jobIterator = 0; jobIterator < jobs.size(); ++jobIterator)

	{

		int sum_max_zero = 0;

		do

		{

			for (int i = 0; i < rTypes; ++i)

			{

				get<2>(jobs[jobIterator])[i] = rand() % lim[i];

			}

			for (auto m : get<2>(jobs[jobIterator])) sum_max_zero += m;

		} while (sum_max_zero == 0);

	}

	//Need

	for (int jobIterator = 0; jobIterator < jobs.size(); ++jobIterator)

	{

		for (int i = 0; i < rTypes; ++i)

		{

			get<3>(jobs[jobIterator])[i] = get<2>(jobs[jobIterator])[i] - get<1>(jobs[jobIterator])[i];

		}

	}

}

void available_fill()

{

	int temp;

	do {

		//Available

		temp = 0;

		printf("Enter available resources (%i resource types): ", rTypes);

		for (int i = 0; i < rTypes; ++i)

		{

			cin >> available[i];

			while (cin.fail())

			{

				cin.clear(); // clear input buffer to restore cin to a usable state

				cin.ignore(100, '\n'); // ignore last input

				cout << "You can only enter numbers.\n";

				printf("Enter available resources (%i resource types): ", rTypes);

				cin >> available[i];

			}

			if (lim[i] < available[i])	lim[i] = available[i] + 2;



			max_sum_available += available[i];

		}

		for (int i = 0; i < rTypes; ++i)

		{

			if (available[i] <= 0)

			{

				printf("You can only enter positive number\n");

				temp = -1;

				break;

			}

		}

	} while (temp == -1);

}

bool safe_state(tuple<string, vector<int>, vector<int>, vector<int>, bool> job)

{

	// need < available

	for (int i = 0; i < rTypes; i++)

		if (get<3>(job)[i] > available[i])

			return false;



	return true;

}



void print_jobs(vector<tuple<string, vector<int>, vector<int>, vector<int>, bool>> jobs)

{

	cout << endl;

	bool first_time = true;

	//Printing format as in banker's algorithm

	printf("____________________________________________________________________\n");



	for (auto j : jobs)

	{

		if (first_time)

		{



			cout << ">\tAvailable< ";

			for (auto a : available)

			{

				cout << setw(3) << a << " ";

			}

			cout << ">" << endl;

			first_time = false;

		}

		if (get<4>(j))

		{

			cout << setw(5) << get<0>(j);



			//Allocations

			cout << "   " << "Alloc< ";

			for (auto res : get<1>(j))

			{

				cout << setw(3) << res << " ";

			}

			cout << ">\t";



			//Max

			cout << "Max< ";

			for (auto need : get<2>(j))

			{

				cout << setw(3) << need << " ";

			}

			cout << ">\t";



			//Need

			cout << "Need< ";

			for (auto need : get<3>(j))

			{

				cout << setw(3) << need << " ";

			}

			cout << ">" << endl;

		}

		else printf("Process P%i Terminated!\n", get<0>(j).at(1) - '0');

	}

	printf("____________________________________________________________________\n");

	cout << endl;

}

void print_request()

{

	cout << "Request: " << process_request.first << " < ";

	for (auto r : process_request.second)

	{

		cout << r << " ";

	}

	cout << ">" << endl;

}

void print_release()

{

	printf("Process (P%i) Released resources < ", (int)(process_release.first.at(1) - '0'));

	for (auto r : process_release.second) printf("%i ", r);

	printf(">\n");

}
