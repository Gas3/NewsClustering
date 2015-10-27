#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>

// header progetto
#include "news.h"
#include "newscluster.h"
#include "filter.h"
#include "filemanager.h"
#include "clustering.h"
#include "dictionary.h"
#include "conversion.h"
#include "newsmodel.h"

// header della libreria GibbsLDA++
#include "lda/model.h"

#define DEBUG_MOD 0

using namespace std;

// costanti
const string TEMPDIR ("./temp_clusteringLDA/");         // path della cartella di output dei file generati
const string LIBINPUT ("library_input_file");
const int MASK = 0777;                                  // permessi della directory di output

// controlla se la directory esiste
bool DirectoryExists(const std::string path)
{
    DIR *dir;
    bool exists = false;

    dir = opendir (path.c_str());

    if (dir != 0) {
        exists = true;
        (void) closedir (dir);
    }

    return exists;
}

// crea una directory se non esiste già
void CreateDirectory (const std::string path)
{
    if (!DirectoryExists(path)) {
        mkdir(path.c_str(), MASK);
    }
}

// ritorna il nomefile dato un path del tipo "/*/*/nomefile"
std::string FileName(std::string path)
{
    size_t start_pos = path.find_last_of("/") + 1;
    size_t end_pos = path.find_last_of(".", start_pos);
    if(end_pos > start_pos)
        return path.substr(start_pos, end_pos - start_pos);   // Elimina 8 caratteri dal nome del file (.stemmed)
    else
        return path.substr(start_pos, std::string::npos);
}

// imposta la filter_mask
void FilterMaskSetting(bitMask& filter_mask, NewsModel &model)
{
    filter_mask.set(PUNCTUATION_FILTER_BIT, model.use_punctuation_filter);
    filter_mask.set(STOPWORDS_FILTER_BIT, model.use_stopwords_filter);
    filter_mask.set(USE_SHINGLES_BIT, model.use_shingles);
    filter_mask.set(IDF_FILTER_BIT, model.use_idf_filter);
}

// configurazione del modello LDA
void LDASetting(model &lda, NewsModel &nm, const string &input_file_path) {
    // imposto i parametri di defualt della libreria
    lda.set_default_values();
    // opzione -est della libreria GibbsLDA++
    lda.model_status = MODEL_STATUS_EST;

    lda.dir = TEMPDIR;

    // file su cui applicare la libreria GibbsLDA++
    lda.dfile = input_file_path;

    // numero di iterazioni che verranno eseguite dalla libreria GibbsLDA++
    lda.niters = nm.num_iters;

    // numero di iterazioni dopo le quali viene effettuato un salvataggio intermedio
    lda.savestep = nm.save_step;

    // numero di parole più probabili per ogni topic
    lda.twords = nm.num_words_cluster;

    // numero di topic (cluster)
    lda.K = nm.num_cluster;

    // modifica di alfa e beta, stessi valori della libreria se non inseriti da riga di comando
    lda.alpha = nm.alpha;
    lda.beta = nm.beta;
}

void help_text(char *name)
{
    cout << "Usage: " << name << " [options] dataset_file" << endl;
    cout << "Options:" << endl;
    cout << "  --help\tDisplay this information" << endl;
    cout << "  -v\t\tDisplay the program parameters" << endl;
    cout << "  -a alpha\tSet the alpha paramater (double) for clustring" << endl;
    cout << "  -b beta\tSet the beta paramater (double) for clustring" << endl;
    cout << "  -n clusters\tSet the number of clusters to generate" << endl;
    cout << "  -t terms\tSet the number of most probable terms for the output" << endl;
    cout << "  -m size\tSet the minimum size (number of documents) of output clusters" << endl;
    cout << "  -p terms\tSet the number of most probable terms per cluser for filtering document," << endl;
    cout << "\t\tif a document of the cluster doesn't have any of these terms, it will be filtered" << endl;
    cout << "  -w words\tSet the number of words per shingles" << endl;
    cout << "  -i iter\tSet the number of iterations to do for clustering model" << endl;
    cout << "  -s step\tSet the step number of iterations for intermediate clustering model" << endl;
    cout << "  -o file\tPlace output in file file" << endl;
    cout << "  -c clust\tRun without doing clustring, but need dataset," << endl;
    cout << "\t\tand procude only output from clustring file clust" << endl;
    cout << "  -d string\tDisable filters base on which character the string contains:" << endl;
    cout << "\t\t\t'.' disable the punctuation filter" << endl;
    cout << "\t\t\t's' disable the stopwords filter" << endl;
    cout << "\t\t\t'w' disable the use of shingles" << endl;
    cout << "\t\t\t'i' disable the idf filter" << endl;
    cout << "\t\t\t'm' disable the cluster filter" << endl;
    cout << "\t\t\t'p' disable the document filter" << endl;
    cout << "\t\tother characters will be ignored" << endl;
    cout << endl << "##### created by Ceppa I., Grandi M. and Ponza M. #####" << endl;
    exit(0);
}

