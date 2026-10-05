Testing mm malloc
Reading tracefile: short1-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.

Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   12%      12  0.000002  7500
Total          12%      12  0.000002  7500

Perf index = 7 (util) + 40 (thru) = 47/100


=> adjusted size 계산시 괄호 문제,, 사소한

=> 이전과 같이 80점. (short 1 bal)만 해서 

=> Testing mm malloc
Reading tracefile: amptjp-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: cccp-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: cp-decl-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: expr-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: coalescing-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: random-bal.rep
Segmentation fault         (core dumped) ./mdriver -V
- 로직 문제 해결 필요 
-> mm.c:178 에서 모두 채웠을 때 다음 블록의 free flag 업데이트 안해서

jungle@c1ac2d71b44b:/workspaces/malloc-lab$ ./mdriver -v
Team Name:jungle-5
Member 1 :Chan Park:parkchan37@cs.cmu.edu
Using default tracefiles in ./traces/
Measuring performance with gettimeofday().

Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.003619  1573
 1       yes   99%    5848  0.003475  1683
 2       yes   99%    6648  0.004962  1340
 3       yes  100%    5380  0.003521  1528
 4       yes   66%   14400  0.000068211454
 5       yes   92%    4800  0.003126  1535
 6       yes   92%    4800  0.002877  1668
 7       yes   55%   12000  0.055422   217
 8       yes   51%   24000  0.110131   218
 9       yes   27%   14401  0.023782   606
10       yes   34%   14401  0.001147 12560
Total          74%  112372  0.212132   530

Perf index = 44 (util) + 35 (thru) = 80/100