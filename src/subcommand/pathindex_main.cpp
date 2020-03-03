#include "subcommand.hpp"
#include "odgi.hpp"
#include "args.hxx"
#include "algorithms/xp.hpp"

namespace odgi {

    using namespace odgi::subcommand;
    using namespace xp;

    int main_pathindex(int argc, char** argv) {

        for (uint64_t i = 1; i < argc-1; ++i) {
            argv[i] = argv[i+1];
        }
        const std::string prog_name = "odgi pathindex";
        argv[0] = (char*)prog_name.c_str();
        --argc;

        args::ArgumentParser parser("create a path index for a given graph");
        args::HelpFlag help(parser, "help", "display this help summary", {'h', "help"});
        args::ValueFlag<std::string> dg_in_file(parser, "FILE", "load the graph from this file", {'i', "idx"});
        args::ValueFlag<std::string> idx_out_file(parser, "FILE", "store the index in this file", {'o', "out"});

        try {
            parser.ParseCLI(argc, argv);
        } catch (args::Help) {
            std::cout << parser;
            return 0;
        } catch (args::ParseError e) {
            std::cerr << e.what() << std::endl;
            std::cerr << parser;
            return 1;
        }
        if (argc==1) {
            std::cout << parser;
            return 1;
        }

        // read in the graph
        graph_t graph;
        assert(argc > 0);
        std::string infile = args::get(dg_in_file);
        if (infile.size()) {
            if (infile == "-") {
                graph.deserialize(std::cin);
            } else {
                ifstream f(infile.c_str());
                graph.deserialize(f);
                f.close();
            }
        }

        XP path_index;
        path_index.from_handle_graph(graph);
        size_t path_count = path_index.path_count;
        if (path_count == 1) {
            std::cout << "Indexed 1 path." << std::endl;
        } else {
            std::cout << "Indexed " << path_index.path_count << " paths." << std::endl;
        }

#ifdef debug_pathindex
        size_t pangenome_pos = path_index.get_pangenome_pos("5", 1);
        std::cerr << "Pangenome position for input \"5\":1 in constructed index is: " << pangenome_pos << std::endl;
        pangenome_pos = path_index.get_pangenome_pos("5", 2);
        std::cerr << "Pangenome position for input \"5\":2 in constructed index is: " << pangenome_pos << std::endl;
        pangenome_pos = path_index.get_pangenome_pos("5", 13);
        std::cerr << "Pangenome position for input \"5\":13 in constructed index is: " << pangenome_pos << std::endl;
        pangenome_pos = path_index.get_pangenome_pos("5", 5);
        std::cerr << "Pangenome position for input \"5\":5 in constructed index is: " << pangenome_pos << std::endl;
        pangenome_pos = path_index.get_pangenome_pos("5", 12);
        std::cerr << "Pangenome position for input \"5\":12 in constructed index is: " << pangenome_pos << std::endl;
#endif

        // writ out the index
        std::ofstream out;
        out.open(args::get(idx_out_file));
        std::cout << "Writing index to " << args::get(idx_out_file) << std::endl;
        path_index.serialize_members(out);
        out.close();

        std::cout << "Reading index from " << args::get(idx_out_file) << std::endl;
        XP path_index_1;
        std::ifstream in;
        in.open(args::get(idx_out_file));
        path_index_1.load(in);
        in.close();

        size_t bin_id_1 = path_index_1.get_pangenome_pos("5", 5);
        std::cout << "Pangenome position \"5\":5 in loaded index is: " << bin_id_1 << std::endl;
        bin_id_1 = path_index_1.get_pangenome_pos("5-", 5);
        std::cout << "Pangenome position \"5-\":5 in loaded index is: " << bin_id_1 << std::endl;
        bin_id_1 = path_index_1.get_pangenome_pos("5", 12);
        std::cout << "Pangenome position \"5-\":12 in loaded index is: " << bin_id_1 << std::endl;

        std::cout << path_index_1.has_path("5") << std::endl;
        std::cout << path_index_1.has_path("5-") << std::endl;
        std::cout << path_index.has_path("34adf") << std::endl;
        std::cout << path_index.has_position("5", 3) << std::endl;
        std::cout << path_index.has_position("5", 45) << std::endl;
        std::cout << path_index.has_position("543", 3) << std::endl;

        return 0;
    }

    static Subcommand odgi_pathindex("pathindex", "create a path index for a given graph",
                               PIPELINE, 3, main_pathindex);


}