void verbose_text(NewsModel &nm, bool clustering)
{
    if(clustering)
        cout << "alpha = " << nm.alpha << ", beta = " << nm.beta << endl;
    cout << "number of clusters: " << nm.num_cluster << endl;
    cout << "number of terms: " << nm.num_words_cluster << endl;
    if(nm.use_size_threshold)
        cout << "minimum size of a output cluster: " << nm.size_threshold << endl;
    else
        cout << "clusters not filtered" << endl;
    if(nm.use_words_threshold)
        cout << "filtering document base on the " << min(nm.num_words_cluster, nm.words_threshold) << " most probable terms" << endl;
    else
        cout << "documents not filtered" << endl;
    if(clustering) {
        cout << "number of iterations: " << nm.num_iters << endl;
        cout << "number of step iterations: " << nm.save_step << endl;
    }
    if(!nm.use_punctuation_filter)
        cout << "punctuation filter not used" << endl;
    if(!nm.use_stopwords_filter)
        cout << "stopwords filter not used" << endl;
    if(!nm.use_shingles)
        cout << "shingles not used" << endl;
    if(!nm.use_idf_filter)
        cout << "idf filter not used" << endl;
    cout << endl;

}

void disable_filters(NewsModel &nm, string what)
{
    for(string::iterator c = what.begin(); c != what.end(); c++)
    {
        switch(*c)
        {
            case '.': // punteggiatura
                nm.use_punctuation_filter = false;
                break;
            case 's': // stop words
                nm.use_stopwords_filter = false;
                break;
            case 'w': // shingles
                nm.use_shingles = false;
                break;
            case 'i': // idf
                nm.use_idf_filter = false;
                break;
            case 'm': // filtro dei cluster (in base alla dimensione)
                nm.use_size_threshold = false;
                break;
            case 'p': // filtro dei documenti (in base ai termini più probabili)
                nm.use_words_threshold = false;
                break;
            //altrimenti ignora
        }
    }
}

