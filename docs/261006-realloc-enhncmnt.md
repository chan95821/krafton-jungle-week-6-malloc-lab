


Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.003668  1552
 1       yes   99%    5848  0.003771  1551
 2       yes   99%    6648  0.005331  1247
 3       yes  100%    5380  0.004038  1332
 4       yes   66%   14400  0.000065222222
 5       yes   96%    4800  0.006314   760
 6       yes   95%    4800  0.006371   753
 7       yes   55%   12000  0.060123   200
 8       yes   51%   24000  0.113038   212
 9       yes   28%   14401  0.000189 76236
10       yes   33%   14401  0.000079181831
Total          75%  112372  0.202986   554

Perf index = 45 (util) + 37 (thru) = 82/100
jungle@c1ac2d71b44b:/workspaces/malloc-lab$ 

/place.c:180 의 frag 제거 기준을 2*DSIZE 에서 32* DSIZE로 늘림 -> if(size_left >= (1<< 5)*DSIZE
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.003762  1514
 1       yes   99%    5848  0.003787  1544
 2       yes   99%    6648  0.004997  1331
 3       yes  100%    5380  0.003931  1369
 4       yes   66%   14400  0.000064224299
 5       yes   96%    4800  0.005825   824
 6       yes   95%    4800  0.005548   865
 7       yes   55%   12000  0.056120   214
 8       yes   50%   24000  0.111031   216
 9       yes   35%   14401  0.000159 90858
10       yes   34%   14401  0.000076189736
Total          75%  112372  0.195299   575

Perf index = 45 (util) + 38 (thru) = 84/100

bestfit에서 first fit으로 바꿨을 때

Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.003550  1604
 1       yes   99%    5848  0.003437  1701
 2       yes   99%    6648  0.005291  1257
 3       yes   99%    5380  0.003972  1354
 4       yes   66%   14400  0.000087164571
 5       yes   92%    4800  0.003201  1500
 6       yes   92%    4800  0.003048  1575
 7       yes   55%   12000  0.059383   202
 8       yes   52%   24000  0.116824   205
 9       yes   49%   14401  0.000106135987
10       yes   86%   14401  0.000062233026
Total          81%  112372  0.198961   565

Perf index = 49 (util) + 38 (thru) = 86/100


realloc시 앞으로 옮기는 것도 구현했을 때
Using default tracefiles in ./traces/
Measuring performance with gettimeofday().

Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.003537  1610
 1       yes   99%    5848  0.003476  1682
 2       yes   99%    6648  0.005210  1276
 3       yes   99%    5380  0.004032  1334
 4       yes   66%   14400  0.000089162528
 5       yes   92%    4800  0.003272  1467
 6       yes   92%    4800  0.003049  1574
 7       yes   55%   12000  0.058597   205
 8       yes   52%   24000  0.116297   206
 9       yes   44%   14401  0.009723  1481
10       yes   45%   14401  0.001240 11614
Total          77%  112372  0.208522   539

Perf index = 46 (util) + 36 (thru) = 82/100

오히려 줄어듦 - 그러니 블록 크게 만드는게 최적해가 아님 


앞으로 옮기는 것을 줄였을 때 / chunk size 는 2 ^ 12 => 그런데 2 ^ 11로 줄이니 4는 좋아졌지만, realloc 케이스가 30% 하락해서 점수 줄어듦 :::`` gpt 가설`` realloc에서 확장 실패시 새 malloc 하기 때문에 chunk 더 많이 요청 가능 , 일단 chunk는 2^12가 local 최적인듯

jungle@c1ac2d71b44b:/workspaces/malloc-lab$ ./mdriver -v
Team Name:jungle-5
Member 1 :Chan Park:paekchan37@cs.cmu.edu
Using default tracefiles in ./traces/
Measuring performance with gettimeofday().

Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.003482  1635
 1       yes   99%    5848  0.003471  1685
 2       yes   99%    6648  0.005021  1324
 3       yes   99%    5380  0.003870  1390
 4       yes   66%   14400  0.000088163451
 5       yes   92%    4800  0.003035  1582
 6       yes   92%    4800  0.002800  1714
 7       yes   55%   12000  0.062769   191
 8       yes   51%   24000  0.101559   236
 9       yes   80%   14401  0.000088163648
10       yes   86%   14401  0.000062231900
Total          84%  112372  0.186246   603

Perf index = 50 (util) + 40 (thru) = 90/100