//
// This file gives a small tour of the iod::parse_command_line features.
// (and also test it)
//

#define IOD_PCL_WITH_EXCEPTIONS

// symbols.hh contains the definitions of all _xxx variables.
// it is generated by running this command "iod_generate_symbols this_file.cc symbols.hh"
#include "symbols.hh"

// The command line parser.
#include "iod/parse_command_line.hh"

// Just for debugging purpose.
#include "iod/json.hh"

int main()
{
  using namespace s;
  using namespace iod;

  // Simple options
  {
    const char* argv[] = {"", "--opt1" , "12", "--opt2", "abc"};
    auto opts = parse_command_line(5, argv,
                                   _opt1 = int(),
                                   _opt2 = std::string());

    std::cout << json_encode(opts) << std::endl;
    assert(opts.opt1 == 12 and opts.opt2 == "abc");
  }

  // Positionals
  {
    const char* argv[] = {"", "abc", "1.23", "--opt1" , "12"};
    auto opts = parse_command_line(5, argv,
                                   cl::positionals(_opt2, _opt3),
                                   _opt1 = int(),
                                   _opt2 = std::string(),
                                   _opt3 = float());

    assert(opts.opt1 == 12 and opts.opt2 == "abc" and std::abs(opts.opt3 - 1.23) < 0.00001);
    //std::cout << json_encode(opts) << std::endl;
  }

  // Defaults
  {
    const char* argv[] = {""};
    auto opts = parse_command_line(1, argv,
                                   _opt1 = int(3),
                                   _opt2 = std::string("abc"));

    assert(opts.opt1 == 3 and opts.opt2 == "abc");
    std::cout << json_encode(opts) << std::endl;
  }

  // Pointers
  {
    const char* argv[] = {"", "--opt1", "42"};
    int opt1 = 0;
    parse_command_line(3, argv,
                       _opt1 = &opt1);

    assert(opt1 == 42);
  }

  // Switches
  {
    const char* argv[] = {"", "--opt1", "0", "-abc", "-d=0"};
    auto opts = parse_command_line(4, argv,
                                   _opt1 = bool(),
                                   _a = bool(),
                                   _b = bool(),
                                   _c = bool(),
                                   _d = bool());

    assert(opts.opt1 == false and
           opts.a == true and
           opts.b == true and
           opts.c == true and
           opts.d == false);
    std::cout << json_encode(opts) << std::endl;
  }
  
  // equals option assignments
  {
    const char* argv[] = {"", "--opt1=43", "-a=23"};
    auto opts = parse_command_line(3, argv,
                                   _opt1 = int(),
                                   _a = int(3));

    std::cout << json_encode(opts) << std::endl;
    assert(opts.opt1 == 43 and opts.a == 23);
  }
  
  // shortcuts names
  {
    const char* argv[] = {"", "-1" , "3", "-2", "abc"};
    auto opts = parse_command_line(5, argv,
                                   _opt1 | _1 = int(),
                                   _opt2 | _2 = std::string());

    std::cout << json_encode(opts) << std::endl;
    assert(opts.opt1 == 3 and opts.opt2 == "abc");
  }

  // vectors
  {
    const char* argv[] = {"", "-a", "1", "-a", "2", "-a", "3", "-a", "4" };
    auto opts = parse_command_line(9, argv, _a = std::vector<int>());
    std::cout << json_encode(opts) << std::endl;
    assert(opts.a.size() == 4 and
           opts.a[0] == 1 and
           opts.a[1] == 2 and
           opts.a[2] == 3 and
           opts.a[3] == 4);
  }

  
  // required args
  {
    const char* argv[] = {"", "-opt1" , "3"};
    std::string err;
    try
    {
      auto opts = parse_command_line(3, argv,
                                     cl::required(_opt1, _opt2),
                                     _opt1 | _1 = int(),
                                     _opt2 | _2 = std::string());
    }
    catch (std::runtime_error e)
    {
      err = e.what();
    };
    assert(err == "Error missing required command line parameter opt2\n");
  }

  // Description.
  {
    const char* argv[] = {"./test_program", "--help"};
    try
    {
      auto opts = parse_command_line(2, argv,
                                     cl::required(_opt1, _a),
                                     cl::positionals(_opt1, _opt2),
                                     cl::description(
                                       "This is a test program",
                                       _opt1 = "Set the first option of our test program.\n second line",
                                       _opt2 = "Set the second option of our test program.",
                                       _opt3 = "A vector of strings."),

                                     _opt1 | _1 = int(),
                                     _opt2 | _2 = std::string(),
                                     _opt3 = std::vector<std::string>(),
                                     _a = bool(),
                                     _b = bool(),
                                     _c = bool(),
                                     _d = bool()
        );

// This should print:
// Usage: ./test_program [options...] [opt1] [opt2]
// This is a test program

//   --opt1|-1 int            [REQUIRED] Set the first option of our test program.
//                             second line
//   --opt2|-2 string         Set the second option of our test program.
//   --opt3 vector<string>    A vector of strings.
//   -a                       [REQUIRED] 
//   -b                       
//   -c                       
//   -d   
    }
    catch(...)
    {
    }
  }
  
}
