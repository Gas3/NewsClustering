#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>

#include "news.h"
#include "filter.h"
#include "newscluster.h"
#include "dictionary.h"

// classe che gestisce:
// -> lettura da file di un dataset di news
// -> scrittura su file dell'input delle libreria
// -> scrittura su file del dizionario
// -> scrittura su file del clustering di news

class FileManager
{
public:
    FileManager(const std::string &filepath);

    /* lettura da file del dataset, applicando ad ogni news il filtro
       restitiusce un vettore di news (vedi classe News) */
    std::vector<News>& read_news_from_file(Filter &filter);

    // scrittura del file di input della libreria
    void write_library_input(std::vector<News> &dataset);

    /* scrittura su file dei documenti il cui corpo risulta vuoto,
       dopo l'applicazione del filtro */
    void write_empty_documents(std::vector<News> &dataset);

    // scrittura su file del dizionario usato per l'Idf
    void write_dictionary_file(const Dictionary& dict);

    // scrittura su file del clustering di news
    void write_clustering_file(std::vector<NewsCluster> &clusters);

    /* scrittura su file del clustering di news, vengono scritti solo i cluster
       la cui dimensione è superiore alla soglia minima indicata */
    void write_clustering_file_with_threshold(std::vector<NewsCluster> &clusters, int threshold);

    // ritorna il nome del file che gestisce
    std::string get_file_name();

private:
    std::string filepath_;          //nome del file da gestire

    // ritorna la sottostringa str[start_position; end_position]
    // end_position è restituito in output, contiene l'indice del carattere successivo al primo '\t'
    static std::string NextSubString(std::string str, int start_position, int &end_position);

    // riempie news_vector di News, costruite a partire dalla stringa str in cui, ogni campo,
    // è separato da un altro da '\t'.
    static void FillVector(std::vector<News> &news_vector, const std::string &str, Filter filter);
};

#endif // FILEMANAGER_H
