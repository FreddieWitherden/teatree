/*
    This file is part of teatree.
    Copyright (C) 2011–2012  Freddie Witherden <freddie@witherden.org>

    teatree is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    teatree is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with teatree.  If not, see
    <http://www.gnu.org/licenses/>.
*/

#include "config.h"

#include "available_simulations.hpp"
#include "simulation/particle_filter.hpp"
#include "simulation/progress.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/bind.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/program_options.hpp>
#include <boost/ref.hpp>
#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>
#include <fstream>
#include <set>
#include <csignal>
#include <stdexcept>
#include <string>

using boost::archive::text_iarchive;
using boost::archive::text_oarchive;
using boost::iostreams::filtering_istream;
using boost::phoenix::bind;
using boost::phoenix::arg_names::arg1;
using boost::phoenix::ref;
using namespace boost::program_options;
namespace mpl = boost::mpl;
using namespace teatree;

typedef boost::shared_ptr<simulation> simulation_ptr;

/**
 * The number of times the process has received the SIGINT signal.
 */
static bool sigint_caught = false;

/**
 *
 */
template<typename F>
struct simulation_type_find
{
    simulation_type_find(const std::string& sim_type, F f)
        : sim_type_(sim_type), f_(f)
    {}

    template<typename SimT>
    void operator()(mpl::identity<SimT>)
    {
        if (simulation_traits<SimT>::type() == sim_type_)
            f_(mpl::identity<SimT>());
    }

    const std::string& sim_type_;
    F f_;
};

/**
 * For each simulation type known to teatree this function invokes the
 *  (templated) functor, f.
 */
template<typename F>
static inline void for_each_sim_type(F f)
{
    mpl::for_each<available_simulations, mpl::make_identity<mpl::_> >(f);
}

/**
 * For the simulation whose type matches sim_type the (templated)
 *  functor f is invoked.
 */
template<typename F>
static inline void for_sim_type(const std::string& sim_type, F f)
{
    for_each_sim_type(simulation_type_find<F>(sim_type, f));
}


/**
 * Prints out a textual description of the supported simulation types.
 */
struct simulation_desc_printer
{
    template<typename SimulationT>
    void operator()(mpl::identity<SimulationT>) const
    {
        std::cout << "\n" << simulation_traits<SimulationT>::desc();
    }
};

/**
 * Templated functor to construct a simulation into a provided smart
 *  pointer.
 */
struct simulation_type_constructor
{
    simulation_type_constructor(simulation_ptr& ptr,
                                std::istream& is,
                                const simulation_options& so)
        : ptr_(ptr), is_(is), so_(so)
    {}

    template<typename SimulationT>
    void operator()(mpl::identity<SimulationT>)
    {
        ptr_ = simulation_ptr(new SimulationT(is_, so_));
    }

    simulation_ptr& ptr_;
    std::istream& is_;
    const simulation_options& so_;
};

/**
 * Templated functor which attempts to load a simulation from an
 *  archive into a provided smart pointer.
 */
template<typename ArchiveT>
struct simulation_type_load
{
    simulation_type_load(ArchiveT& ar,
                         simulation_ptr& ptr)
        : ar_(ar), ptr_(ptr)
    {}

    template<typename SimulationT>
    void operator()(mpl::identity<SimulationT>)
    {
        ptr_ = simulation_ptr(new SimulationT);
        ar_ & (*boost::dynamic_pointer_cast<SimulationT>(ptr_));
    }

    ArchiveT& ar_;
    simulation_ptr& ptr_;
};

/**
 * Saves a simulation to an archive.
 */
template<typename ArchiveT>
struct simulation_type_save
{
    simulation_type_save(ArchiveT& ar,
                         simulation_ptr ptr)
        : ar_(ar), ptr_(ptr)
    {}

    template<typename SimulationT>
    void operator()(mpl::identity<SimulationT>)
    {
        ar_ & (*boost::dynamic_pointer_cast<SimulationT>(ptr_));
    }

    ArchiveT& ar_;
    simulation_ptr ptr_;
};

