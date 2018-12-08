#ifndef __PIPE_DEF
#define __PIPE_DEF

#include <unistd.h>

class PipeUse
{
	public:
		PipeUse();
		~PipeUse();

		void write(void* buf, size_t bytes);
		char* read(size_t bytes);

	private:
		int _pipefd[2];
};


#endif
