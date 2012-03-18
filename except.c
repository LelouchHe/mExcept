#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "except.h"
#include "util.h"

except_Frame *except_stack = NULL;

void except_throw(const except_T *e, const char *file, int line)
{
	except_Frame *p = except_stack;
	assert(e);
	if (p == NULL) 
    {
		fprintf(stderr, "Uncaught exception");
		if (e->msg)
			fprintf(stderr, " %s", e->msg);
		else
			fprintf(stderr, " at 0x%p", e);
        
		if (file && line > 0)
			fprintf(stderr, " raised at %s:%d\n", file, line);
		
        util_dumpstack();
        
        fflush(stderr);
        exit(-1);
	}
	p->exception = e;
	p->file = file;
	p->line = line;
    except_stack = except_stack->prev;
	longjmp(p->env, except_RAISED);
}
