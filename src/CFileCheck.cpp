#include <CStrUtil.h>
#include <CFile.h>

#include <accessor.h>
#include <iostream>
#include <cstdlib>

#define LINE_LEN 100

void process_file(const std::string &filename);

class CFileCheck {
 public:
  CFileCheck() { }

  ACCESSOR(OnlyFileName    , bool, only_filename)
  ACCESSOR(CheckExtraSpace , bool, check_extra_space)
  ACCESSOR(CheckTabs       , bool, check_tabs)
  ACCESSOR(CheckBlanks     , bool, check_blanks)
  ACCESSOR(CheckDups       , bool, check_dups)
  ACCESSOR(CheckLength     , int , check_length)
  ACCESSOR(CheckInitSpacing, bool, check_init_spacing)
  ACCESSOR(CheckEqualsAlign, bool, check_equals_align)
  ACCESSOR(CheckSemiColon  , bool, check_semi_colon)
  ACCESSOR(CheckKeywords   , bool, check_keywords)

  void processFile(const std::string &filename);

  bool error(const std::string &msg, const std::string &detail="");

 private:
  bool                   only_filename_ { false };
  bool                   check_extra_space_ { false };
  bool                   check_tabs_ { false };
  bool                   check_blanks_ { false };
  bool                   check_dups_ { false };
  int                    check_length_ { 0 };
  bool                   check_init_spacing_ { false };
  bool                   check_equals_align_ { false };
  bool                   check_semi_colon_ { false };
  bool                   check_keywords_ { false };
  std::string            filename_;
  uint                   line_num_ { 0 };
  std::string::size_type last_eq_ { std::string::npos };
};

int
main(int argc, char **argv)
{
  bool only_filename      = false;
  bool check_extra_space  = false;
  bool check_tabs         = false;
  bool check_blanks       = false;
  bool check_dups         = false;
  int  check_length       = 0;
  bool check_init_spacing = false;
  bool check_equals_align = false;
  bool check_semi_colon   = false;
  bool check_keywords     = false;

  std::vector<std::string> filenames;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      if      (argv[i][1] == 'f') only_filename     = true;
      else if (argv[i][1] == 's') check_extra_space = true;
      else if (argv[i][1] == 't') check_tabs        = true;
      else if (argv[i][1] == 'b') check_blanks      = true;
      else if (argv[i][1] == 'd') check_dups        = true;
      else if (argv[i][1] == 'l') {
        check_length = atoi(&argv[i][2]);

        if (check_length == 0)
          check_length = LINE_LEN;
      }
      else if (argv[i][1] == 'i')
        check_init_spacing = true;
      else if (argv[i][1] == 'q')
        check_equals_align = true;
      else if (argv[i][1] == 'S')
        check_semi_colon = true;
      else if (argv[i][1] == 'k')
        check_keywords = true;
      else if (argv[i][1] == 'a') {
        check_extra_space  = true;
        check_tabs         = true;
        check_blanks       = true;
        check_dups         = true;
        check_length       = LINE_LEN;
        check_init_spacing = true;
        check_equals_align = true;
        check_semi_colon   = true;
        check_keywords     = true;
      }
      else if (argv[i][1] == 'h') {
        std::cerr << "CFileCheck -f|-s|-t|-b|-d|-l|-i|-q|-S|-k|-a" << std::endl;
        exit(1);
      }
      else
        std::cerr << "Invalid Option " << argv[i] << std::endl;
    }
    else
      filenames.push_back(argv[i]);
  }

  if (! check_extra_space  &&
      ! check_tabs         &&
      ! check_blanks       &&
      ! check_dups         &&
      ! check_length       &&
      ! check_init_spacing &&
      ! check_equals_align &&
      ! check_semi_colon   &&
      ! check_keywords) {
    check_extra_space  = true;
    check_tabs         = true;
    check_blanks       = true;
    check_dups         = true;
    check_length       = LINE_LEN;
    check_init_spacing = false;
    check_equals_align = false;
    check_semi_colon   = false;
    check_keywords     = true;
  }

  CFileCheck check;

  check.setOnlyFileName    (only_filename);
  check.setCheckExtraSpace (check_extra_space);
  check.setCheckTabs       (check_tabs);
  check.setCheckBlanks     (check_blanks);
  check.setCheckDups       (check_dups);
  check.setCheckLength     (check_length);
  check.setCheckInitSpacing(check_init_spacing);
  check.setCheckEqualsAlign(check_equals_align);
  check.setCheckSemiColon  (check_semi_colon);
  check.setCheckKeywords   (check_keywords);

  int num_filenames = filenames.size();

  for (int i = 0; i < num_filenames; ++i)
    check.processFile(filenames[i]);
}

