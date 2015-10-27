#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <math.h>
#include <stdlib.h>

#include "clustering.h"
#include "conversion.h"

using std::string;
using std::vector;
using std::pair;
using std::ifstream;
using std::map;

/* ######################################################################## */
// funzioni di confronto per l'odinamento

// ordinamento per dimensione dei cluster (decrescente)
bool compareClusters(NewsCluster a, NewsCluster b)
{
    return (a.get_validity() >= b.get_validity() && a.size() > b.size());
}

// ordinamento dei documenti rispetto alla probabilità (decrescente)
bool compareDocs(document_t a, document_t b)
{
    return (a.second > b.second);
}

// ordinamento delle parole rispetto alla probabilità (decrescente)
bool compareWords(word_t a, word_t b)
{
    return (a.second > b.second);
}
/* ######################################################################## */

Clustering::Clustering(int n_topic, int n_words, const std::string &pathThetafile,
                       const std::string &pathTwordsfile, std::vector<News>& dataset)
    : n_topics_(n_topic)
    , n_words_(n_words)
    , dataset_(dataset)
    , use_shingles_(false)
    , documents_by_topic_(n_topics_)
{
    // carico i documenti (document_t) dal ralativo file
    read_theta(pathThetafile);
    // riempio il vettore di topic, ordinato rispetto ai topic
    fill_topic_vector(dataset_);
    // carico le parole (word_t) dal relativo file
    read_twords(pathTwordsfile);

    // calcolo i valori statistici
    set_docs_statistics();
    //set_terms_statistics();

    //ordino i documenti/le parole di ogni topic per probabilità decrescente
    for(vector< documents_topic_t >::iterator vi = documents_by_topic_.begin(); vi != documents_by_topic_.end(); vi++)
        sort(vi->begin(), vi->end(), compareDocs);
    for(vector< words_topic_t >::iterator vi = words_by_topic_.begin(); vi != words_by_topic_.end(); vi++)
        sort(vi->begin(), vi->end(), compareWords);
}

Clustering::Clustering(int n_topic, int n_words, const std::string &pathThetafile,
                       const std::string &pathTwordsfile, std::vector<News>& dataset, bool use_shingle)
    : n_topics_(n_topic)
    , n_words_(n_words)
    , dataset_(dataset)
    , use_shingles_(use_shingle)
    , documents_by_topic_(n_topics_)
{
    // carico i documenti (document_t) dal ralativo file
    read_theta(pathThetafile);
    // riempio il vettore di topic, ordinato rispetto ai topic
    fill_topic_vector(dataset);
    // carico le parole (word_t) dal relativo file
    read_twords(pathTwordsfile);

    // calcolo i valori statistici
    set_docs_statistics();
    //set_terms_statistics();

    //ordino i documenti/le parole di ogni topic per probabilità decrescente
    for(vector< documents_topic_t >::iterator vi = documents_by_topic_.begin(); vi != documents_by_topic_.end(); vi++)
        sort(vi->begin(), vi->end(), compareDocs);
    for(vector< words_topic_t >::iterator vi = words_by_topic_.begin(); vi != words_by_topic_.end(); vi++)
        sort(vi->begin(), vi->end(), compareWords);
}

// riempie il vettore documents_
void Clustering::read_theta(std::string filepath)
{
    string line;
    vector<prob_t> *row;                        // puntatore a vettore di probabilità
    ifstream theta_file (filepath.c_str());     // file di lettura .theta

    /* leggo il file .theta e per ogni riga (che rappresenta un documento) determina
       qual'è il topic con la probailità più alta per quel documento */
    if (theta_file.is_open())
    {
        // salvo la posizione iniziale
        std::streampos begin = theta_file.tellg();

        // controllo che il file abbia lo stesso numero di topic di quelli richiesti
        getline (theta_file, line);
        if (line.size() != 0) {
            row = new vector<prob_t>();
            // inserisce in row tutte le probabilità del documento nella stringa line
            FillVector(row, line);
            // controllo sul numero di topic
            if (row->size() != n_topics_) {
                std::cerr << "Error: The model in " << filepath << " has "<< row->size() <<" clusters instead of " << n_topics_ << std::endl;
                exit(1);
            }
            row->~vector();
        }

        // riposiziono la lettura nella posizione iniziale
        theta_file.seekg(begin);

        while (theta_file.good())
        {
            getline (theta_file, line);
            if (line.size() != 0) {
                row = new vector<prob_t>();
                // inserisce in row tutte le probabilità del documento nella stringa line
                FillVector(row, line);
                // calcolo del topic con probabilità massima
                int index = (std::max_element(row->begin(), row->end()) - row->begin());

                // inserisco in documents_ la coppia (indice del topic, probabilità di appartenenza al topic)
                documents_.push_back(std::make_pair(index, (*row)[index]));

                row->~vector();
            }
        }
        theta_file.close();
    }
    else {
        std::cerr << "Error: File " << filepath << " not found" << std::endl;
        exit(1);
    }
}

