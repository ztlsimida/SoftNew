#include "typesdef.h"
#include "errno.h"
#include "list.h"
#include "osal/string.h"
#include "osal/msgqueue.h"
#include "csi_kernel.h"

#define MSGQ_MAGIC (0x4a8b1c9d)

int32 os_msgq_init(struct os_msgqueue *msgq, int32 size)
{
    if(msgq->magic == MSGQ_MAGIC){
        os_printf(KERN_WARNING"msgq repeat initialization ????\r\n");
    }
    msgq->hdl = csi_kernel_msgq_new(size, sizeof(uint32));
    if(msgq->hdl) msgq->magic = MSGQ_MAGIC;
    return (msgq->hdl ? RET_OK : RET_ERR);
}

uint32 os_msgq_get(struct os_msgqueue *msgq, int32 tmo_ms)
{
    uint32 val = 0;
    csi_kernel_msgq_get(msgq->hdl, &val, csi_kernel_ms2tick(tmo_ms));
    return val;
}

uint32 os_msgq_get2(struct os_msgqueue *msgq, int32 tmo_ms, int32 *err)
{
    uint32 val = 0;
    k_status_t ret = csi_kernel_msgq_get(msgq->hdl, &val, csi_kernel_ms2tick(tmo_ms));
    if(err) *err = ret;
    return val;
}

int32 os_msgq_put(struct os_msgqueue *msgq, uint32 data, int32 tmo_ms)
{
    return csi_kernel_msgq_put(msgq->hdl, &data, 0, csi_kernel_ms2tick(tmo_ms));
}

int32 os_msgq_put_head(struct os_msgqueue *msgq, uint32 data, int32 tmo_ms)
{
    return csi_kernel_msgq_put(msgq->hdl, &data, 0, csi_kernel_ms2tick(tmo_ms));
}

int32 os_msgq_del(struct os_msgqueue *msgq)
{
    ASSERT(msgq->hdl);
    csi_kernel_msgq_del(msgq->hdl);
    msgq->hdl = NULL;
    msgq->magic = 0;
    return RET_OK;
}

int32 os_msgq_cnt(struct os_msgqueue *msgq)
{
    return csi_kernel_msgq_get_count(msgq->hdl);
}