void
CFileCheck::
processFile(const std::string &filename)
{
  filename_ = filename;
  line_num_ = 1;
  last_eq_  = std::string::npos;

  CFile file(filename_);

  if (! file.exists() || ! file.isRegular()) {
    std::cerr << "File '" << filename_ << "' does not exists" << std::endl;
    return;
  }

  std::string line, sline, last_line, last_sline, detail;

  int  num_blank = 0;
  bool end_block = false;

  while (file.readLine(line)) {
    sline = CStrUtil::stripSpaces(line);

    if (getCheckExtraSpace()) {
      size_t len = line.size();

      if (len > 0 && isspace(line[len - 1])) {
        if (error("Extra Space"))
          break;
      }
    }

    if (getCheckTabs()) {
      if (line.find('\t') != std::string::npos) {
        if (error("Tabs"))
          break;
      }
    }

    if (getCheckBlanks()) {
      int len = line.size();

      if (sline.size() == 0) {
        ++num_blank;

        if (num_blank == 2) {
          if (error("Extra Blank Lines"))
            break;
        }
      }
      else
        num_blank = 0;

      if (end_block) {
        if (sline.size() != 0) {
          if (error("Block without spacer line"))
            break;
        }

        end_block = false;
      }

#if 0
      if ((len > 0 && line[0] == '}') ||
          (line.rfind("};") != std::string::npos))
        end_block = true;
#else
      if (len > 0 && line[0] == '}')
        end_block = true;
#endif
    }

    if (getCheckDups() && last_sline != "" && sline.size() > 2) {
      if (sline == last_sline) {
        if (error("Dup Line", line))
          break;
      }
    }

    if (getCheckLength()) {
      int len = line.size();

      if (len > getCheckLength()) {
        detail = CStrUtil::strprintf("%d > %d", len, getCheckLength());

        if (error("Too long", detail))
          break;
      }
    }

    if (getCheckInitSpacing()) {
      uint num = 0;
      uint len = line.size();

      for (uint i = 0; i < len && isspace(line[i]); ++i)
        ++num;

      if (num < len && (num % 2) != 0) {
        bool skip = false;

        if (! skip && (line.find(';') == std::string::npos))
          skip = true;

        if (! skip) {
          char c1 = line[num];
          char c2 = line[len - 1];
          char c3 = (len > 1 ? line[len - 2] : '\0');

          if (c1 == '(' && c2 == ';' && c3 == ')')
            skip = true;

          if (c1 == '~')
            skip = true;
        }

        if (! skip) {
          uint len1 = last_sline.size();

          if (len1 > 0 &&
              (last_sline[len1 - 1] == ',' ||
               last_sline[len1 - 1] == '+' ||
               last_sline[len1 - 1] == '-' ||
               last_sline[len1 - 1] == '*' ||
               last_sline[len1 - 1] == '/' ||
               last_sline[len1 - 1] == '%' ||
               last_sline[len1 - 1] == '?' ||
               last_sline[len1 - 1] == ':' ||
               last_sline[len1 - 1] == '.' ||
               last_sline[len1 - 1] == '\"' ||
               last_sline[len1 - 1] == '|' ||
               last_sline[len1 - 1] == '&'))
            skip = true;
        }

        if (! skip) {
          if (error("Bad indent"))
            break;
        }
      }
    }

    if (getCheckEqualsAlign()) {
      std::string::size_type p = line.find('=');

      if (p != std::string::npos) {
        if      (line[p + 1] == '=')
          p = std::string::npos;
        else if (p > 0 && line[p - 1] == '!')
          p = std::string::npos;
        else if (last_eq_ != std::string::npos) {
          uint indent1 = 0;
          uint indent2 = 0;

          while (indent1 < last_line.size() && isspace(last_line[indent1]))
            ++indent1;
          while (indent2 < line.size() && isspace(line[indent2]))
            ++indent2;

          if (indent1 != indent2)
            p = std::string::npos;
        }
      }

      if (p != std::string::npos && last_eq_ != std::string::npos) {
        if (p != last_eq_) {
          if (error("Bad equals align"))
            break;
        }
      }

      last_eq_ = p;
    }

    if (getCheckSemiColon()) {
      if (line.find(";;") != std::string::npos) {
        if (error("Double semi-colon"))
          break;
      }
    }

    if (getCheckKeywords()) {
      const char *keywords[] = {
        "if",
        "do",
        "while",
        "repeat",
        "for",
        "else",
        "return",
        "switch",
      };

      for (uint i = 0; i < sizeof(keywords)/sizeof(keywords[0]); ++i) {
        std::string pattern = std::string(keywords[i]) + "(";

        std::string::size_type pos = line.find(pattern);

        if (pos != std::string::npos && pos > 0 && isalpha(line[pos - 1]))
          pos = std::string::npos;

        if (pos != std::string::npos) {
          if (error("keyword " + std::string(keywords[i]) + " used as a function"))
            break;
        }
      }
    }

    last_line  = line;
    last_sline = sline;

    ++line_num_;
  }

  if (getCheckBlanks()) {
    if (num_blank == 1) {
      (void) error("Extra Blank Lines");
    }
  }
}

bool
CFileCheck::
error(const std::string &msg, const std::string &detail)
{
  if (! getOnlyFileName()) {
    std::cout << filename_ << "@" << line_num_ << ": " << msg;

    if (detail != "")
      std::cout << " [" << detail << "]";

    std::cout << std::endl;

    return false;
  }
  else {
    std::cout << filename_ << std::endl;

    return true;
  }
}
