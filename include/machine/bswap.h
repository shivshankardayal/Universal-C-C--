/*      $NetBSD: bswap.h,v 1.1 1999/01/15 13:31:24 bouyer Exp $      */

/* Written by Manuel Bouyer. Public domain */

#ifndef _I386_BSWAP_H_
#define _I386_BSWAP_H_

#include <sys/cdefs.h>

__BEGIN_DECLS
#ifdef _KERNEL
u_int16_t       bswap16 __P((u_int16_t));
u_int32_t       bswap32 __P((u_int32_t));
u_int64_t       bswap64 __P((u_int64_t));
#else
u_int16_t       bswap16 __P((u_int16_t)) __RENAME(__bswap16);
u_int32_t       bswap32 __P((u_int32_t)) __RENAME(__bswap32);
u_int64_t       bswap64 __P((u_int64_t)) __RENAME(__bswap64);
#endif
__END_DECLS


#include <machine/byte_swap.h>

#define bswap16(x)      __byte_swap_word(x)
#define bswap32(x)      __byte_swap_long(x)
#define bswap64(x)      __byte_swap_quad(x)

#define bswapBE16(x)      __byte_swap_wordBE(x)
#define bswapBE32(x)      __byte_swap_longBE(x)
#define bswapBE64(x)      __byte_swap_quadBE(x)

#define bswapLE16(x)      __byte_swap_wordLE(x)
#define bswapLE32(x)      __byte_swap_longLE(x)
#define bswapLE64(x)      __byte_swap_quadLE(x)


#endif 

