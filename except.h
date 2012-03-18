#ifndef _EXCEPT_INCLUDED
#define _EXCEPT_INCLUDED

#include <setjmp.h>

typedef struct except_T
{
    const char *msg;
} except_T;

typedef struct except_Frame
{
    struct except_Frame *prev;
    jmp_buf env;
    const char *file;
    int line;
    const except_T *exception;
} except_Frame;

enum {except_ENTERED = 0, except_RAISED,
      except_HANDLED, except_FINALIZED};

extern except_Frame *except_stack;

void except_throw(const except_T *e, const char *file, int line);

#define TRY                                     \
do                                              \
{                                               \
    except_Frame except_frame;                  \
    except_frame.prev = except_stack;           \
    except_stack = &except_frame;               \
    volatile int except_flag;                   \
    except_flag = setjmp(except_frame.env);     \
    if (except_flag == except_ENTERED)          \
    {

#define CACHE(e)                                \
        if (except_flag == except_ENTERED)      \
            except_stack = except_stack->prev;  \
    }                                           \
    else if (except_frame.exception == &(e))    \
    {                                           \
        except_flag = except_HANDLED;

#define ELSE                                    \
        if (except_flag == except_ENTERED)      \
            except_stack = except_stack->prev;  \
    }                                           \
    else                                        \
    {                                           \
        except_flag = except_HANDLED;

#define FINALLY                                 \
        if (except_flag == except_ENTERED)      \
            except_stack = except_stack->prev;  \
    }                                           \
    {                                           \
    if (except_flag == except_ENTERED)          \
        except_flag = except_FINALIZED;

#define END_TRY                                 \
        if (except_flag == except_ENTERED)      \
            except_stack = except_stack->prev;  \
    }                                           \
    if (except_flag == except_RAISED)           \
        RETHROW;                                \
} while (0)

#define THROW(e) except_throw(&(e), __FILE__, __LINE__)

#define RETHROW except_throw(except_frame.exception, except_frame.file, except_frame.line)

#define RETURN switch (except_stack = except_stack->prev,0) default: return

#endif
