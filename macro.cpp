// TODO: 抽象简化逻辑，详细报错位置
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <sstream>

using namespace std;

stringstream FILEBUF;

bool PRINT_TO_FILE = false;
string TARGET_FILE;

vector<string> split_str(string str) {
  vector<string> strs;
  size_t start = 0;
  size_t pos = str.find(" ");
  while(pos != string::npos) {
    strs.push_back(str.substr(start, pos - start));
    start = pos + 1;
    pos = str.find(" ", start);
  }
  strs.push_back(str.substr(start, pos - start));
  return strs;
}

string join_str(vector<string> arr) {
  string buf = "";
  buf += arr[0];
  for (size_t i = 1; i < arr.size(); i++) {
    buf += " " + arr[i];
  }
  return buf;
}

static string MACRO_OP  = ".macro";
static string ENDM_OP   = ".endm";
static string DEFINE_OP = ".define";
static string REPEAT_OP = ".repeat";
static string INCLUDE_OP = ".include";
static string IMPORT_OP = ".import";

class Macro {
private:
  static vector<Macro*> macros;
  static unordered_map<string, Macro*> macros_table;
  string name;
  string content;

  // TODO: 增加重定义检验
  void Register() {
    macros.push_back(this);
    macros_table[name] = this;
  }

  void set_content(string m_content) {
    content = m_content;
  }

  static string need_replace(string line) {
    for (size_t i = 0; i < macros.size(); i++) {
      if (line.find(macros[i]->name) != string::npos) {
        return macros[i]->name;
      }
    }
    return "";
  }

public:
  Macro(string m_name): name(m_name) {Register();}

  static void load_file(string path) {
    ifstream file(path);
    if (!file) {
      clear();
      std::cerr << "Error: can not open " << path << "." << std::endl;
      exit(1);
    }
    stringstream filebuf;
    filebuf << file.rdbuf();
    file.close();
    bool is_full = true;
    string tmp_buf = "";
    std::string line;
    while (getline(filebuf, line)) {
      if (line.starts_with(MACRO_OP)) {
        is_full = false;
        if (split_str(line).size() != 2) {
          cerr << "Error: `.macro` must set a name." << endl;
          exit(1);
        }
        string name = split_str(line)[1];
        new Macro(name);
      }
      else if (line.starts_with(ENDM_OP)) {
        is_full = true;
        macros.back()->set_content(tmp_buf);
        tmp_buf.clear();
      }
      else if (!is_full) {
        tmp_buf += line + "\n";
      }
      else if (line.starts_with(DEFINE_OP)) {
        vector<string> str_group = split_str(line);
        if (str_group.size() < 2) {
	  clear();
          cerr << "Error: `.define` must follow the format: `.define name [words]`. It accept a name at least." << endl;
          exit(1);
        }
        string name = str_group[1];
        Macro* new_macro = new Macro(name);
	if (str_group.size() > 2) {
	  tmp_buf = join_str(vector<string>(str_group.begin()+2, str_group.end()));
	  new_macro->set_content(tmp_buf);
	} else {
	  new_macro->set_content(string(""));
	}
	tmp_buf.clear();
      }
      else if (line.starts_with(REPEAT_OP)) {
        vector<string> str_group = split_str(line);
        if (str_group.size() != 4) {
	  clear();
          cerr << "Error: `.repeat` must follow the format: `.define name times word`" << endl;
          exit(1);
        }
        string name = str_group[1];
        size_t counts = *str_group[2].c_str() - 40;
        Macro* new_macro = new Macro(name);
        for (size_t i = 0; i < counts; i++) {
          tmp_buf += str_group[3];
        }
        new_macro->set_content(tmp_buf);
	tmp_buf.clear();
      }
      else if (line.starts_with(INCLUDE_OP)) {
	vector<string> str_group = split_str(line);
	if (str_group.size() < 2) {
	  clear();
          cerr << "Error: `.repeat` must follow the format: `.define name times word`" << endl;
          exit(1);
	}
	load_file(str_group[1]);
      }
      else if (line.starts_with(IMPORT_OP)) {
	vector<string> str_group = split_str(line);
	if (str_group.size() < 2) {
	  clear();
          cerr << "Error: `.repeat` must follow the format: `.define name times word`" << endl;
          exit(1);
	}
	load_macro(str_group[1]);
      }
      else {
	size_t start = 0;
        string _name = need_replace(line);
	size_t pos = line.find(_name);
	while (!_name.empty()) {
	  FILEBUF << line.substr(start, pos - start);
	  FILEBUF << macros_table[_name]->content;
	  start = pos + _name.size();
	  _name = need_replace(line.substr(start));
	  pos = line.find(_name);
	}
	if (start == 0) {
	  FILEBUF << line << "\n";
	} else {
	  FILEBUF << line.substr(start) << "\n";
	}
      }
    }
    if (!is_full) {
      clear();
      cerr << "Error: " << path << "'s `.macro` have not close." << endl;
      exit(1);
    }
  }