// riempie il vettore documents_by_topic_ con gli indici corretti
void Clustering::fill_topic_vector(std::vector<News>& dataset)
{
    // ##### NOTA: documents_ non contiene i documenti vuoti #####
    index_t news_index = 0;
    for(vector< document_t >::iterator vi = documents_.begin(); vi != documents_.end(); vi++)
    {
        // skippo le news che hanno corpo vuoto (dopo l'applicazione dei filtri)
        while(news_index < dataset.size() && dataset[news_index].is_empty()) news_index++;

        documents_by_topic_[vi->first].push_back(std::make_pair (news_index, vi->second));
        news_index++;
    }
}

// riempie il vettore words_by_topic_
void Clustering::read_twords(std::string filepath)
{
    string line;
    index_t j;                                  // indice della j-esima parola del topic corrente
    vector<word_t> * row;                       // vettore di coppie (parola, probabilità di quella parola)
    ifstream twords_file (filepath.c_str());    // file di lettura .twords

    /* leggo il file .twords che per ogni topic contiene le n_words_ più probabili per quel topic
       con la relativa probabilità di appartenenza */
    if (twords_file.is_open())
    {
        // salvo la posizione iniziale
        std::streampos begin = twords_file.tellg();

        // numero di parole più probabili contate
        index_t counted = 0;
        // indica se ho letto le parole del primo topic
        bool counting = false;

        // leggo la prima linea e controllo che contenga per "Topic"
        if (twords_file.good()) {
            getline (twords_file, line);
            if (line.find("Topic") != std::string::npos)
                counting = true;
        }

        // controllo che il file abbia almeno il numero di parole più probabili richieste
        while (twords_file.good() && counting)
        {
            getline (twords_file, line);
            // se trovo il Topic successivo, ho letto le parole del primo Topics
            if (line.find("Topic") != std::string::npos)
                counting = false;
            // altrimenti ho trovato un altra parola
            else
                counted++;
        }
        // controllo sul numero di topic
        if (counted < n_words_) {
            std::cerr << "Error: The model in " << filepath << " has only "<< counted <<" words instead of " << n_words_ << std::endl;
            exit(1);
        }
        else if (counted > n_words_) {
            std::cout << "Warning: The model in " << filepath << " has "<< counted <<" words instead of " << n_words_ << std::endl;
        }

        // riposiziono la lettura nella posizione iniziale
        twords_file.seekg(begin);

        while (twords_file.good())
        {   
            getline (twords_file, line);        // lettura della riga "Topic i-th:"
            if (line.size() != 0)
            {
                j = 0;
                row = new std::vector<word_t>;
                /* leggo tutte le parole e le rispettive probablità del topic corrente:
                   ogni riga ha la sintassi "\tparola   probabilità\n"
                   NOTA: la parola e la sua probabilità sono SEPARATI DA 3 SPAZI */
                while (twords_file.good() && j < n_words_)
                {
                    getline (twords_file, line);
                    if (line.size() != 0) {
                        // inserisce in coda a row la coppia (parola, probabilità)
                        ParseTwordsLine(row, line);
                        j++;
                    }
                }
                // skippo le eventuali parole in più presenti nel file
                while (twords_file.good() && j < counted) {
                    getline (twords_file, line);
                    j++;
                }

                // inserisco in words_by_topic_ il vettore di coppie (parola, probabilità) dell'i-esimo topic.
                words_by_topic_.push_back(*row);

                row->~vector();
            }
        }
        twords_file.close();
    }
    else {
        std::cerr << "Error: File " << filepath << " not found" << std::endl;
        exit(1);
    }
}


