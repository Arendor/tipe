

#ifndef COMPILE_TIME_LOOP

#define COMPILE_TIME_LOOP


template <int I, int N>
struct ForLoop_static {
    template <typename Lambda>
    void operator()(const Lambda& f) {
        f(std::integral_constant<int, I>{});
        if constexpr (I + 1 < N) {
            ForLoop_static<I + 1, N>(f);
        }
    }
};


template<int I, int N, class F>
constexpr void static_for(F f) {
    if constexpr (I < N) {
        f.template operator() < I > ();
        static_for<I + 1, N>(f);
    }
}


#endif