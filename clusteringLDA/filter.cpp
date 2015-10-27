#include "filter.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>

using std::string;
using std::vector;

// stopwords file path
const string Filter::STOPWORDS_FILE_PATH = "./stopwords.txt";

Filter::Filter()
    : filter_punctuation_(false)
    , filter_stopwords_(false)
    , use_shingles_(false)
    , filter_idf_(false)
    , number_words_shingle_(0)
    , max_idf_threshold_(0)
    , min_idf_threshold_(0)
    , min_length_(MIN_WORD_LENGTH)
{}

Filter::Filter(bitMask filter_mask)
    : filter_punctuation_(filter_mask[PUNCTUATION_FILTER_BIT])
    , filter_stopwords_(filter_mask[STOPWORDS_FILTER_BIT])
    , use_shingles_(false)
    , filter_idf_(false)
    , number_words_shingle_(0)
    , max_idf_threshold_(0)
    , min_idf_threshold_(0)
    , min_length_(MIN_WORD_LENGTH)
{
    if(filter_stopwords_)
        load_stop_words();
}

Filter::Filter(bitMask filter_mask, int number_words_shingle)
    : filter_punctuation_(filter_mask[PUNCTUATION_FILTER_BIT])
    , filter_stopwords_(filter_mask[STOPWORDS_FILTER_BIT])
    , use_shingles_(filter_mask[USE_SHINGLES_BIT])
    , filter_idf_(false)
    , number_words_shingle_(number_words_shingle)
    , max_idf_threshold_(0)
    , min_idf_threshold_(0)
    , min_length_(MIN_WORD_LENGTH)
{
// ############ a cosa serve il 2? ############
//    min_length_ = 2;
// ############################################
    if(filter_stopwords_)
        load_stop_words();
}

Filter::Filter(bitMask filter_mask, int number_words_shingle, float max_idf, float min_idf)
    : filter_punctuation_(filter_mask[PUNCTUATION_FILTER_BIT])
    , filter_stopwords_(filter_mask[STOPWORDS_FILTER_BIT])
    , use_shingles_(filter_mask[USE_SHINGLES_BIT])
    , filter_idf_(filter_mask[IDF_FILTER_BIT])
    , number_words_shingle_(number_words_shingle)
    , max_idf_threshold_(max_idf)
    , min_idf_threshold_(min_idf)
    , min_length_(MIN_WORD_LENGTH)
{
    // ############ a cosa serve il 2? ############
    //    min_length_ = 2;
    // ############################################
    if(filter_stopwords_)
        load_stop_words();
}

void Filter::load_stop_words()
{
    string line;            //stringa temporanea per la lettura di una linea del file

    // apertura del file di stop word
    string path (STOPWORDS_FILE_PATH);
    std::ifstream file(path.c_str(), std::ifstream::in);

    if (!file.is_open()) {
        std::cerr << "Error opening the file " + path << std::endl;
        exit(1);
    }

    // lettura delle stop word e creazione della stopWordsMap
    while (file.good())
    {
        getline(file, line);
        // estrazione della stop word (dall'inizio fino al primo separatore)
        string word = line.substr(0, line.find(WORD_SEPARATOR, 0));
        stopWordsMap.insert(std::make_pair<std::string, bool>(word, true));
    }

    file.close();
}

void Filter::apply(std::string &text)
{
    // trasforma il testo in lowercase
    transform(text.begin(), text.end(), text.begin(), ::tolower);

    if(filter_punctuation_)
        punctuation_filter(text);
    if(filter_stopwords_)
        remove_stop_words(text);
    if(use_shingles_)
        make_shingles(text, number_words_shingle_);
}

// rimuove la punteggiatura dalla stringa text
void Filter::punctuation_filter(std::string &text)
{
    // variabili booleane che indicano se il precedente carattere era uno spazio o se è stato ignorato
    bool was_space = false, was_skipped = false;
    // iteratore di scrittura sulla stringa
    string::iterator current = text.begin();

    for (string::iterator it = text.begin(); it != text.end(); ++it)
    {
        // se il carattere è stampabile e non è punteggiatura lo copio
        if(isprint(*it) && !ispunct(*it))
        {
            // se il carattere non è uno spazio ed il precedente è stato ignorato è necessario copiare uno spazio
            if(*it != ' ' && was_skipped)
            {
                *current = ' ';
                ++current;
            }
            // tale controllo evita di accoppiare parole
            *current = *it ;
            ++current;
            was_skipped = false;
        }
        // altrimenti
        else
            // il carattere è stato ignorato solo se il carattere precedente NON era uno spazio
            was_skipped = !was_space;

        was_space = (*it == ' ') ? true : false;;
    }
    // riduco la dimensione della stringa
    text.resize(current - text.begin());
}

