#ifndef CLUSTERING_H
#define CLUSTERING_H

#include <string>
#include <vector>
#include <utility>

#include "newscluster.h"
#include "news.h"

/* NOTA: termine e parola sono usati come sinonimi */

// classe che effettua il clustering di un dataset

class Clustering
{
public:
    Clustering(int n_topic, int n_words, const std::string &pathThetafile,
               const std::string &pathTwordsfile, std::vector<News>& dataset);

    Clustering(int n_topic, int n_words, const std::string &pathThetafile,
               const std::string &pathTwordsfile, std::vector<News>& dataset, bool use_shingle);

    // procedura che ritorna i cluster presenti nel dataset
    std::vector<NewsCluster>& get_clusters_vector();

    /* procedura che ritorna i cluster presenti nel dataset, dopo aver filtrato le news che non
       contengno nessuna delle num_words parole più probabili del topic a cui appartengono
       NOTA: se num_words è maggiore di n_words_, vengono considerate solo n_words_ parole */
    std::vector<NewsCluster>& get_clusters_vector_with_filter(index_t num_words);

private:
    // parametri del clustering
    index_t n_topics_;
    index_t n_words_;

    // dateaset del clustring
    std::vector<News> dataset_;
    // indica se il clustering è stato ottenuto considerando shingles di parole
    bool use_shingles_;

    // media e dev.standard delle probabilità del dataset (per documento e per termine)
    prob_t avg_doc_prob_;
    prob_t stddev_doc_prob_;
    prob_t avg_term_prob_;
    prob_t stddev_term_prob_;

    /* vettore di documenti, ogni documento è una coppia (index_t, prob_t)
       dove l'indice è l'indice del topic con la probabilità più alta
       e la probabilità è la probabilità di appartenenza a tale topic */
    std::vector< document_t > documents_;

    /* vettore di documenti per topic, ordinato secondo il topic,
       l'elemento i-esimo del vettore è il vettore di documenti del topic i-esimo
       [ogni documento è una coppia (index_t, prob_t) dove l'indice
       rappresenta il numero del documento mentre la probabilità è la
       probabilità di appartenenza a tale topic] */
    std::vector< documents_topic_t > documents_by_topic_;

    /* vettore di parole per topic, ordinato secondo il topic,
       l'elemento i-esimo del vettore è il vettore di parole per il topic i-esimo
       [ogni parola è una coppia (string, prob_t) dove la stringa è l'effettiva
       parola mentre la probabilità indica la probabilità di appartenenza a tale topic] */
    std::vector< words_topic_t > words_by_topic_;

    // calcola i valori statistici per i documenti
    void set_docs_statistics();
    // calcola i valori statistici per i termini
    void set_terms_statistics();

    // calcola la validità di un cluster
    int set_validity(int index);

    /* riempie il vettore documents_ a partire dal file .theta salvato in filepath:
       ogni riga riguarda un documento e contiene la probabilità di appartenenza ad ognuno dei n_topics_ topic */
    void read_theta(std::string filepath);

    /* riempie il vettori documents_by_topic_ con gli indici corretti, dato che documents_ non contiene
       i documenti che hanno corpo vuoto, presenti invece in dataset */
    void fill_topic_vector(std::vector<News>& dataset);

    /* riempie il vettore words_by_topic_ a partire dal file .twords salvato in filepath:
       la prima riga indica il topic, le successive n_words_ righe indicano le parole più probabili per quel topic
       con la relativa probabilità */
    void read_twords(std::string filepath);

    // ritorna true se news_body possiede almeno una delle parole in words
    bool has_at_least_one_word(std::vector<std::string> &words, std::string &news_body);

    /* aggiorna la frequenza della mappa di parole rispetto al corpo della news news_body
       NOTA: procedura usata per il controllo della bontà dei cluster */
    void update_map_frequency(std::map<std::string, int>& map_shingles, std::string& news_body);

    /* riempie row con le probabilità (prob_t) presenti in str,
       tali probabilità sono separate da uno spazio */
    static void FillVector(std::vector<prob_t>* row, const std::string &str);

    // inserice in coda a row la coppia (parola, probabilità) prelevata da str.
    static void ParseTwordsLine(std::vector<word_t>* row, const std::string &str);

};

#endif // CLUSTERING_H
