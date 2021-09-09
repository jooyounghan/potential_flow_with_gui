#pragma once
#include <chrono>
#include <iostream>

class timer
{
private :
	std::chrono::steady_clock::time_point start_time;
	std::chrono::steady_clock::time_point end_time;

public :
	void start() { start_time = std::chrono::steady_clock::now(); }
	void end() { end_time = std::chrono::steady_clock::now(); }
	void elapsed()
	{
		std::chrono::duration<double> elapsed_seconds = end_time - start_time;
		std::cout << elapsed_seconds.count() << std::endl;
	}
};