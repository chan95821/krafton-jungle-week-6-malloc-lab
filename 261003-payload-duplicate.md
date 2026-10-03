Reading tracefile: short1-bal.rep
Checking mm_malloc for correctness, ERROR [trace 0, line 9]: Payload (0xf6926820:0xf6927807) overlaps another payload (0xf6926820:0xf692684f)


payload 시작점이 동일하게 겹침 - 

1. alloc bit를 잘못 설정했을 수 있다
    place에서 chunk 만들고, 남은 블록 free 처리함

2. place()에서 나눈 free와 allocated block 처리에 문제 


3. find_fit()에서 while문이 바로 전 bp에서 멈췄을 수 있다



-> GET_ALLOC 인자 잘못 넣은 - HDRP로 감싸줘야 하는 오타 수준