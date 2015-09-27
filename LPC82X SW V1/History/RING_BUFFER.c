/****************************************************************************
	Luka Penger 2015
	RING BUFFER
****************************************************************************/

#include <RING_BUFFER.h>

#define RB_INDH(rb)                ((rb)->head & ((rb)->count - 1))
#define RB_INDT(rb)                ((rb)->tail & ((rb)->count - 1))

int RingBuffer_Init(RINGBUFF_T *RingBuff, void *buffer, int itemSize, int count) {
	RingBuff->data = buffer;
	RingBuff->count = count;
	RingBuff->itemSz = itemSize;
	RingBuff->head = RingBuff->tail = 0;

	return 1;
}

void RingBuffer_Flush(RINGBUFF_T *RingBuff) {
	RingBuff->head = RingBuff->tail = 0;
}

int RingBuffer_GetSize(RINGBUFF_T *RingBuff) {
	return RingBuff->count;
}

int RingBuffer_GetCount(RINGBUFF_T *RingBuff) {
	return RB_VHEAD(RingBuff) - RB_VTAIL(RingBuff);
}

int RingBuffer_GetFree(RINGBUFF_T *RingBuff) {
	return RingBuff->count - RingBuffer_GetCount(RingBuff);
}

int RingBuffer_IsFull(RINGBUFF_T *RingBuff) {
	return (RingBuffer_GetCount(RingBuff) >= RingBuff->count);
}

int RingBuffer_IsEmpty(RINGBUFF_T *RingBuff) {
	return RB_VHEAD(RingBuff) == RB_VTAIL(RingBuff);
}

/* Insert a single item into Ring Buffer */
int RingBuffer_Insert(RINGBUFF_T *RingBuff, const void *data) {
	uint8_t *ptr = RingBuff->data;

	/* We cannot insert when queue is full */
	if (RingBuffer_IsFull(RingBuff))
		return 0;

	ptr += RB_INDH(RingBuff) * RingBuff->itemSz;
	memcpy(ptr, data, RingBuff->itemSz);
	RingBuff->head++;

	return 1;
}

/* Insert multiple items into Ring Buffer */
int RingBuffer_InsertMult(RINGBUFF_T *RingBuff, const void *data, int num)
{
	uint8_t *ptr = RingBuff->data;
	int cnt1, cnt2;

	/* We cannot insert when queue is full */
	if (RingBuffer_IsFull(RingBuff))
		return 0;

	/* Calculate the segment lengths */
	cnt1 = cnt2 = RingBuffer_GetFree(RingBuff);
	if (RB_INDH(RingBuff) + cnt1 >= RingBuff->count)
		cnt1 = RingBuff->count - RB_INDH(RingBuff);
	cnt2 -= cnt1;

	cnt1 = MIN(cnt1, num);
	num -= cnt1;

	cnt2 = MIN(cnt2, num);
	num -= cnt2;

	/* Write segment 1 */
	ptr += RB_INDH(RingBuff) * RingBuff->itemSz;
	memcpy(ptr, data, cnt1 * RingBuff->itemSz);
	RingBuff->head += cnt1;

	/* Write segment 2 */
	ptr = (uint8_t *) RingBuff->data + RB_INDH(RingBuff) * RingBuff->itemSz;
	data = (const uint8_t *) data + cnt1 * RingBuff->itemSz;
	memcpy(ptr, data, cnt2 * RingBuff->itemSz);
	RingBuff->head += cnt2;

	return cnt1 + cnt2;
}

int RingBuffer_Pop(RINGBUFF_T *RingBuff, void *data) {
	uint8_t *ptr = RingBuff->data;

	/* We cannot pop when queue is empty */
	if (RingBuffer_IsEmpty(RingBuff))
		return 0;

	ptr += RB_INDT(RingBuff) * RingBuff->itemSz;
	memcpy(data, ptr, RingBuff->itemSz);
	RingBuff->tail++;

	return 1;
}

int RingBuffer_PopMult(RINGBUFF_T *RingBuff, void *data, int num) {
	uint8_t *ptr = RingBuff->data;
	int cnt1, cnt2;

	/* We cannot insert when queue is empty */
	if (RingBuffer_IsEmpty(RingBuff))
		return 0;

	/* Calculate the segment lengths */
	cnt1 = cnt2 = RingBuffer_GetCount(RingBuff);
	if (RB_INDT(RingBuff) + cnt1 >= RingBuff->count)
		cnt1 = RingBuff->count - RB_INDT(RingBuff);
	cnt2 -= cnt1;

	cnt1 = MIN(cnt1, num);
	num -= cnt1;

	cnt2 = MIN(cnt2, num);
	num -= cnt2;

	/* Write segment 1 */
	ptr += RB_INDT(RingBuff) * RingBuff->itemSz;
	memcpy(data, ptr, cnt1 * RingBuff->itemSz);
	RingBuff->tail += cnt1;

	/* Write segment 2 */
	ptr = (uint8_t *) RingBuff->data + RB_INDT(RingBuff) * RingBuff->itemSz;
	data = (uint8_t *) data + cnt1 * RingBuff->itemSz;
	memcpy(data, ptr, cnt2 * RingBuff->itemSz);
	RingBuff->tail += cnt2;

	return cnt1 + cnt2;
}

/****************************************************************************
                            End Of File
****************************************************************************/
