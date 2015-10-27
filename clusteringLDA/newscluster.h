// Classe che rappresenta un cluster di news.

#ifndef NEWSCLUSTER_H
#define NEWSCLUSTER_H

#include <string>
#include <vector>

// tipo per l'indice di un topic/documento
typedef unsigned int index_t;

// tipo per la probabilità di appartenenza di un documento/parola ad un topic
typedef double prob_t;

// tipo di un documento: coppia (index_t, prob_t)
typedef std::pair< index_t, prob_t > document_t;

// tipo di un topic di documenti: vettore di documenti
typedef std::vector< document_t > documents_topic_t;

// tipo di una parola
typedef std::pair< std::string, prob_t> word_t;

// tipo di un topic di parole: vettore di word_t
typedef std::vector< word_t > words_topic_t;

// classe che rappresenta un cluster di news

class NewsCluster
{
public:
    NewsCluster(words_topic_t terms, documents_topic_t idnews, int validity);

    NewsCluster(words_topic_t terms, documents_topic_t idnews, int validity, bool use_shingles);

    // ritorna una stringa formattata nel seguente modo:
    // [intero non significativo]
    // [\t]
    // [sequenza termini importanti del cluster, divisi da spazi]
    // Possono anche essere inserite più sequenze nel caso i termini siano divisi in insiemi,
    // in tal caso dividere le sequenze con una virgola “,”.
    // [\t]
    // [sequenza ID news ordinate per importanza nel cluster]
    // [\t]
    // [flag binario: 1 cluster valido, 0 cluster non valido]
    std::string str();

    // procude lo stesso risultato di str() sullo stream os
    friend std::ostream& operator<<(std::ostream& os, const NewsCluster& nc);

    // restituisce la dimensione del cluster
    int size();

    // restituisce la validità del cluster
    int get_validity();


private:
    // primo campo, intero non significativo
    int first_field_;

    // secondo campo, termini importanti per il cluster
    words_topic_t  terms_;

    // terzo campo, id dei documenti del cluster
    documents_topic_t  idnews_;

    // quarto campo, validità del cluster
    int validity_;

    // indica se il cluster è stato ottenuto considerando shingles di parole
    bool use_shingles_;

    /* scrive sullo ostream stream le sottoparole della parola word, ottenute suddividendola dove
       occorre il carattere sep, separate da uno spazio */
    void split_shingles(std::ostream &stream, std::string word, char sep) const;

    /* scrive sullo stream passato come parametro tutti i termini presenti in terms_, separati da uno spazio
       prima di ogni termine viene riportata la sua probabilità, separata da uno spazio dal termine
       NOTA: se i termini sono shingle, vengono divisi nelle parole che li costituiscono */
    void print_terms(std::ostream &stream) const;

    // restituisce la stringa ottenuta da print_terms(std::ostream &stream)
    std::string print_terms() const;

    /* scrive sullo stream passato come parametro la dimensione del dizionario seguito da
       la lista di tutte le parole del dizionario ed il rispettivo idf:
       ogni riga contiene la parola e l'idf separati da uno spazio */

    // scrive sullo stream passato come parametro tutti gli id presenti in idnews_, separati da uno spazio
    void print_ids(std::ostream &stream) const;

    // restituisce la stringa ottenuta da print_ids(std::ostream &stream)
    std::string print_ids() const;
};

#endif // NEWSCLUSTER_H