// calcolo dei valori statistici su documenti e termini

// calcola i valori statistici per i documenti
void Clustering::set_docs_statistics()
{
    prob_t avg = 0.0, std_dev = 0.0;

    //itero sui topic
    for(vector< documents_topic_t >::iterator topic_it = documents_by_topic_.begin(); topic_it != documents_by_topic_.end(); topic_it++)
    {
        //itero sui documenti di un topic
        for(documents_topic_t::iterator doc_it = topic_it->begin(); doc_it != topic_it->end(); doc_it++)
            avg += doc_it->second;
    }

    avg /= documents_.size();
    avg_doc_prob_ = avg;

    //itero sui topic
    for(vector< documents_topic_t >::iterator topic_it = documents_by_topic_.begin(); topic_it != documents_by_topic_.end(); topic_it++)
    {
        //itero sui documenti di un topic
        for(documents_topic_t::iterator doc_it = topic_it->begin(); doc_it != topic_it->end(); doc_it++)
            std_dev += pow(doc_it->second - avg_doc_prob_, 2);
    }

    std_dev /= documents_.size();
    stddev_doc_prob_ = sqrt(std_dev);
//    cout << "media: " << avg_doc_prob << " std dev : " << stddev_doc_prob << endl;
}

// calcola i valori statistici per i termini
void Clustering::set_terms_statistics()
{
    prob_t avg = 0.0, std_dev = 0.0;

    //itero sui topic
    for(vector< words_topic_t >::iterator topic_it = words_by_topic_.begin(); topic_it != words_by_topic_.end(); topic_it++)
    {
        //itero sulle parole di un topic
        for(words_topic_t::iterator word_it = topic_it->begin(); word_it != topic_it->end(); word_it++)
            avg += word_it->second;
    }

    avg /= words_by_topic_.size()*n_words_;
    avg_term_prob_ = avg;

    //itero sui topic
    for(vector< words_topic_t >::iterator topic_it = words_by_topic_.begin(); topic_it != words_by_topic_.end(); topic_it++)
    {
        //itero sulle parole di un topic
        for(words_topic_t::iterator word_it = topic_it->begin(); word_it != topic_it->end(); word_it++)
             std_dev += pow(word_it->second - avg_term_prob_, 2);
    }

    std_dev/=words_by_topic_.size() * n_words_;
    stddev_term_prob_ = sqrt(std_dev);
//    cout << "media: " << avg_term_prob << " std dev : " << stddev_term_prob << endl;
}

// calcolo della validità
int Clustering::set_validity(int index)
{
    int validity;

    /* un cluster NON è valido se:
       - è vuoto
       o
       - contiene almeno un documento ed il documento con probabilità più alta ha tale probabilità minore della media del dataset
       negli altri casi il cluster è valido */
    if ( documents_by_topic_[index].size() <= 0
         ||
         (documents_by_topic_[index].size() > 0 && (documents_by_topic_[index][0].second) <= avg_doc_prob_ ) )
        validity = 0;
    else
        validity = 1;

    return validity;
}


// funzioni che restituiscono il clustering

// senza ulteriori filtri
std::vector<NewsCluster>& Clustering::get_clusters_vector()
{
    int ith_topic,              // indice dell'i-esimo topic
        validity;               // intero di validità

    std::vector<NewsCluster> * clusters = new std::vector<NewsCluster>();       // clustering
    NewsCluster * nc;                                                           // cluster temporaneo

    // itero sui topic e per ogni topic produco il relativo cluster
    for(vector< documents_topic_t >::iterator topic_it = documents_by_topic_.begin(); topic_it != documents_by_topic_.end(); topic_it++)
    {
        // calcolo il numero del topic per differenza tra i puntatori
        ith_topic = topic_it - documents_by_topic_.begin();
        // calcolo la validità
        validity = set_validity(ith_topic);

        // il cluster del topic i-esimo è costituito dalle parole e dai documenti di quel topic
        nc = new NewsCluster(words_by_topic_[ith_topic], documents_by_topic_[ith_topic], validity, use_shingles_);
        clusters->push_back(*nc);
    }

    // ordino i cluster secondo compareClusters
    std::sort(clusters->begin(), clusters->end(), compareClusters);
    return *clusters;
}


