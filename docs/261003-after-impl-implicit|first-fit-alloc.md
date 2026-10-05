commit hash:
35240c6cd35446d286186c9a5858e1133525fd56

Team Name:jungle-5
Member 1 :Chan Park:parkchan37@cs.cmu.edu
Measuring performance with gettimeofday().

Testing mm malloc
Reading tracefile: short1-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.

Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   66%      12  0.000000 24000
Total          66%      12  0.000000 24000

Perf index = 40 (util) + 40 (thru) = 80/100
[1] + Done                       "/usr/bin/gdb" --interpreter=mi --tty=${DbgTerm} 0<"/tmp/Microsoft-MIEngine-In-zhbqbxpj.pgw" 1>"/tmp/Microsoft-MIEngine-Out-ozr20r0y.gu1"


- allocator의 요구사항
	- Handling arbitrary request sequences.
	- Making immediate responses to requests.
	- Using only the heap.
	- Aligning blocks (alignment requirement).
	- Not modifying allocated blocks. => 그래서 compaction/ 이동 안됨,,
- 목표
	- Goal 1: Maximizing throughput. -시간
	- Goal 2: Maximizing memory utilization. -공간
	- -> Throughput과 memory utilization을 모두 만족하기 어렵다

## 생각해봐야 할 것


1. space utilization 
    - 할당 chunk의 메타데이터 오버헤드
        footer는 header만으로 이전 블록 위치 찾을 수 없어서 만들어 놓은 것/ 이때 footer는 free block coalesce 위해 만든 것이므로, 이전 블록이 free일 때만 쓸모있음 유의

    - 외부 단편화
        - fit / , 어떤 크기가 어떤 순서로 들어올지 알 수 없다 -> 온라인 알고리즘, 좋은 방법론 없는지?
    - 내부 단편화 
        align인데, 일단 대책 없어 보임

2. time 
    - request에 대해 free block 찾는 시간이 빨라야 함
        -> 순차 탐색이니 마지막에 있으면 느림
            - priority_queue? / e.g. .../ 정렬 후 이진 검색?  겹쳐놓는것 구현해볼지

            
        -> 그러면서 utilization 고려 필요

implicit/ explicit은 어느 문제 해결용?

    explicit이 탐색 시간을 줄여줄 수 있으니, 절약한 만큼 좋은 fit 찾는데 시간 더 쓸 수 있다