static simulation_ptr simulation_load(std::istream& is)
{
    text_iarchive ia(is);

    // Load the name of the simulation being serialized
    std::string sim_type;
    ia >> sim_type;

    // Check that we support the request simulation type
    bool found = false;
    for_sim_type(sim_type, ref(found) = true);

    if (!found) throw std::invalid_argument("Simulation type "
                                          + sim_type
                                          + " is not available");

    // Load the actual simulation from the archive
    simulation_ptr sim;
    for_sim_type(sim_type, simulation_type_load<text_iarchive>(ia, sim));

    return sim;
}

/**
 * Checks to see if the requested simulation type is compiled into this
 *  build of teatree.
 */
static void validate_simulation_type(const std::string& sim_type)
{
    bool found = false;

    // Search for the simulation type
    for_sim_type(sim_type, ref(found) = true);

    // Throw if we can not find it
    if (!found) throw std::invalid_argument("Simulation type '"
                                          + sim_type
                                          + "' is not available");
}

/**
 *
 */
static void simulation_save(std::ostream& os, simulation_ptr sim)
{
    text_oarchive oa(os);

    // Save the name of the simulation being serialized
    const std::string sim_type = sim->type();
    oa << sim_type;

    // Serialize the simulation
    for_sim_type(sim_type, simulation_type_save<text_oarchive>(oa, sim));
}

static std::string simulation_base_name(const std::string& file_name)
{
    return file_name.substr(0, file_name.find("-", 0));
}

/**
 * Slot which is called after each completed iteration.  This takes
 *  care of updating the progress bar and of requesting premature
 *  termination if a SIGINT signal has been received.
 */
static bool simulation_update_progress(const simulation&,
                                       const simulation::iteration_stats&,
                                       simulation_progress<>& prog)
{
    // If we have been asked to break then do so
    if (sigint_caught)
        return true;
    // Otherwise update the progress bar and return true
    else
    {
        // Update the progress bar
        ++prog;
        return false;
    }
}

static bool simulation_process_stats(const simulation& sim,
                                     const simulation::iteration_stats& stats,
                                     std::ostream& os)
{
    // Unpack the iteration stats
    double times[3]; int64_t vis[2]; int neval;
    boost::tie(times[0], times[1], times[2], vis[0], vis[1], neval) = stats;

    std::ostringstream ss;
    ss << std::setprecision(5);

    ss << std::setw(6) << sim.completed_steps();
    for (int i = 0; i < 3; ++i)
        ss << std::setw(12) << times[i];
    for (int i = 0; i < 2; ++i)
        ss << std::setw(12) << vis[i];
    ss << std::setw(6) << neval;

    // Write them out
    os << ss.str() << "\n";

    return false;
}

/**
 * Handles the "init" command line action.
 */
static void process_init(const std::string& input_file,
                         const std::string& output_file,
                         const std::string& sim_type,
                         const simulation_options& so)
{
    // Attempt to open the provided particle file
    std::ifstream ifs(input_file.c_str());

    // Error out if there was an issue opening the file
    if (!ifs.good())
        throw std::runtime_error("can't open " + input_file);

    /*
     * Attach a particle_filter to the stream.  This performs input validation,
     * strips out comments ('#' lines) and handles headers at the start of a
     * file.
     */
    filtering_istream filt_ifs;
    filt_ifs.push(particle_filter(sim_type[1] - '0'));
    filt_ifs.push(ifs);

    std::cout << "Creating simulation of type " << sim_type << "\n";

    // Create the simulation
    simulation_ptr sim;
    for_sim_type(sim_type, simulation_type_constructor(sim, filt_ifs, so));

    // Output the final parameters being used by the simulation
    std::cout << "Parameters: " << sim->parameters() << "\n";

    // Pick the output file name
    const std::string ofname = output_file + "-00000.tts";

    std::cout << "Serializing to " << ofname << "\n";

    // Serialize it so it can be run at a later date
    std::ofstream ofs(ofname.c_str());
    simulation_save(ofs, sim);

    std::cout << "Done :-)\n";
}

/**
 * Signal handler for dealing with SIGINT.
 */
static void process_run_sigint(int)
{
    sigint_caught = true;

    std::cout << "\nCtrl+C caught; will break after this interation"
                 "\nSend again to IMMEDIATELY terminate tearee"
              << std::endl;
}

/**
 * Handles the "run" command line action.
 */
