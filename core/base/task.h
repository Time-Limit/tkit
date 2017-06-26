#ifndef _TASK_H_
#define _TASK_H_

class Connector;

class Task
{
public:
	virtual void Exec() = 0;
};

class NetTask
{
public:
	virtual void Exec() = 0;
	void SetConnector(Connector * c)
	{
		connector = c;
	}
private:
	Connector * connector;
};

class TaskCreator
{
public:
	virtual Task * Create() const = 0;
};

#endif
