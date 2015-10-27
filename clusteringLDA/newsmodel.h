#ifndef NEWSMODEL_H
#define NEWSMODEL_H

#include <string>

class NewsModel
{
public:
    int num_cluster;                // numero di topic/cluster da generare
    int num_words_cluster;          // numero di parole più probabili per topic

    int num_iters;                  // numero di iterazioni da far effettuare alla libreria GibbsLDA++
    int save_step;                  // numero di iterazioni per salvataggi intermedi
    double alpha;                    // parametro alpha della libreria
    double beta;                     // parametro beta della libreria

    bool use_punctuation_filter;    // uso del filtro sulla punteggiatura
    bool use_stopwords_filter;      // uso del filtro sulle stop-word
    bool use_idf_filter;            // uso del filtro sulle parole con intervallo idf
    bool use_shingles;              // uso degli shingle (unione di più parole)
    bool use_words_threshold;       // uso della soglia per
    bool use_size_threshold;        // uso della soglia sui cluter in output

    int num_words_shingle;          // numero di parole che formano uno shingle

    // NOTA: controllare il secondo tipo del dizionario (value_type), deve coincidere!
    double max_idf_threshold;        // massimo valore di idf
    double min_idf_threshold;        // minimo valore di idf

    int words_threshold;              // numero di parole più probabili da considerare per filtrare i documenti di un topic

    int size_threshold;             // dimensione minima di un topic, perchè questo venga aggiunto ai risultati

    NewsModel();

    void set_default_values();

};

#endif // NEWSMODEL_H
