Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   97%    5694  0.003881  1467
 1       yes   97%    5848  0.003842  1522
 2       yes   98%    6648  0.005667  1173
 3       yes   98%    5380  0.004228  1273
 4       yes   66%   14400  0.000088163823
 5       yes   92%    4800  0.003130  1533
 6       yes   91%    4800  0.003018  1591
 7       yes   88%   12000  0.041875   287
 8       yes   53%   24000  0.087374   275
 9       yes   80%   14401  0.000090160189
10       yes   46%   14401  0.000063230416
Total          82%  112372  0.153255   73

Perf index = 49 (util) + 40 (thru) = 89/100

128 단위로만 할당 - binary 1만 의도
생각보다 나머지가 떨어지지 않음
0-4가 소폭 하락, binary 7은 상승,, 그래서 평균 더 줄어듦
-> 128 밑은 지수 단위로 해보면 더 촘촘해질듯

``` c
static inline size_t ADJUST(size_t size) {
    if(size < 128) {
        if (size > 64) return 128;
        if (size > 32) return 64;
        if (size > 16) return 32;
        if (size > 8) return 16;
        else return 8; // 어차피 8 바이트 이상이어야, 의미없는 
    }
    return (128 *((size) / 128 + ((size) % 128 ? 1 : 0)));
}
```
-> Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   97%    5694  0.003540  1608
 1       yes   98%    5848  0.003551  1647
 2       yes   98%    6648  0.005329  1248
 3       yes   98%    5380  0.004001  1345
 4       yes   66%   14400  0.000088163080
 5       yes   92%    4800  0.003009  1595
 6       yes   91%    4800  0.002923  1642
 7       yes   97%   12000  0.041390   290
 8       yes   90%   24000  0.067277   357
 9       yes   80%   14401  0.000090160011
10       yes   53%   14401  0.000059244915
Total          87%  112372  0.131257   856

Perf index = 52 (util) + 40 (thru) = 92/100
큰 요청의 올림 규칙은 유지하고, 작은 요청에 붙는 여유를 줄였다. binary는 util이 88%→97%, binary2는 53%→90%로 상승했다. 특히 binary2에서는 16B 요청의 실제 블록 크기가 136B→24B로 줄면서, 큰 블록을 재사용하는 이득을 유지하고 작은 블록의 낭비를 줄일 수 있었다.
다른 trace의 util은 대부분 그대로였고, trace 1은 97%→98%, realloc2는 46%→53%로 상승했다. 전체 util은 82%→87%, 점수는 89→92점으로 올랐다.
이번 결과는 “작은 요청의 낭비를 줄이면서 큰 요청의 재사용 효과를 유지하자”는 예상과 일치한다. 다만 realloc2는 기존 8B 정렬의 86%보다 여전히 낮으므로, 남은 손해의 원인은 별도로 분석해야 한다.


0 - 3 요청 개선 위해 tc 분석 

trace	전체 malloc	4072B	72B	160B
0 — amptjp	2,847회	2,057회 · 72.3%	268회 · 9.4%	260회 · 9.1%
1 — cccp	2,924회	2,120회 · 72.5%	266회 · 9.1%	251회 · 8.6%
2 — cp-decl	3,324회	2,515회 · 75.7%	297회 · 8.9%	286회 · 8.6%
3 — expr	2,690회	2,005회 · 74.5%	237회 · 8.8%	230회 · 8.6%

``` c
static inline size_t ADJUST(size_t size) {
    if(size < 128) {
        if (size > 64) return 128;
        if (size > 32) return 64;
        if (size > 16) return 32;
        if (size > 8) return 16;
        else return 8; // 어차피 8 바이트 이상이어야, 의미없는 
    }else if(size < 256){
        return (32 * ((size) / 32 + (size % 32 ? 1 : 0)));
    }
    return (128 *((size) / 128 + ((size) % 128 ? 1 : 0)));
}
```
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   98%    5694  0.003670  1552
 1       yes   99%    5848  0.003434  1703
 2       yes   99%    6648  0.005146  1292
 3       yes   99%    5380  0.004067  1323
 4       yes   66%   14400  0.000089162712
 5       yes   92%    4800  0.003444  1394
 6       yes   91%    4800  0.003200  1500
 7       yes   97%   12000  0.041739   288
 8       yes   90%   24000  0.065259   368
 9       yes   80%   14401  0.000094153039
10       yes   53%   14401  0.000065223271
Total          88%  112372  0.130205   863

Perf index = 53 (util) + 40 (thru) = 93/100

jungle@c1ac2d71b44b:/workspaces/malloc-lab$ ./mdriver -v
Team Name:jungle-5
Member 1 :Chan Park:paekchan37@cs.cmu.edu
Using default tracefiles in ./traces/
Measuring performance with gettimeofday().


힙 확장 가능할 때 제자리 확장하기
``` c
if( ((orig_size+ nxt_blk_size) >= adjusted_size)){
            if(nxt_blk_size){
                PUT(HDRP(ptr), PACK(orig_size + nxt_blk_size, GET_PREV_BLOCK_FREE(HDRP(ptr))));
                PUT(FTRP(ptr), PACK(orig_size + nxt_blk_size, GET_PREV_BLOCK_FREE(HDRP(ptr))));
            }
            place(ptr, adjusted_size);
        } else if ((GET_SIZE(HDRP(NEXT_BLKP(ptr))) == 0) || (nxt_blk_size && GET_SIZE(HDRP(NEXT_BLKP(NEXT_BLKP(ptr)))) == 0 )){ // heap 확장 가능할 때 제자리 확장하기

                size_t size_to_extend = MAX(adjusted_size - (orig_size + nxt_blk_size), CHUNKSIZE);
                extend_heap(size_to_extend/WSIZE);
                PUT(HDRP(ptr), PACK(orig_size + GET_SIZE(HDRP(NEXT_BLKP(ptr))), GET_PREV_BLOCK_FREE(HDRP(ptr))));
                PUT(FTRP(ptr), PACK(orig_size + GET_SIZE(HDRP(NEXT_BLKP(ptr))), GET_PREV_BLOCK_FREE(HDRP(ptr))));
                place(ptr, adjusted_size);
        }
        else ....
```

Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   98%    5694  0.003637  1566
 1       yes   99%    5848  0.003374  1733
 2       yes   99%    6648  0.005053  1316
 3       yes   99%    5380  0.003789  1420
 4       yes   66%   14400  0.000088164196
 5       yes   92%    4800  0.003151  1523
 6       yes   91%    4800  0.003145  1526
 7       yes   97%   12000  0.041459   289
 8       yes   90%   24000  0.065828   365
 9       yes   99%   14401  0.000072200014
10       yes   85%   14401  0.000109131998
Total          92%  112372  0.129704   866

Perf index = 55 (util) + 40 (thru) = 95/100


