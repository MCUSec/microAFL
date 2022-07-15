#ifndef __OUTPUT_H__
#define __OUTPUT_H__

extern unsigned int verbosity;

#define DBG_BASIC	1
#define DBG_STREAM	2
#define DBG_PROTO	4
#define DBG_DECODE	8
#define DBG_MASK	0x0f

#define dbg(l, f, args...) \
	do { if ((verbosity & (l)) == (l)) fprintf(stderr, f, ## args); } while (0)

#define SAY(f, args...) \
	fprintf(stdout, f, ## args)
#define DBG(f, args...) \
	fprintf(stderr, f, ## args)
#define ERR(f, args...) \
	fprintf(stderr, "[!!!] " f, ## args)

#endif /* __OUTPUT_H__ */