  static void load_macro(string path) {
    ifstream file(path);
    if (!file) {
      clear();
      std::cerr << "Error: can not open " << path << "." << std::endl;
      exit(1);
    }
    stringstream filebuf;
    filebuf << file.rdbuf();
    file.close();
    bool is_full = true;
    string tmp_buf = "";
    std::string line;
    while (getline(filebuf, line)) {
      if (line.starts_with(MACRO_OP)) {
        is_full = false;
        if (split_str(line).size() != 2) {
          cerr << "Error: `.macro` must set a name." << endl;
          exit(1);
        }
        string name = split_str(line)[1];
        new Macro(name);
      }
      else if (line.starts_with(ENDM_OP)) {
        is_full = true;
        macros.back()->set_content(tmp_buf);
        tmp_buf.clear();
      }
      else if (!is_full) {
        tmp_buf += line + "\n";
      }
      else if (line.starts_with(DEFINE_OP)) {
        vector<string> str_group = split_str(line);
        if (str_group.size() < 2) {
	  clear();
          cerr << "Error: `.define` must follow the format: `.define name [words]`. It accept a name at least." << endl;
          exit(1);
        }
        string name = str_group[1];
        Macro* new_macro = new Macro(name);
	if (str_group.size() > 2) {
	  tmp_buf = join_str(vector<string>(str_group.begin()+2, str_group.end()));
	  new_macro->set_content(tmp_buf);
	} else {
	  new_macro->set_content(string(""));
	}
	tmp_buf.clear();
      }
      else if (line.starts_with(REPEAT_OP)) {
        vector<string> str_group = split_str(line);
        if (str_group.size() != 4) {
	  clear();
          cerr << "Error: `.repeat` must follow the format: `.define name times word`" << endl;
          exit(1);
        }
        string name = str_group[1];
        size_t counts = *str_group[2].c_str() - 40;
        Macro* new_macro = new Macro(name);
        for (size_t i = 0; i < counts; i++) {
          tmp_buf += str_group[3];
        }
        new_macro->set_content(tmp_buf);
	tmp_buf.clear();
      }
      else if (line.starts_with(INCLUDE_OP)) {
	vector<string> str_group = split_str(line);
	if (str_group.size() < 2) {
	  clear();
          cerr << "Error: `.repeat` must follow the format: `.define name times word`" << endl;
          exit(1);
	}
	load_file(str_group[1]);
      }
      else if (line.starts_with(IMPORT_OP)) {
	vector<string> str_group = split_str(line);
	if (str_group.size() < 2) {
	  clear();
          cerr << "Error: `.repeat` must follow the format: `.define name times word`" << endl;
          exit(1);
	}
	load_macro(str_group[1]);
      }
      else {}
    }
    if (!is_full) {
      clear();
      cerr << "Error: " << path << "'s `.macro` have not close." << endl;
      exit(1);
    }
  }

  static void print() {
    cout << FILEBUF.str();
  }
  
  static void print_to_file(string file_name) {
    ofstream file(file_name);
    if (!file) {
      clear();
      cerr << "Can not output to " << file_name << "." << endl;
      exit(1);
    }
    file << FILEBUF.str();
  }
  
  static void clear() {
    for (size_t i = 0; i < macros.size(); i++) {
      delete macros[i];
    }
  }

  static void dump() {
    for (size_t i = 0; i < macros.size(); i++) {
      cout << "[name] " << macros[i]->name << endl;
      cout << "[content]\n" << macros[i]->content;
    }
    puts("\n============\ntarget content:");
    cout << FILEBUF.str();
  }
};
vector<Macro*> Macro::macros;
unordered_map<string, Macro*> Macro::macros_table;

// TODO: 更严谨的参数检查
void command_line(int args, char* argv[]) {
  if (args < 2) {
    cerr << "Error: Must accept a file name" << endl;
    exit(1);
  }
  for (int i = 1; i < args; i++) {
    string argvar = string(argv[i]);
    if (argvar == "-i") {
      PRINT_TO_FILE = true;
      TARGET_FILE = argv[args-1];
    }
    else if (argvar == "-o") {
      PRINT_TO_FILE = true;
      TARGET_FILE = argv[i+1];
      i++;
    }
    else if (argvar == "-f") {
      if (string(argv[i+1]) == "0") {
	MACRO_OP  = ".macro";
	ENDM_OP   = ".endm";
	DEFINE_OP = ".define";
	REPEAT_OP = ".repeat";
	INCLUDE_OP = ".include";
	IMPORT_OP = ".import";
      }
      else if (string(argv[i+1]) == "1") {
	MACRO_OP  = "#macro";
        ENDM_OP   = "#endm";
        DEFINE_OP = "#define";
        REPEAT_OP = "#repeat";
	INCLUDE_OP = "#include";
	IMPORT_OP = "#import";
      }
      else {
	cerr << "Error: unkownflag. `-f` only accept 0 or 1." << endl;
	exit(1);
      }
      i++;
    }
    else if (argvar == "--flag") {
      string prefix = argv[i+1];
      MACRO_OP  = prefix + "macro";
      ENDM_OP   = prefix + "endm";
      DEFINE_OP = prefix + "define";
      REPEAT_OP = prefix + "repeat";
      INCLUDE_OP = prefix + "include";
      IMPORT_OP = prefix + "import";
      i++;
    }
    else if (argvar == "--help") {
      cout << "help messagee:." << endl;
      cout << "\t-i\t- replace content in-place." << endl;
      cout << "\t-o file\t- output to file." << endl;
      cout << "\t-f 0/1\t- use diffevent style prefix(0 for . style, 1 for # style)." << endl;
      cout << "\t--flag prefix\t- use custom style prefix." << endl;
      cout << "\t--help\t- print this message." << endl;
      exit(0);
    }
  }
}

int main(int args, char* argv[]) {
  command_line(args, argv);
  Macro::load_file(argv[args-1]);
  // Macro::dump();
  if (PRINT_TO_FILE) {
    Macro::print_to_file(TARGET_FILE);
  } else {
    Macro::print();
  }
  Macro::clear();
  return 0;
}