static void process_run(const std::string& input_file, bool verbose)
{
    std::ifstream ifs(input_file.c_str());
    std::ofstream ofs_stats;

    if (!ifs.good())
        throw std::runtime_error("can't open " + input_file);

    std::cout << "Attempting to restore " << input_file << "\n";

    // Unserialize the simulation
    simulation_ptr sim = simulation_load(ifs);

    std::cout << "Loaded simulation of type " << sim->type() << "\n";
    std::cout << "Parameters: " << sim->parameters() << "\n";

    boost::shared_ptr<std::ostream> stat_of;

    // In verbose mode output per-iteration statistics
    if (verbose)
    {
        // Compute the stats file name and open the file
        const std::string name = sim->options().output_basename()
                               + "-stats.txt";
        ofs_stats.open(name.c_str());

        // Register the callback to output the stats
        sim->do_on_iteration(boost::bind(&simulation_process_stats,
                                         _1, _2, boost::ref(ofs_stats)));


        std::cout << "Writing iteration statistics to " << name << "\n";

    }

    const int completed = sim->completed_steps();
    const int total     = sim->total_steps();

    if (completed == 0)
        std::cout << "Starting run\n";
    else
        std::cout << "Resuming run at iteration " << completed+1 << "\n";

    // Create a progress bar
    simulation_progress<> prog(total, completed);

    // Register our callback to update the progress bar
    sim->do_on_iteration(boost::bind(&simulation_update_progress,
                                     _1, _2, boost::ref(prog)));

    // Handle Ctrl+C (aka SIGINT)
    struct sigaction sigint_handler;
    sigint_handler.sa_handler = &process_run_sigint;
    sigemptyset(&sigint_handler.sa_mask);
    sigint_handler.sa_flags = SA_RESETHAND;

    sigaction(SIGINT, &sigint_handler, 0);

    // Run the simulation
    std::cout << "\n";
    bool save = sim->run();
    std::cout << "\n\n";

    // Save if we have not finished
    if (save)
    {
        // Get the base name of the simulation "foo-00100.tts" => "foo"
        const std::string name = simulation_base_name(input_file);

        // Generate the new file name of the form "foo-<completed>.tts"
        std::ostringstream os;
        os << name << "-" << std::setw(5) << std::setfill('0')
           << sim->completed_steps() << ".tts";

        std::cout << "Saving the simulation to " << os.str() << "\n";

        // Save the simulation
        std::ofstream ofs(os.str().c_str());
        simulation_save(ofs, sim);
    }

    std::cout << "Done :-)\n";
}

static void notify_output_steps(simulation_options* so,
                                const std::vector<std::string>& op)
{
    std::set<int> steps;

    // Build a set out of the provided options
    BOOST_FOREACH(const std::string& s, op)
        steps.insert(boost::lexical_cast<int>(s));

    so->output_steps(steps);
}