// ritorna true se news_body possiede almeno una delle parole in words
bool Clustering::has_at_least_one_word(std::vector<std::string> &words, std::string &news_body)
{
    // cerco ogni shingle del vettore nella stringa news_body
    for (vector<string>::iterator word = words.begin(); word != words.end(); word++)
    {
        if (news_body.find(*word) != std::string::npos) // è presente almeno una delle parole più probabili
            return true;
    }

    // altrimenti nessuna delle parole in words è presente in news_body
    return false;
}

// aggiorna la frequenza della mappa di parole rispetto al corpo della news news_body
void Clustering::update_map_frequency(std::map<std::string, int> &words_map, std::string &news_body)
{
    size_t i = 0;
    size_t j = 0;
    string word;                                // parola nel corpo della news
    map<string, int>::iterator result;          //per la find sulla mappa

    while (i < news_body.length())
    {
        // calcolo del primo carattere della parola successiva
        while (news_body[i] == ' ' && i < news_body.length()) i++;

        // calcolo del primo spazio dopo la parola che inizia posizione i
        j = i;
        while (news_body[j] != ' ' && j < news_body.length()) j++;

        word = news_body.substr(i, j - i);
        result = words_map.find(word);
        // se la parola è stata trovata nella mappa, aumento la frequenza
        if (result != words_map.end())
            result->second++;

        i = j + 1;
    }
}


/* filtra i documenti che non presentano nessuna delle num_words parole più probabili del topic a cui appartengono
   NOTA: se num_words è maggiore di n_words_, vengono considerate solo n_words_ parole */
