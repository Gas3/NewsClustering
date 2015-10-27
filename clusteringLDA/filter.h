#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <vector>
#include <utility>
#include <tr1/unordered_map>
#include <bitset>
#include <stdlib.h>

#include "dictionary.h"

// numero massimo di filtri che si possono abilitare
const int NFILTER = 4;

// bit mask per indicare quali filtri utilizzare
typedef std::bitset<NFILTER> bitMask;

// enumerazione dei bit della bit mask
enum {
    PUNCTUATION_FILTER_BIT,
    STOPWORDS_FILTER_BIT,
    USE_SHINGLES_BIT,
    IDF_FILTER_BIT
};

// classe che rappresenta un filtro, ovvero un insieme di filtri da applicare al corpo di una news

class Filter
{
public:
    //costruttori
    Filter();

    Filter(bitMask filter_mask);

    Filter(bitMask filter_mask, int number_words_shingle);

    Filter(bitMask filter_mask, int number_words_shingle, float max_idf, float min_idf);

    // applica i filtri di un filtro ad una stringa
    void apply(std::string &text);

    // applica il filtro idf sul dataset, in base al dizionario costruito su tale dataset
    void apply_idf(std::vector<News> &dataset, const Dictionary &dictionary);

private:
    // indica quali filtri applicare
    bool filter_punctuation_;
    bool filter_stopwords_;
    bool use_shingles_;
    bool filter_idf_;

    // numero di parole per shingle
    int number_words_shingle_;
    // limiti dei valori dell'idf di una parola
    float max_idf_threshold_;
    float min_idf_threshold_;
    // lunghezza minima di una parola
    unsigned int min_length_;

    std::tr1::unordered_map<std::string, bool> stopWordsMap;    // stop words map

    static const int MIN_WORD_LENGTH = 3;           // lunghezza minima di una parola
    static const std::string STOPWORDS_FILE_PATH;   // stopwords file path
    static const char WORD_SEPARATOR = ' ';         // carattere che separa una parola da un'altra

    // restituisce una stringa ottenuta da text eliminando tutte le parole con idf al di fuori dell'intervallo
    // min_idf_threshold_ - max_idf_threshold_: una parola viene eliminata se ha idf < min_idf_threshold_ o idf > max_idf_threshold_
    std::string idf_filter(std::string text, const Dictionary &dictionary);

    // rimuove la punteggiatura ed i caratteri non stampabili dalla stringa text
    void punctuation_filter(std::string &text);

    // carica il file di stop word
    void load_stop_words();

    // rimuove le stopword presenti nella mappa stopWordsMap dalla stringa text
    void remove_stop_words(std::string &text);

    // trasforma la stringa text
    void make_shingles(std::string &text, int num_words_shingle);

    // restituisce true se str è un numero
    static bool IsNumber(const std::string &str);
};

#endif // FILTER_H