int main(int argc, char *argv[])
{
    vector<News> dataset;                               // vettore contenente le news
    vector<NewsCluster> result_clusters;                // vettore contenente i cluster delle news
    bitMask filter_mask;                                // maschera di bit usata per determinare quali filtri applicare
    string dataset_path;                                // path del file delle news
    string output_path ("clusters.out");                // output file dei clusters
    string clustering_path (TEMPDIR + "model-final");   // path del file di clustering
    NewsModel news_model;                               // modello con i parametri
    int option;                                         // opzione da riga di comando
    bool error_flag = false;                            // segnala se ci sono errori
    bool verbose_flag = false;                          // segnala se bisogna mostrare le infomazioni dell'esecuzione
    bool only_output = false;                           // segnala se bisogna evitare di utilizzare la libreria
    bool alpha_not_modify = true;                       // segnala che l'alfa non è stata modificata

    // variabile di getopt: 0 indica che non sono presenti errori
    opterr = 0;

    while ( (option = getopt(argc,argv,"a:b:n:t:m:p:w:i:s:o:c:d:vh"))!=-1 )
    {
        switch(option)
        {
            case 'a':
                alpha_not_modify = false;
                news_model.alpha = StringToNumber<double>(optarg);
                break;

            case 'b':
                news_model.beta = StringToNumber<double>(optarg);
                break;

            case 'n':
                news_model.num_cluster = StringToNumber<int>(optarg);
                if(alpha_not_modify)
                    news_model.alpha = 50.0 / static_cast<double>(news_model.num_cluster);
                break;

            case 't':
                news_model.num_words_cluster = StringToNumber<int>(optarg);
                break;

            case 'm':
                news_model.size_threshold = StringToNumber<int>(optarg);
                break;

            case 'p':
                news_model.words_threshold = StringToNumber<int>(optarg);
                break;

            case 'w':
                news_model.num_words_shingle = StringToNumber<int>(optarg);
                break;

            case 'i':
                news_model.num_iters = StringToNumber<int>(optarg);
                break;

            case 's':
                news_model.save_step = StringToNumber<int>(optarg);
                break;

            case 'o':
                output_path = string(optarg);
                cout << "output file: " << output_path << endl;
                break;

            case 'c':
                only_output = true;
                clustering_path = string(optarg);
                cout << "clustering files: " << clustering_path << ".theta & " << clustering_path << ".twords" << endl;
                break;

            case 'd':
                disable_filters(news_model, string(optarg));
                break;

            case 'v':
                verbose_flag = true;
                break;

            case 'h':
                help_text(argv[0]);
                break;

            default: error_flag = true;
        }
    }

    if(optind == (argc - 1)) {
        dataset_path = string(argv[optind]);
        cout << "file: " << dataset_path << endl;
    }
    else {
        cerr << argv[0] << ": no input dataset" << endl;
        cerr << "Usage: " << argv[0] << " [options] dataset" << endl;
        exit(1);
    }

    if(error_flag) {
        cerr << argv[0] << ": error" << endl;
        cerr << "Usage: " << argv[0] << " [options] dataset" << endl;
        exit(1);
    }

    if(verbose_flag) {
        verbose_text(news_model, !only_output);
    }

    // #################### PREPROCESSING ####################
    FileManager dataset_file (dataset_path);

    // setto la maschera e creo il filtro
    FilterMaskSetting(filter_mask, news_model);
    Filter filtro (filter_mask, news_model.num_words_shingle, news_model.max_idf_threshold, news_model.min_idf_threshold);

    cout << "Filtering body news..." << endl;
    cout << "Reading the dataset from file..." << endl;
    if(news_model.use_punctuation_filter)
        cout << "\t...applying punctuation filter..." << endl;
    if(news_model.use_stopwords_filter)
        cout << "\t...applying stopwords filter..." << endl;
    if(news_model.use_shingles)
        cout << "\t...using shingles of words..." << endl;
    dataset = dataset_file.read_news_from_file(filtro);

    // cartella di output
    CreateDirectory(TEMPDIR);

    if(news_model.use_idf_filter)
    {
        cout << "Creating dictionary and computing Idf..." << endl;
        const Dictionary dict(dataset);

        cout << "Applying Idf filer..." << endl;
        filtro.apply_idf(dataset, dict);

        // scrittura sul file del dizionario
        FileManager dictionary_file (TEMPDIR + "clustering.dictionary");
        cout << "Saving dictionary on disk..." << endl;
        dictionary_file.write_dictionary_file(dict);
    }

    cout << "Dataset for clustering built." << endl;
    // #################### END PREPROCESSING ####################


    // #################### LIBRARY ####################
    if(!only_output)
    {
        FileManager library_input_file (TEMPDIR + LIBINPUT);

#if DEBUG_MOD
        // per debug, scrittura su file dei documenti eliminati dal filtro
        FileManager emptydoc (TEMPDIR + "clustering.empty");
        emptydoc.write_empty_documents(dataset);
#endif

        // scrittura su file dell'input per la libreria
        library_input_file.write_library_input(dataset);

        model lda;
        // impostazione dei parametri per l'utilizzo della libreria GibbsLDA++
        LDASetting(lda, news_model, library_input_file.get_file_name());

        cout << "Clustering..." << endl;

        // stima del modello da zero
        if (lda.init_est()) {
            return 1;
        }
        // esecuzione dell'LDA
        lda.estimate();

    }
    // #################### END LIBRARY ####################


    // #################### POSTPROCESSING ####################
    // controllo per la stringa vuota
    if(output_path == "" || output_path == " ") {
        output_path = "clusters.out";
    }

    cout << "Writing clustering file..." << endl;

    Clustering c (news_model.num_cluster, news_model.num_words_cluster, clustering_path + ".theta",
                              clustering_path + ".twords", dataset, news_model.use_shingles);
    if(news_model.use_words_threshold)
        result_clusters = c.get_clusters_vector_with_filter(news_model.words_threshold);
    else
        result_clusters = c.get_clusters_vector();

    FileManager result_file = FileManager(output_path);
    if (news_model.use_size_threshold)
        result_file.write_clustering_file_with_threshold(result_clusters, news_model.size_threshold);
    else
        result_file.write_clustering_file(result_clusters);

    cout << "\t ...file " << output_path << " created." << endl;
    // #################### END POSTPROCESSING ####################

    return 0;

}