// rimuove le stop word dalla stringa text
void Filter::remove_stop_words(std::string &text)
{
    // posizione e lunghezza delle sottostringhe
    size_t pos = 0;
    size_t len = 0;
    // indica la presenza dello spazio
    bool has_space = false;
    // stringa temporanea per le sottostringhe
    string word;
    // iteratori della stringa:
    // it è di lettura
    string::iterator it = text.begin();
    // current è di scrittura
    string::iterator current = it;
    // before_word indica la posizione di current prima della parola da analizzare
    string::iterator before_word = current;

    while(it != text.end())
    {
        has_space = false;

        // leggo i carattari che sono separatori di parole
        while (it != text.end() && *it == WORD_SEPARATOR)
        {
            // scrivo solo il primo spazio
            if(!has_space) {
                *current = *it;
                ++current;
                has_space = true;
            }
            ++it;
        }

        // salvo la posizione prima della prossima parola, se cambiata
        // NOTA: questo controllo evita problemi quando tutte le parole vengano rimosse
        before_word = (current != before_word) ? (current - 1) : before_word;
        pos = it - text.begin();

        // leggo e scrivo la prossima parola
        while (it != text.end() && *it != WORD_SEPARATOR)
        {
            *current = *it;
            ++it;
            ++current;
        }

        len = (it - text.begin()) - pos;
        // parola da analizzare
        word = text.substr(pos, len);

        // se la parola è un numero o la sua lunghezze è inferiore a min_length_ oppure è una stop word
        // riposiziono l'iteratore di scrittura prima dell'inizio di tale parola
        if (IsNumber(word) || word.length() < min_length_ || (stopWordsMap.find(word) != stopWordsMap.end())) {
            current = before_word;
        }
    }
    // riduco la dimensione della stringa
    text.resize(current - text.begin());
}

// ritorna una stringa che contiene tutti gli shingle di parole della stringa text
void Filter::make_shingles(std::string &text, int num_words_shingle)
{
    // posizione e lunghezza delle sottostringhe
    size_t pos = 0;
    size_t len = 0;
    // nuova stringa di shingle
    string new_text;
    // iteratore di lettura della stringa
    string::iterator it = text.begin();
    // array di parole da usare per generare uno shingle
    // nota: è usato in maniera circolare
    string words[num_words_shingle];
    unsigned i;
    // posizione nell'array della parola corrente
    unsigned index = 0;
    // posizione nell'array della prima parola dello shingle
    unsigned first_word = 0;

    while(it != text.end())
    {
        // leggo i carattari che sono separatori di parole
        while (it != text.end() && *it == WORD_SEPARATOR) ++it;

        pos = it - text.begin();

        // leggo la prossima parola
        while (it != text.end() && *it != WORD_SEPARATOR) ++it;

        len = (it - text.begin()) - pos;

        // copio la parola nel vettore di parole per lo shingle nella posizione corrente
        words[index] = text.substr(pos, len);

        // se la prossima posizione è quella della prima parola devo scrivere lo shingle
        if(((index + 1) % num_words_shingle) == first_word)
        {
            i = first_word;

            // appendo le parole dello shingle (a parte l'ultima)
            while(i != index)
            {
                new_text.append(words[i] + "_");
                i = (i + 1) % num_words_shingle;
            }

            // appendo l'ultima parola dello shingle
            if(it != text.end())
                new_text.append(words[i] + " ");
            else
                // se il puntatore di lettura è arrivato alla fine non appendo lo spazio
                new_text.append(words[i]);

            first_word = (first_word + 1) % num_words_shingle;
        }
        index = (index + 1) % num_words_shingle;
    }

    // scambio il vecchio testo con il nuovo
    text.swap(new_text);
}

// applica il filtro idf sul dataset
void Filter::apply_idf(std::vector<News>& dataset, const Dictionary &dictionary)
{
    if(filter_idf_)
    {
        for (vector<News>::iterator news = dataset.begin(); news != dataset.end(); news++)
        {
            if ( !(news->is_empty()) )
                news->set_body(idf_filter(news->body(), dictionary));
        }
    }
}

// applica il filtro idf su text
std::string Filter::idf_filter(std::string text, const Dictionary &dictionary)
{
    // posizione e lunghezza delle sottostringhe
    size_t pos = 0;
    size_t len = 0;
    // stringa temporanea per le sottostringhe
    string word;
    // nuova stringa di shingle
    string new_text;
    // iteratore di lettura della stringa
    string::iterator it = text.begin();
    // iteratore per il risultato della ricerca in dictionary
    double idf;

    while(it != text.end())
    {
        // leggo i carattari che sono separatori di parole
        while (it != text.end() && *it == WORD_SEPARATOR) ++it;

        pos = it - text.begin();

        // leggo la prossima parola
        while (it != text.end() && *it != WORD_SEPARATOR) ++it;

        len = (it - text.begin()) - pos;
        // parola da analizzare
        word = text.substr(pos, len);

        // ottengo il valore idf della parola
        // NOTA: non controllo se non è presente, se non lo è dovrebbe ritornare un valore comunque piccolo provocare l'eliminazione
        idf = dictionary.find(word)->second;

        if(word != "")
        {
            // la parola viene aggiunta solo se il relativo valore idf è compreso nei limiti
            if (min_idf_threshold_ <= idf && idf <= max_idf_threshold_)
            {
                if(it != text.end())
                    new_text.append(word + " ");
                else
                    // se il puntatore di lettura è arrivato alla fine non appendo lo spazio
                    new_text.append(word);
            }
        }
    }

    return new_text;
}


//membri statici

// Controlla se s è un numero
bool Filter::IsNumber(const std::string &str)
{
    std::string::const_iterator it = str.begin();
    while (it != str.end() && std::isdigit(*it)) ++it;
    return !str.empty() && it == str.end();
}
