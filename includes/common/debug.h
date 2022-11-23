#ifndef COMMON_H
#define COMMON_H

// If both NDEBUG and DEBUG are defined DEBUG has priority
// In any case if one is defined the other will not
#ifdef DEBUG
# undef NDEBUG
#else
# define NDEBUG
#endif

#ifdef NDEBUG
# undef DEBUG
#endif

#endif /* COMMON_H */
