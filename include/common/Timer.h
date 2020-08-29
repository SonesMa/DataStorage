//=============================================================================
/**
* @file    Timer.h
* @version v0.1
* @brief   This header contains Timer and StopWatch classes. Timer is used to 
*          call callable event with periodic delay. StopWatch is used to measure
*          elapsed time with high resolution.
*/
//=============================================================================
#pragma once

#include <Windows.h>
#include <Mmsystem.h>
#pragma comment(lib, "Winmm.lib")

#include <functional>
#include <cstdint>

/**
* @brief  Get time of day, mocked function to Unix gettimeofday().
*/
static inline int gettimeofday(struct timeval *tp, void *tzp)
{
	FILETIME ft;
	::GetSystemTimeAsFileTime( &ft );  // never fails
	long long t = (static_cast<long long>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
# if _MSC_VER > 1300 // > VC++ 7.0
	t -= 116444736000000000LL;
# else
	t -= 116444736000000000;
# endif
	t /= 10;  // microseconds
	tp->tv_sec = static_cast<long>( t / 1000000UL);
	tp->tv_usec = static_cast<long>( t % 1000000UL);
	return 0;
}

/**
* Base virtual class for defining TimerTask.
*/
class TimerTask {
public:
	virtual ~TimerTask() {}

	/**
	* @brief  Call this function when timer is triggered.
	*/
	virtual void run() = 0;
};

/**
* Packaging TimerTask so that the same TimerTask object can be shared to Timer
* instead of copying a different object.
*/
class PackagedTimerTask {
public:
	PackagedTimerTask(TimerTask &task):task(task) {}

	/**
	* Override operator() to construct a std::function<void()> functor.
	*/
	void operator()()
	{
		task.run();
	}

private:
	TimerTask &task;
};

/**
* Timer is used to call std::function<void()> functor periodicly when timer is triggered.
* The timer support 1 ms resolution and the periodic delay can be set when constructing.
*/
class Timer {
public:
	/**
	* @param   std::function<void()>[in], callable object with void() prototype.
	*          uint32_t[in], periodic delay in milliseconds.
	**/
	Timer(std::function<void()> func, uint32_t delay_ms):task(func), timer_id(0), delay(delay_ms){}
	Timer(std::function<void()> func):task(func), timer_id(0), delay(1){}

	/**
	* @param   TimerTask[in], reference to a TimerTask object, and this object will be packaged
	*          into PackagedTimerTask.
	*          uint32_t[in], periodic delay in milliseconds.
	**/
	Timer(TimerTask &timer_task, uint32_t delay_ms) 
		:task(PackagedTimerTask(timer_task)), 
		timer_id(0), 
		delay(delay_ms)
	{}
	Timer(TimerTask &timer_task) 
		:task(PackagedTimerTask(timer_task)), 
		timer_id(0), 
		delay(1)
	{}

	~Timer() {
		/**
		* If Internal time is not stopped, stop it.
		*/
		if (timer_id != 0)
		{
			timeKillEvent(timer_id);
		} 
	}

	/**
	* @brief  Set internal timer event and start it.
	*/
	int start()
	{
		if (timer_id != 0)
			stop();

		timer_id = timeSetEvent(delay, 1, (LPTIMECALLBACK)onTimeFunc,
			reinterpret_cast<DWORD_PTR>(this), TIME_PERIODIC);
		if(0 == timer_id)
		{
			return GetLastError();
		}

		return 0;
	}

	/**
	* @brief  Stop internal timer if it is not stopped.
	*/
	void stop()
	{
		if (timer_id != 0)
		{
			timeKillEvent(timer_id);
			timer_id = 0;
		}
	}

private:
	/**
	* Windows LPTIMECALLBACK implementation. This function will call actual task from callable object.
	*/
	static void WINAPI onTimeFunc(UINT wTimerID, UINT msg, DWORD_PTR dwUser, DWORD dwl, DWORD dw2)
	{
		Timer *timer = reinterpret_cast<Timer *>(dwUser);
		timer->task();
	}

	MMRESULT timer_id;
	std::function<void()> task;
	uint32_t delay;
};

/**
* StopWatch is used to measure elapsed time with high resolution.
*/
class StopWatch{
public:
	StopWatch()
	{
		QueryPerformanceFrequency(&frequency);
		is_stopped = false;
	}
	~StopWatch() {}

	/**
	* @brief   Start stopwatch.
	*/
	void start()
	{
		QueryPerformanceCounter(&begin);
		is_stopped = false;
	}

	/**
	* @brief  Stop stopwatch.
	*/
	void restart()
	{
		QueryPerformanceCounter(&begin);
		is_stopped = false;
	}

	/**
	* @brief  Restart this stopwatch.
	*/
	void stop()
	{
		QueryPerformanceCounter(&end);
		is_stopped = true;
	}

	/**
	* @brief  Calculate the elapsed time in milliseconds.
	*/
	double elapsed_ms()
	{
		const int MilliSecondInOneSecond = 1000;
		return elapsed_s() * MilliSecondInOneSecond;
	}

	/**
	* @brief  Calculate the elapsed time in microseconds.
	*/
	double elapsed_us()
	{
		const int MicroSecondInOneSecond = 1000000;
		return elapsed_s() * MicroSecondInOneSecond;
	}

	/**
	* @brief   Calculate the elapsed time in seconds.
	*/
	double elapsed_s()
	{
		if (!is_stopped)
		{
			QueryPerformanceCounter(&end);
		}
		return (double)(end.QuadPart - begin.QuadPart) / (double)frequency.QuadPart;
	}

private:
	LARGE_INTEGER begin;
	LARGE_INTEGER end;
	LARGE_INTEGER frequency;
	bool is_stopped;
};