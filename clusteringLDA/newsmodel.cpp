#include "newsmodel.h"

#include <string>

using namespace std;

NewsModel::NewsModel() { set_default_values(); }

void NewsModel::set_default_values()
{
    num_words_cluster = 5;
    num_iters = 4000;
    num_cluster = 200;
    save_step = 2000;
    alpha = 50.0 / static_cast<double>(num_cluster);
    beta = 0.1;

    use_punctuation_filter = true;
    use_stopwords_filter = true;
    use_idf_filter = true;
    use_shingles = true;
    use_words_threshold = true;
    use_size_threshold = true;

    num_words_shingle = 2;

    max_idf_threshold = 7.5;
    min_idf_threshold = 2.5;

    words_threshold = 3;
    size_threshold = 10;
}