int main(int argc, char* argv[])
{
    std::string simulation_type;
    simulation_options so;

    // Generic command line options
    options_description generic_opt("Generic options");
    generic_opt.add_options()
    ("help", "Displays this information")
    ("version", "Displays version information")
    ("config,c", value<std::string>(), "Configuration file")
    ("verbose,v", value<bool>()->default_value(false)->zero_tokens(),
     "Enable verbose output");

    // Init specific options
    options_description init_opt("Init options");
    init_opt.add_options()
    ("simulation-type,t",
     value(&simulation_type)->notifier(validate_simulation_type)->required(),
     "Type of simulation to create; see below for available types")
    ("output-steps,s",
     value<std::vector<std::string> >()->multitoken()->composing()->required()
     ->notifier(bind(&notify_output_steps, &so, arg1)),
     "Steps where output should be generated")
    ("base-name,o",
     value<std::string>()->default_value("out")
                         ->notifier(bind(&simulation_options::output_basename,
                                         &so, arg1)),
     "Base file name for generated output; combined with the iteration count "
     "to give the output file name <base>-<iter #>.txt")
    ("no-header,p",
     value<bool>()->zero_tokens()
                  ->notifier(bind(&simulation_options::output_header, &so, !arg1)),
     "Do not include a header in generated output files")
    ("nd",
     value<double>()->required()
                    ->notifier(bind(&simulation_options::nd, &so, arg1)),
     "Number of electrons/ions in the Debye sphere/circle; this is required "
     "to correctly characterise the evolution of the plasma")
    ("epsilon",
     value<double>()->required()
                    ->notifier(bind(&simulation_options::epsilon, &so, arg1)),
     "Plummer softening parameter > 0")
    ("dt",
     value<double>()->required()
                    ->notifier(bind(&simulation_options::dt, &so, arg1)),
     "Particle pusher timestep > 0")
    ("theta",
     value<double>()->required()
                    ->notifier(bind(&simulation_options::theta, &so, arg1)),
     "MAC parameter >= 0")
    ("qtom-cutoff",
     value<double>()->default_value(0.0)
                    ->notifier(bind(&simulation_options::qtomcutoff, &so, arg1)),
     "Charge-to-mass cutoff ratio; particles with an absolute ratio less than "
     "this are assumed to be immobile with a vanishing acceleration; this is "
     "useful for simulating one component plasmas")
    ("min-bound",
     value<double>()->default_value(0.0)
                    ->notifier(bind(&simulation_options::min_bound, &so, arg1)),
     "Minimum boundary coord for reflective boundaries; not relevant for open "
     "boundary simulations")
     ("max-bound",
      value<double>()->default_value(32.0)
                     ->notifier(bind(&simulation_options::max_bound, &so, arg1)),
      "Maximum boundary coord for reflective boundaries; not relevant for open "
      "boundary simulations");

    // Hidden (positional) options
    options_description hidden_opt("Hidden options");
    hidden_opt.add_options()
    ("action",     value<std::string>(), "action")
    ("input-file", value<std::string>(), "input file")
    ("output-file",value<std::string>(), "output name");

    positional_options_description pdesc;
    pdesc.add("action", 1)
         .add("input-file", 1)
         .add("output-file", 1);

    // Valid options on the command line
    options_description cli_opt("Command line options");
    cli_opt.add(generic_opt).add(hidden_opt).add(init_opt);

    // Valid options inside of a config file
    options_description config_opt("Configuration file options");
    config_opt.add(init_opt);

    try
    {
        variables_map cli_vm;

        /*
         * Argument parsing is a little complicated as, depending on
         * the action, we support various sets of arguments.  Here we
         * parse all available options but do not validate or notify
         * any of them.  Validation is performed only once the action
         * is known.
         */
        store(command_line_parser(argc, argv).options(cli_opt)
                                             .positional(pdesc)
                                             .run(),
              cli_vm);

        // Handle --version
        if (cli_vm.count("version"))
        {
            std::cout << "teatree "   TEATREE_VERSION
                         " (" TEATREE_GIT_SHA1 ")"
                         " built on " TEATREE_BUILD_DATE  "\n";
            return 0;
        }

        // Handle --help (or display if required arguments are missing)
        if (cli_vm.count("help")
         || !cli_vm.count("action")
         || !cli_vm.count("input-file"))
        {
            // General help
            std::cout << "Usage: teatree [OPTION...] <action> <action args>\n"
                         "<action> is one of\n"
                         "  init <[IN] particle file> <[OUT] simulation name>\n"
                         "  run  <[IN] simulation file>\n\n"
                      << generic_opt << "\n"
                      << init_opt << "\n";
            // Supported simulation types + descriptions
            std::cout << "Available simulations:";
            for_each_sim_type(simulation_desc_printer());
            return 0;
        }

        // Process the action
        const std::string& action = cli_vm["action"].as<std::string>();
        if (action == "init")
        {
            variables_map init_vm;

            // Process the init specific options
            store(command_line_parser(argc, argv).options(init_opt)
                                                 .allow_unregistered()
                                                 .run(),
                  init_vm);

            // If a config file was specified then parse it
            if (cli_vm.count("config"))
                store(parse_config_file<char>(cli_vm["config"].as<std::string>()
                                                              .c_str(),
                                              config_opt),
                      init_vm);

            // Call notify to perform parameter validation on the init options
            notify(init_vm);

            process_init(cli_vm["input-file"].as<std::string>(),
                         cli_vm["output-file"].as<std::string>(),
                         simulation_type, so);
        }
        else if (action == "run")
            process_run(cli_vm["input-file"].as<std::string>(),
                        cli_vm["verbose"].as<bool>());
        else
            throw std::invalid_argument("bad action");
    }
    catch (std::exception& ex)
    {
        std::cout << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
