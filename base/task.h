#ifndef __TASK_H_
#define __TASK_H_

class Task
{
public:
	Task() = 0;
	~Task() = 0;
	virtual void Exec() = 0;
};

class IOTask : public Task
{
public:
	IOTask() = 0;
	~IOTask() = 0;
	void Exec() = 0;
private:
	MutexLock lock;
	int fd;
	Octets rdata, wdata;
	flag_t flag;
private:
	bool CanRead();
	bool CanWrite();
	int Read();
	int Write();
};

#endif
