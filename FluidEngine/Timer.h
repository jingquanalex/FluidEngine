#pragma once

#include <vector>
#include <algorithm>

class Timer
{

private:

	static std::vector<Timer*> listTimers;

public:

	static void updateTimers(float dt);

private:

	bool toDelete = 0;
	float currentTime = 0.0f;
	float tickInterval;
	float tickDuration;
	float durationLeft;
	bool isRunning = false;
	bool hasTicked = false;

	bool getToDelete() const;

public:

	Timer(float interval, float duration = 0);
	~Timer();

	// === Functions ===

	void update(float dt);
	void destroy();
	bool ticked();
	void start();
	void stop();
	void reset();

	// === Accessors ===

	float getTickInterval() const;
	float getDuration() const;
	bool getIsRunning() const;

	void setTickInterval(float interval);
	
};