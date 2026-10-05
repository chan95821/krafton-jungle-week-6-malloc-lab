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