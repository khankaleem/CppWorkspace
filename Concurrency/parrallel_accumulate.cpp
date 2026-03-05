// Aligned val to prevent false sharing
template<typename T>
struct alignas(64) AlignedValue {
    T m_val;
};

template<typename T, typename Iterator, int MINBLOCKSIZE = 16>
T parrallel_accumulate(Iterator st, Iterator en, T init) {
    int n = std::distance(st, en);
    if (n <= 0) {
        return init;
    }
    
    // calculate threads and block size
    const int numThreads = std::min<int>(std::max<int>(2, std::thread::hardware_concurrency()), 
                                         ((n + MINBLOCKSIZE - 1) / MINBLOCKSIZE));
    const int blksz = (n + numThreads - 1) / numThreads;

    std::vector<AlignedValue<T>> result(numThreads);
    std::vector<std::thread> workers;
    workers.reserve(numThreads);

    // define accumulator
    auto accumulator = [](T& loc, Iterator i, Iterator j) noexcept {
        loc = std::accumulate(i, j, T{});
    };

    // create workers
    Iterator blk_st = st;
    for (int threadnum = 0; threadnum < numThreads-1; threadnum++) {
        Iterator blk_en = blk_st;
        std::advance(blk_en, blksz);
        workers.push_back(std::thread{accumulator, std::ref(result[threadnum].m_val), blk_st, blk_en});
        blk_st = blk_en;
    }
    workers.push_back(std::thread{accumulator, std::ref(result[numThreads-1].m_val), blk_st, en});
    
    // wait for workers to finish
    for (auto& worker: workers) {
        worker.join();
    }

    // total sum
    T ret{init};
    for (auto& v: result) {
        ret += v.m_val;
    }
    return ret;
}