std::vector<NewsCluster>& Clustering::get_clusters_vector_with_filter(index_t num_words)
{
    int ith_topic,                              // indice dell'i-esimo topic
        validity,                               // intero di validità
        number;                                 // numero di parole da considerare
    vector<string> words;                       // vettore contenente le number parole che posseggono la probabilità più alta
    string news_body;                           // corpo di una news



    // ====================== [INIZIO] controllo ======================
    // mappa di controllo e relativo file di output
//    map<std::string, int> words_map;                                            // chiave: parola e valore: frequenza
//    std::ofstream output_file ("./temp_clusteringLDA/words_frequency_per_clusters");

//    // controllo se il file di output è aperto
//    if (!output_file.is_open()) {
//        std::cerr << "Unable to write to file " << output_file << std::endl;
//        exit(1);
//    }
    // ====================== [FINE] controllo ======================

    // il numero di parole che considero è il minimo tra quelli richiesti e quelli del modello
    number = std::min(num_words, n_words_);

    // itero sui topic e per ogni topic elimino i documenti che non contengono nessuna delle number parole più probabili
    for(vector< documents_topic_t >::iterator topic_it = documents_by_topic_.begin(); topic_it != documents_by_topic_.end(); topic_it++)
    {
        // calcolo il numero del topic per differenza tra i puntatori
        ith_topic = topic_it - documents_by_topic_.begin();

        // inserisco nel vettore words le parole con la probabilità più alta dell'i-esimo cluster
        for (int j = 0; j < number; j++) {
            words.push_back(words_by_topic_[ith_topic][j].first);
        }

        // ====================== [INIZIO] controllo ======================
        // inserisco nella map words_map le parole con la probabilità più alta dell'i-esimo cluster
//        for (int j = 0; j < number; j++) {
//            // nota: la frequenza iniziale è zero
//            words_map.insert(std::make_pair(words_by_topic_[ith_topic][j].first, 0));
//        }

        // itero sui documenti del topic e per ogni documento aggiorno la mappa: aggiorno la frequenza della parola
//        for (documents_topic_t::iterator doc_it = topic_it->begin(); doc_it != topic_it->end(); doc_it++) {
//            news_body = dataset_[doc_it->first].body();
//            update_map_frequency(words_map, news_body);
//        }

        // scrittura sul file di output delle frequenze delle parole
//        output_file << (topic_it->size());
//        for (map<string, int>::iterator vi = words_map.begin(); vi != words_map.end(); vi++) {
//            output_file << "\t(" << (vi->first) << ", " << (vi->second) << ")";
//        }
//        output_file << std::endl;

//        words_map.clear();
        // ====================== [FINE] controllo ======================

        // itero sui documenti e cancello
        for (documents_topic_t::iterator doc_it = topic_it->begin(); doc_it != topic_it->end(); doc_it++) {
            news_body = dataset_[doc_it->first].body();
            if (!has_at_least_one_word(words, news_body)) {
                doc_it = topic_it->erase(doc_it);
                /* cancellare un elemento fa diminuire la dimensione della struttura:
                   devo considerare il nuovo elemento puntato dal puntatore appena usato*/
                doc_it--;
            }
        }

        // svuoto il vettore di parole per la prossima iterazione
        words.clear();
    }

    // ====================== [INIZIO] controllo ======================
    // chiudo il file di output
//    output_file.close();
    // ====================== [FINE] controllo ======================

    /* ricalcola i valori statistici per i documenti, dato che l'eliminazione potrebbe
       aver cambiato la numerosità del dataset*/
    set_docs_statistics();

    std::vector<NewsCluster> * clusters = new std::vector<NewsCluster>();       // clustering
    NewsCluster * nc;                                                           // cluster temporaneo

    // itero sui topic e per ogni topic produco il relativo cluster
    for(vector< documents_topic_t >::iterator topic_it = documents_by_topic_.begin(); topic_it != documents_by_topic_.end(); topic_it++)
    {
        // calcolo il numero del topic per differenza tra i puntatori
        ith_topic = topic_it - documents_by_topic_.begin();
        // calcolo la validità
        validity = set_validity(ith_topic);

        // il cluster del topic i-esimo è costituito dalle parole e dai documenti di quel topic
        nc = new NewsCluster(words_by_topic_[ith_topic], documents_by_topic_[ith_topic], validity, use_shingles_);
        clusters->push_back(*nc);
    }

    // ordino i cluster secondo compareClusters
    sort(clusters->begin(), clusters->end() ,compareClusters);

    return *clusters;
}


//membri statici

void Clustering::FillVector(std::vector<prob_t>* row, const std::string &str)
{
    size_t start_pos = 0;                  // indice del primo carattere di una parola in str
    size_t end_pos;                        // indice del primo spazio dopo la parola che inizia in start_pos
    size_t length;                         // lunghezza della parola

    // inizializzo end_pos alla fine della prima probabilità
    end_pos = str.find(' ', start_pos);

    // vengono lette tutte le probabilità di str: ogni probabilità viene inserita in row
    while(end_pos != std::string::npos)
    {
        length = end_pos - start_pos;
        row->push_back(StringToNumber<prob_t>(str.substr(start_pos, length)));
        start_pos = end_pos + 1;
        end_pos = str.find(' ', start_pos);
    }
}

void Clustering::ParseTwordsLine(std::vector<word_t> * row, const std::string &str)
{
    const int num_spaces = 3;                       // parola e probabilità sono separati da 3 SPAZI
    size_t start_pos = 1;                           // non considero il tab iniziale: str[0] = '\t'
    size_t end_pos = str.find(' ', start_pos);      // ultimo carattere della parola
    size_t length = end_pos - start_pos;            // lunghezza della parola

    std::string word (str.substr(start_pos, length));

    start_pos = end_pos + num_spaces;               // inizio della probabilità: la fine della parola più i 3 SPAZI
    // fino a std::string::npos, ovvero la fine della stringa
    prob_t p = StringToNumber<prob_t>(str.substr(start_pos, std::string::npos));
    row->push_back(std::make_pair(word, p));
}
