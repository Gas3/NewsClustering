#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "filemanager.h"
#include "conversion.h"

using namespace std;

using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::cout;
using std::cerr;
using std::endl;

FileManager::FileManager(const string &filepath) : filepath_(filepath) {}

// nota: il filtro viene applicato al corpo di ogni news
vector<News>& FileManager::read_news_from_file(Filter &filter)
{
    string line;
    vector<News> * dataset = new vector<News>();
    ifstream dataset_file ((filepath_).c_str());

    // legge ogni riga del file e, richiamando la funzione FillVector, inserisce
    // un nuovo oggetto News (costruito a partire da line) in news_vector
    if (dataset_file.is_open())
    {
        while (dataset_file.good())
        {
            getline (dataset_file, line);
            // inserisce una nuova news nel vettore
            if (line.size() != 0){

                FillVector(*dataset, line, filter);
            }
        }
        dataset_file.close();
    }
    else {
        cerr << "File " << filepath_ << "not found" << endl;
        exit(1);
    }

    return *dataset;
}

void FileManager::write_library_input(std::vector<News> &dataset)
{
    int n_news = 0;
    ofstream output_file ((filepath_).c_str());

    // conto il numero di news che non hanno il corpo vuoto
    for (vector<News>::iterator news_it = dataset.begin(); news_it != dataset.end(); news_it++)
        if ( !(news_it->is_empty()) )
            n_news++;

    if (output_file.is_open())
    {
        output_file << n_news;
        //scrivo il corpo delle news, per cui tale campo non è vuoto
        for (vector<News>::iterator news_it = dataset.begin(); news_it != dataset.end(); news_it++)
        {
            // scrivo prima il carattere \n per evitare di inserirlo per l'ultima news
            if ( !(news_it->is_empty()) )
                output_file << endl << news_it->body();
        }
        output_file.close();
    }
    else {
        cerr << "Error: Unable to write to file " << filepath_ << endl;
        exit(1);
    }
}

void FileManager::write_empty_documents(std::vector<News> &dataset)
{
    ofstream output_file ((filepath_).c_str());

    if (output_file.is_open())
    {
        /* scrivo il vecchio corpo delle news, per cui tale campo è risultato essere vuoto
           dopo l'applicazione dei filtri*/
        for (vector<News>::iterator news_it = dataset.begin(); news_it != dataset.end(); news_it++)
        {
            if ( (news_it->is_empty()) )
                output_file << (news_it - dataset.begin()) << " " << news_it->old_body() << endl;
        }
        output_file.close();
    }
    else {
        cerr << "Error: Unable to write to file " << filepath_ << endl;
        exit(1);
    }
}

void FileManager::write_dictionary_file(const Dictionary& dict)
{
    std::ofstream output_file (filepath_.c_str());

    if (output_file.is_open())
    {
        dict.print_dictionary(output_file);
        output_file.close();
    }
    else {
        cerr << "Error: Unable to write to file " << filepath_ << endl;
        exit(1);
    }
}

void FileManager::write_clustering_file(std::vector<NewsCluster> &clusters)
{
    std::ofstream output_file (filepath_.c_str()); 

    if (output_file.is_open())
    {
        for (vector<NewsCluster>::iterator news_it = clusters.begin(); news_it != clusters.end(); news_it++)
            output_file << *news_it << endl;
        output_file.close();
    }
    else {
        cerr << "Error: Unable to write to file " << filepath_ << endl;
        exit(1);
    }
}

void FileManager::write_clustering_file_with_threshold(std::vector<NewsCluster> &clusters, int threshold)
{
    std::ofstream output_file (filepath_.c_str());

    if (output_file.is_open())
    {
        for (vector<NewsCluster>::iterator news_it = clusters.begin(); news_it != clusters.end(); news_it++)
        {
            if (news_it->size() >= threshold)
                output_file << *news_it << endl;
        }
        output_file.close();
    }
    else {
        cerr << "Error: Unable to write to file " << filepath_ << endl;
        exit(1);
    }
}

std::string FileManager::get_file_name()
{
    size_t startpos = filepath_.find_last_of('/');
    if (startpos != std::string::npos)
        return filepath_.substr(startpos + 1, (filepath_.size() - startpos));
    else
        return filepath_;
}


// membri statici

std::string FileManager::NextSubString(std::string str, int start_position, int &end_position)
{
    end_position = str.find('\t', start_position);
    int length = end_position - start_position;
    return str.substr(start_position,length);
}

void FileManager::FillVector(std::vector<News>& news_vector, const std::string &str, Filter filter)
{
    int end_pos = 0;        // indice del primo spazio successivo ad una parola
    // nota: va incrementato dopo la chiamata di NextSubString per spostarsi sul prossimo caratterere

    int epoch;                  //
    //string id_source;         //
    //string newspaper;         // campi di una news
    //string title;             //
    //string body;              //
    int id_category;            //
    //string link;              //

    // costruisce tutti i campi di News a partire da str e, poi, inserisce
    // il rispettivo oggetto News in news_vector.
    epoch = StringToNumber<int>(NextSubString(str, end_pos, end_pos));
    ++end_pos;
    string id_source (NextSubString(str, end_pos, end_pos));
    ++end_pos;
    string newspaper (NextSubString(str, end_pos, end_pos));
    ++end_pos;
    string title (NextSubString(str, end_pos, end_pos));
    ++end_pos;

    /* ########################################## */
    string body (NextSubString(str, end_pos, end_pos));
    // al corpo viene appeso il titolo, che può risultare utile ai fini del clustering
    body.append(" " + title);

    // applico il filtro al corpo
    filter.apply(body);
    ++end_pos;
    /* ########################################## */

    id_category = StringToNumber<int>(NextSubString(str, end_pos, end_pos));
    ++end_pos;
    string link (NextSubString(str, end_pos, end_pos));

    news_vector.push_back(News (epoch, id_source, newspaper, title, body, id_category, link));
}
