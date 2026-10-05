/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

// static은 구현 파일 안에서만 전방 선언,,  가시성 제한해야 하고, 다른 파일에서 의미 없음
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t size);
static void *place(void *bp, size_t size);
/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "jungle-5",
    /* First member's full name */
    "Chan Park",
    /* First member's email address */
    "parkchan37@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""};

#define WSIZE 4             // header, footer도 word size
#define DSIZE 8             // dword size in bytes
#define CHUNKSIZE (1 << 12) // extend heap by this amount (bytesx)

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define ABS(x, y) ((x) > (y) ? (x - y) : (y - x) )

// footer 없는 구현용 
#define BIT_PREV_FREE 0x2
#define BIT_ALLOCATED 0x1
// alocc bit를 OR 연산으로 합치기, - 적어도 짝수 워드 정렬 되어야 
#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

#define GET_SIZE(p) (GET(p) & ~0x7) // 헤더에서 size 읽어오기. pointer가 헤더/footer 위치여야 함
#define GET_ALLOC(p) (GET(p) & BIT_ALLOCATED) // 헤더에서 alloc_stat 읽어오기. pointer가 헤더/footer 위치여야 함

#define GET_PREV_BLOCK_FREE(p) (GET(p) & BIT_PREV_FREE) // 헤더에서 이전 블록에 footer있는지(free인지) 확인하기. pointer가 header 위치여야 함

#define HDRP(bp) (((char *)(bp)) - WSIZE)                      // bp는 payload의 주소, word 만큼 뒤로가기
#define FTRP(bp) (((char *)(bp)) + GET_SIZE(HDRP(bp)) - DSIZE) // 시작점이 payload이니, word 두번 뒤로가야 // 상수 연산이면 최적화하지 않나? DSIZE = 2 * WSIZE

#define NEXT_BLKP(bp) (((char *)(bp)) + GET_SIZE((((char *)(bp)) - WSIZE))) // 다음 chunk payload 주소
#define PREV_BLKP(bp) (((char *)(bp)) - GET_SIZE((((char *)(bp)) - DSIZE))) // 이전 chunk payload 주소 -> 이전 블록이 free일때만 가능
/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7) // LSB에서 3번째까지 0이니, 잘라내는

// 왜 함수 안쓰고 매크로?
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))





static void *heap_listp; // prolog 블록의 bp

static void *coalesce(void *bp){ // 상수 시간이면서, 사이에 free chunk 두개 이상 없는 상태 유지 가능, live 객체 이동 못하므로, 추가 선택지 없음
    size_t prev_alloc = (GET_PREV_BLOCK_FREE(HDRP(bp))) ? 0 : 1;
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    // 좌 우 케이스별 로직
    if (prev_alloc && next_alloc) {  // coalesce 할 것 없음
        ;
    }else if (!prev_alloc && next_alloc){ // 이전 청크가 free 
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        // header 넣기

        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0|(GET_PREV_BLOCK_FREE(HDRP(PREV_BLKP(bp))))));
        // footer 넣기
        PUT(FTRP(bp), PACK(size, 0|(GET_PREV_BLOCK_FREE(HDRP(PREV_BLKP(bp))))));

        bp = PREV_BLKP(bp); 
    }else if (prev_alloc && !next_alloc){ // 다음 chunk가 free 
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        //header 넣기
        PUT(HDRP(bp), PACK(size, 0));
        //footer 넣기
        //PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0)); // TODO: 교재 코드하고 다른데, 이게 맞는것 같다 - 아님. HEADER가 이미 바뀌었기 때문에 합친 크기만큼 이동함/ 모두 맞는
        PUT(FTRP(bp), PACK(size, 0));
    }else { // 양쪽 chunk free
        size = size + GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0|(GET_PREV_BLOCK_FREE(HDRP(PREV_BLKP(bp))))));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0|(GET_PREV_BLOCK_FREE(HDRP(PREV_BLKP(bp))))));
        bp = PREV_BLKP(bp);
    }
    
    return bp;
}



static void *extend_heap(size_t words) // payload 주소 반환
{
    char *bp;
    size_t size;

    // 짝수 word로 align하기 위해  반올림
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1) // 확장 실패
                                           // 성공시 bp는 start address of the new area
        return NULL;

    //bp += WSIZE; // mem_sbrk는 첫 주소 반환하니, head만큼 offset 줘야?  하니 8바이트 정렬 안된다 // 그리고 segfault,, epilogue가 word 사이즈이고, header로 덧씌우면 된다. 그러니 bp + WSIZE 필요 없다

    // free한 후에 다음 블록에 free인지 플래그 붙여줌. 그러니 epilogue에 flag 있을 것,, 
    PUT(HDRP(bp), PACK(size, 0|(GET_PREV_BLOCK_FREE(HDRP(bp))))); // epilogue의 prev free flag를 사용해 그 전 블록 free 여부 붙여넣기
    PUT(FTRP(bp), PACK(size, 0|(GET_PREV_BLOCK_FREE(HDRP(bp)))));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, BIT_PREV_FREE|BIT_ALLOCATED)); // epilogue 만들어서 덧씌움

    return coalesce(bp); // 새 bp 이전 블록이 free였으면, coalesce해야
}







