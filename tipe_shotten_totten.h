// tipe_shotten_totten.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets.

#ifndef MAIN_SHOTTEN
#define MAIN_SHOTTEN

/*
#define ARMA_DONT_USE_WRAPPER
#define ARMA_USE_BLAS
#define ARMA_USE_LAPACK
//#define ARMA_BLAS_LONG_LONG
//#define ARMA_USE_MKL_ALLOC
//#define ARMA_DONT_USE_LAPACK
*/




#define DLIB_USE_BLAS
#define DLIB_USE_LAPACK
//#define DLIB_DISABLE_LAPACK
#define MKL_ILP64 0




/*
Config pour faire fonctionner DLIB :


//#define ARMA_DONT_USE_WRAPPER
//#define ARMA_USE_BLAS
//#define ARMA_USE_LAPACK
//#define ARMA_BLAS_LONG_LONG
//#define ARMA_USE_MKL_ALLOC
#define ARMA_DONT_USE_LAPACK

#define DLIB_USE_BLAS
//#define DLIB_USE_LAPACK
#define DLIB_DISABLE_LAPACK
#define MKL_ILP64 0





*/

void train_batch(int nb_iterations, bool load, bool playAfter, bool testAgainstRandom, int nb_tests);
void trainAgainstRandom(int nb_iterations, bool load, bool playAfter, bool testAgainstRandom, int nb_tests);
//void test_network();

#endif