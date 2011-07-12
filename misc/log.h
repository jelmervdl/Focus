#ifndef __MISC_LOG__
#define __MISC_LOG__

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

enum Importances {
    TRIVIAL = 0,
    COMMON= 1,
    SPECIAL = 2,
    USEFULL = 3,
    MAX = 4,
    EXTRA = 5
};

static unsigned int log_print_importance = USEFULL;

inline void set_log_print_importance(unsigned int importance)
{
    log_print_importance = importance;   
}

template <typename T>
inline T string_to_t(std::string str)
{
    std::istringstream iss(str);
    T value;
    iss >> value;
    return value;
}


template <typename T>
inline std::string t_to_string(T value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}



template <typename T>
inline void log(T const &iets, unsigned int importance = EXTRA, bool new_line = true)
{
  log(t_to_string(iets), importance, new_line);
}

template <>
inline void log<std::string>(std::string const &sentence, unsigned int importance, bool new_line)
{
    if (importance >= log_print_importance)
    {
        std::cerr << sentence;
        if (new_line)
            std::cerr << std::endl;
    }
    std::cerr.flush();
}

template <typename T>
inline void log2(std::string const &pre_sentence, T const &iets, unsigned int importance = EXTRA)
{
    log(pre_sentence, importance, false);
    log(t_to_string(iets), importance, true);
}


template <typename V>
inline std::string t_to_string(std::vector<V> const &value)
{
    std::ostringstream oss;
    for (unsigned int i(0); i < value.size(); ++i)
        oss  << t_to_string<>(value[i]) << " ";
    return oss.str();
}

#endif
