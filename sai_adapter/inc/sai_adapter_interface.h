#ifndef SAI_ADAPTER_INTERFACE_H
#define SAI_ADAPTER_INTERFACE_H
#include <sai.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef void *S_O_Handle;
S_O_Handle create_sai_adapter();
void free_sai_adapter(S_O_Handle);

// api
sai_status_t sai_adapter_api_query(S_O_Handle, sai_api_t, void **);
#ifdef __cplusplus
}
#endif
#endif