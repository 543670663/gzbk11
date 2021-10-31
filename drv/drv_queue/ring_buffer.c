#include "ring_buffer.h"
#include <string.h>

#define RB_INDH(rb)                ((rb)->head & ((rb)->count - 1))
#define RB_INDT(rb)                ((rb)->tail & ((rb)->count - 1))

#define MIN(a, b)                   (a) > (b) ? (b) : (a)

/* Initialize ring buffer */
int RingBufInit(RingBufStruct *RingBuff, void *buffer, unsigned int itemSize, unsigned int count)
{
	RingBuff->data = buffer;
	RingBuff->count = count;
	RingBuff->itemSize = itemSize;
	RingBuff->head = RingBuff->tail = 0;

	return 1;
}

/* Insert a single item into Ring Buffer */
int RingBufInsert(RingBufStruct *RingBuff, const void *data)
{
	uint8_t *ptr = RingBuff->data;

	/* We cannot insert when queue is full */
	if (IsRingBufFull(RingBuff))
		return 0;

	ptr += RB_INDH(RingBuff) * RingBuff->itemSize;
	memcpy(ptr, data, RingBuff->itemSize);
	RingBuff->head++;

	return 1;
}

/* Insert multiple items into Ring Buffer */
int RingBufInsertMult(RingBufStruct *RingBuff, const void *data, unsigned int num)
{
	uint8_t *ptr = RingBuff->data;
	int cnt1, cnt2;

	/* We cannot insert when queue is full */
	if (IsRingBufFull(RingBuff))
		return 0;

	/* Calculate the segment lengths */
	cnt1 = cnt2 = GetRingBufFree(RingBuff);
	if (RB_INDH(RingBuff) + cnt1 >= RingBuff->count)
		cnt1 = RingBuff->count - RB_INDH(RingBuff);
	cnt2 -= cnt1;

	cnt1 = MIN(cnt1, num);
	num -= cnt1;

	cnt2 = MIN(cnt2, num);
	num -= cnt2;

	/* Write segment 1 */
	ptr += RB_INDH(RingBuff) * RingBuff->itemSize;
	memcpy(ptr, data, cnt1 * RingBuff->itemSize);
	RingBuff->head += cnt1;

	/* Write segment 2 */
	ptr = (uint8_t *) RingBuff->data + RB_INDH(RingBuff) * RingBuff->itemSize;
	data = (const uint8_t *) data + cnt1 * RingBuff->itemSize;
	memcpy(ptr, data, cnt2 * RingBuff->itemSize);
	RingBuff->head += cnt2;

	return cnt1 + cnt2;
}

/* Pop single item from Ring Buffer */
int RingBufPop(RingBufStruct *RingBuff, void *data)
{
	uint8_t *ptr = RingBuff->data;

	/* We cannot pop when queue is empty */
	if (IsRingBufEmpty(RingBuff))
		return 0;

	ptr += RB_INDT(RingBuff) * RingBuff->itemSize;
	memcpy(data, ptr, RingBuff->itemSize);
	RingBuff->tail++;

	return 1;
}

/* Pop multiple items from Ring buffer从环形队类弹出多个项目 */
int RingBufPopMult(RingBufStruct *RingBuff, void *data, int num)
{
	uint8_t *ptr = RingBuff->data;
	int cnt1, cnt2;

	/* We cannot insert when queue is empty */
	if (IsRingBufEmpty(RingBuff))
		return 0;

	/* Calculate the segment lengths */
	cnt1 = cnt2 = GetRingBufCount(RingBuff);
	if (RB_INDT(RingBuff) + cnt1 >= RingBuff->count)
		cnt1 = RingBuff->count - RB_INDT(RingBuff);
	cnt2 -= cnt1;

	cnt1 = MIN(cnt1, num);
	num -= cnt1;

	cnt2 = MIN(cnt2, num);
	num -= cnt2;

	/* Write segment 1 */
	ptr += RB_INDT(RingBuff) * RingBuff->itemSize;
	memcpy(data, ptr, cnt1 * RingBuff->itemSize);
	RingBuff->tail += cnt1;

	/* Write segment 2 */
	ptr = (uint8_t *) RingBuff->data + RB_INDT(RingBuff) * RingBuff->itemSize;
	data = (uint8_t *) data + cnt1 * RingBuff->itemSize;
	memcpy(data, ptr, cnt2 * RingBuff->itemSize);
	RingBuff->tail += cnt2;

	return cnt1 + cnt2;
}
