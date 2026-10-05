
``` c
static void *find_fit(size_t size){ // 가능한 위치를 찾아서 payload 포인터 반환
    // first fit
    void *bp = heap_listp;
    while((GET_ALLOC(HDRP(bp)) || GET_SIZE(HDRP(bp)) < size) && GET_SIZE(HDRP(bp)) > 0 ){ // allocated이거나 필요 size보다 작으면 다음 탐색, size가 0이면, epilogue이므로 중단
        bp = NEXT_BLKP(bp);
    } // 조건 만족하는 가장 첫 번째에서 중단

    if(GET_SIZE(HDRP(bp)) == 0) return NULL; // 마지막 도달인 경우

     
    return bp;
}
```