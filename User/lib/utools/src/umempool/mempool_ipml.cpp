#include "mempool_ipml.h"

#if UTOOLS_MEMPOLL_EANBLE == UTOOLS_TRUE
::utools::pool::Mempool<UTOOLS_MEMPOLL_CHUNK_SIZE> global_mempool;
#endif // UTOOLS_MEMPOLL_EANBLE