/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{

    // 메모리 시스템에서 4워드를 가져와서 빈 free list 만들 수 있도록 초기화
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1) // FFFF... -> ==로 가능?
        return -1;
    // 에필로그, 프롤로그
    PUT(heap_listp, 0);
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, BIT_ALLOCATED | 0));
    // PUT(heap_listp + (2 * WSIZE), 0 /*PACK(DSIZE, 1)*/);// => prologue도 allocated이니 header만 필요, 그런데 payload 정렬해야하므로 적어도 8바이트는 할당해야 
    PUT(heap_listp + (3 * WSIZE), PACK(0, BIT_ALLOCATED | 0)); // epilogue는 header만 필요 - prologue가 allocated이니

    heap_listp += (2 * WSIZE);
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;

    return 0;
}

static void *place(void *bp, size_t size) { //size는 footer와 header를 모두 포함 
    
    
    // place는 사용할 블록이니, mm_malloc에서만 사용,, - allocated 실수 방어용
    if(GET_ALLOC(HDRP(bp))) return NULL; //TODO: caller가 NULL 예외 처리해야 함

    size_t free_chunk_size = GET_SIZE(HDRP(bp));
    size_t size_left = free_chunk_size - size;
    // alloc 빈 size - H/ F 둘 다 들어갈 수 있는 크기여야

    if(size_left >= 2*DSIZE){ // dword 정렬이니, 적어도 DSIZE만큼 있거나 꽉 찰 것 예상 // TODO: 검증해야함
        PUT(FTRP(bp), PACK(size_left, 0 )); // 이전 푸터 여부도 0, place할 것이 앞에 있으므로
        PUT(FTRP(bp) - (size_left - WSIZE) , PACK(size_left, 0));
    }
    else { // free block 없는 
        int* nptr = (int *)(HDRP(NEXT_BLKP(bp)));
        (*nptr) &= ~(BIT_PREV_FREE);
    }
    
    PUT(HDRP(bp), PACK(size, BIT_ALLOCATED|(GET_PREV_BLOCK_FREE(HDRP(bp))) ));    
    return bp;
}

static void *find_fit(size_t size){ // 가능한 위치를 찾아서 payload 포인터 반환
    // best fit w/ brute force a

    void *bp = heap_listp;
    size_t min_diff = (size_t) -1;
    void *min_ptr;

    while(GET_SIZE(HDRP(bp)) > 0 ){ // allocated이거나 필요 size보다 작으면 다음 탐색, size가 0이면, epilogue이므로 중단
        if((GET_ALLOC(HDRP(bp)) || GET_SIZE(HDRP(bp)) < size)) {
            ;
        }else {
            size_t diff = GET_SIZE(HDRP(bp)) - size;
            if(diff == 0) {
                min_diff = diff;
                min_ptr = bp;
                break;
            }

            if(diff < min_diff){
                min_diff = diff;
                min_ptr = bp;
            }
        }
        bp = NEXT_BLKP(bp);
    }

    if(min_diff == (size_t) -1) return NULL; // 마지막 도달인 경우
    
     
    return min_ptr;
}



/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{

    if(size == 0){ //unsigned 
        return NULL;
    }

    size_t adjusted_size;
    char *bp;

    // adjusted size는 header/footer 공간 오버헤드(2word)까지 필요. => 항상 짝수단위만큼 && DSIZE 추가해서 계산 - 오버헤드까지 포함할 수 있음
    // adjusted_size = DSIZE * ((size +         DSIZE + (DSIZE -1 ) ) / DSIZE); // 짝수word (DSIZE) align 하도록 , 짝수 되게 올림 처리
    adjusted_size = DSIZE * ( (size - WSIZE + DSIZE + (DSIZE -1 )) / DSIZE); // header만 있다면, word로도 충분,  // 괄호 잘못 넣어서 
    // 4까지는 8이어도 됨,
    // 12까지는 16이어도 됨, .... 

    if((bp = find_fit(adjusted_size)) != NULL){
        place(bp, adjusted_size);
        return bp;
    } else {
        size_t size_to_extend = MAX(adjusted_size, CHUNKSIZE); // 왜 적어도 chunksize여야 할까?  = malloc 모사이니, 불필요하게 syscall 안하려고
        if( (bp = extend_heap(size_to_extend/WSIZE)) == NULL) // bp는 이미 coalesced chunk의 payload 위치
            return NULL;

        place(bp, adjusted_size);
        return bp;
    }

    // int newsize = ALIGN(size + SIZE_T_SIZE);
    // void *p = mem_sbrk(newsize); // 어차피 한번 함수 호출하니 스택 depth 있는데 왜 매크로 사용
    // 코드가 바로 syscall해서 좋지 않은, extendHeap이 이미 있다.
    // if (p == (void *)-1)
    //     return NULL;
    // else
    // {
    //     *(size_t *)p = size;
    //     return (void *)((char *)p + SIZE_T_SIZE);
    // }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{

    size_t size = GET_SIZE(HDRP(bp));
    size_t footer_availability = GET_PREV_BLOCK_FREE(HDRP(bp));

    // PUT(HDRP(NEXT_BLKP(bp)), GET(HDRP(NEXT_BLKP(bp))) | BIT_PREV_FREE) ;
    char * nxt_block_header = HDRP(NEXT_BLKP(bp));
    (*((int*)nxt_block_header)) |= BIT_PREV_FREE;

    PUT(HDRP(bp), PACK(size, 0 | footer_availability));
    PUT(FTRP(bp), PACK(size, 0 | footer_availability));

    coalesce(bp);  

}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    copySize = GET_SIZE(HDRP(ptr));
    if (size < copySize)
        copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}
