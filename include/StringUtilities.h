/*
 * Copyright (c) 2015, Alessandro Saccoia. All rights reserved.
 */
/*
 * Copyright (c) 2017, Alesandro Saccoia. All rights reserved.
 */

//  Created by Alessandro Saccoia on 7/18/12.

#ifndef dmaf_ios_StringUtilities_hpp
#define dmaf_ios_StringUtilities_hpp

#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <stdio.h>  /* defines FILENAME_MAX */
#include <iomanip>
#include <deque>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <cassert>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <dirent.h>
#include <stdexcept>
#include <sys/stat.h>


#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
    #ifdef ASU_PLATFORM_OSX
    #include <sys/types.h>
    #include <pwd.h>
    #include <uuid/uuid.h>
    #endif
#endif

namespace asu {
namespace utilities {

template <template<typename...> class ContainerT = std::vector>
inline ContainerT<std::string> tokenize(const std::string & str, const std::string & delim, bool toLower = false) {
  ContainerT<std::string> tokens;

  size_t p0 = 0, p1 = std::string::npos;
  while(p0 != std::string::npos)
  {
    p1 = str.find_first_of(delim, p0);
    if(p1 != p0)
    {
      std::string token = str.substr(p0, p1 - p0);
      if (toLower) {
        std::transform(token.begin(), token.end(), token.begin(), ::tolower);
      }
      tokens.push_back(token);
    }
    p0 = str.find_first_not_of(delim, p1);
  }
  return tokens;
}

std::string substringFromCharacter(const std::string& input, const std::string & delim) {
  return input.substr(input.find(delim) + 1);
}

long long millisecondsSinceEpoch() {
  std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
      std::chrono::system_clock::now().time_since_epoch()
  );
  return ms.count();
}

inline std::string concat(const std::vector<std::string>& tokens_, const std::string & delim_, size_t sIndex_, size_t eIndex_) {
  std::string reconstructedString("");
  for (size_t i = sIndex_; i < eIndex_; ++i) {
    reconstructedString += tokens_[i] + delim_;
  }
  reconstructedString += tokens_[eIndex_];
  return reconstructedString;
}

template <typename OutputType>
OutputType stringConverter(std::string fieldValue_) {
  std::stringstream m_idReader(fieldValue_);
  OutputType toReturn;
  m_idReader >> toReturn;
  if (m_idReader.fail()) {
    throw std::runtime_error("Parsing failed");
  }
  return toReturn;
}

template <typename InputType>
std::string toString(InputType input) {
  std::stringstream out;
  out << input;
  return out.str();
}

std::string getFullPath(std::string file);
std::string pathToFile(std::string file);
std::string pathToFile(std::string file, std::string ext);
std::string getEnclosingDirectory(std::string file);

inline bool isAbsolute(std::string file) {
  return file[0] == '/';
}

inline void ToUpper(const char* bufferIn, char* bufferOut, const int len) {
  for(int i = 0; i < len; ++i) {
    bufferOut[i] = toupper(bufferIn[i]);
  }
}

inline bool IsNumber(const std::string& s) {
  for (unsigned int i = 0; i < s.length(); i++) {
    if (!std::isdigit(s[i])) {
    	return false;
    }
  }
  return true;
}


// returns the date in format MM/DD/YY hh::mm::ss
inline std::string getCurrentDateAndTime() {
  std::ostringstream oss;
  
  time_t t = time(NULL);
	tm* timePtr = localtime(&t);
  
  oss << timePtr->tm_mon + 1 << "/";
  oss << timePtr->tm_mday << "/";
  oss << ( 1900 + timePtr->tm_year) << " ";
  
  oss << timePtr->tm_hour << ":";
  oss << timePtr->tm_min << ":";
  oss << timePtr->tm_sec;

  return oss.str();
}

// returns the date in format MM/DD/YY hh::mm::ss
inline std::string getCurrentDateAndTime(std::string separator) {
  std::ostringstream oss;
  
  time_t t = time(NULL);
	tm* timePtr = localtime(&t);
  
  oss << std::setfill('0');
  oss << std::setw(4) << (1900 + timePtr->tm_year) << separator;
  oss << std::setw(2) << timePtr->tm_mon << separator;
  oss << std::setw(2) << timePtr->tm_mday << separator;
  
  oss << std::setw(2) << timePtr->tm_hour << separator;
  oss << std::setw(2) << timePtr->tm_min << separator;
  oss << std::setw(2) << timePtr->tm_sec;

  return oss.str();
}

inline std::string GetWorkingDirectory() {
  static char cCurrentPath[FILENAME_MAX];

  if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
    return std::string();
  }

  cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

  return std::string(cCurrentPath);
}

inline std::string GetCurrentUser() {
  #ifdef ASU_PLATFORM_OSX
    struct passwd *passwd;
    passwd = getpwuid ( getuid());
    return passwd->pw_name;
  #else
    return "Not implemented";
  #endif
}

// changes the terminal settings, allowing one to get the key code
// of the button that was pressed without waiting for enter to be pressed
/*
  USAGE:
  for (;;) {
        key = getch();
        // terminate loop on ESC (0x1B) or Ctrl-D (0x04) on STDIN 
        if (key == 0x1B || key == 0x04) {
            break;
        }
        else {
            printf("%c\n", key));
        }
    }
*/
inline int getch(void) {
  int c=0;
#ifndef ASU_PLATFORM_CYGWIN
  struct termios org_opts, new_opts;
  int res=0;
      //-----  store old settings -----------
  res=tcgetattr(STDIN_FILENO, &org_opts);
  assert(res==0);
      //---- set new terminal parms --------
  memcpy(&new_opts, &org_opts, sizeof(new_opts));
  new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
  c=getchar();
      //------  restore old settings ---------
  res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
  assert(res==0);
#endif
return(c);
}
// throws exception
inline std::string getFileContentAsString(std::string path_) {
  std::ifstream ifs(path_.c_str());
  return std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
}

inline bool fileExists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

inline std::string getFileExtension(const std::string filePath_) {
  std::vector<std::string> tokens = tokenize(filePath_, ".");
  return tokens[tokens.size() - 1];
}

inline void toLower(std::string& str_) {
  std::transform(str_.begin(), str_.end(), str_.begin(), ::tolower);
}

inline std::string getFilenameFromPath(const std::string filePath_, bool removeExtension = false) {
  std::vector<std::string> tokens = tokenize(filePath_, "/");
  if (removeExtension) {
    return tokenize(tokens[tokens.size() - 1], ".")[0];
  }
  return tokens[tokens.size() - 1];
}

inline std::string getFilePathWithoutFilename(const std::string filePath_) {
  std::vector<std::string> tokens = tokenize(filePath_, "/");
  std::stringstream s;
  s << "/";
  std::copy(tokens.begin(),tokens.end() - 1, std::ostream_iterator<std::string>(s,"/"));
  return s.str();
}

template <template<typename...> class ContainerT = std::vector>
inline int getFilesInDirectory(ContainerT<std::string> &out_paths, const std::string& directory, const std::string& extension, std::vector<std::string> exclude = {})
{
	DIR *dir;
	class dirent *ent;
	class stat st;
	
	if ((dir = opendir (directory.c_str())) != NULL)
	{
		while ((ent = readdir (dir)) != NULL)
		{
			const std::string file_name = ent->d_name;
			const std::string full_file_name = directory + "/" + file_name;
			
			if (file_name[0] == '.') continue;
			if (stat(full_file_name.c_str(), &st) == -1) continue;
			const bool is_directory = (st.st_mode & S_IFDIR) != 0;
			
			if (is_directory)
			{
				ContainerT<std::string> results;
				getFilesInDirectory(results, full_file_name, extension);
				for (int i=0;i<results.size();i++) {
          if (find(exclude.begin(), exclude.end(), results[i]) == exclude.end()) {
            out_paths.push_back(results[i]);
          }
        }
			}
			else
			{
				if ( strstr( ent->d_name, extension.c_str())) {
          if (find(exclude.begin(), exclude.end(), file_name) == exclude.end()) {
            out_paths.push_back(full_file_name);
          }
        }
			}
		}
		closedir (dir);
		return 0;
	}
	else
	{
		perror ("");
		return EXIT_FAILURE;
	}
}

inline std::vector<std::string> findFolders(const std::string path)
{
	DIR *dir = opendir(path.c_str());
	std::vector<std::string> results;
	struct dirent *entry = readdir(dir);
	
	while (entry != NULL)
	{
		if (entry->d_type == DT_DIR && entry->d_name[0]!='.')
		{
			results.push_back(path + entry->d_name);
		}
		entry = readdir(dir);
	}
	
	closedir(dir);
	return results;
}

using namespace std;


inline void splitToFloats(const string& s, const char c, vector<float>& v) {
    string::size_type i = 0;
    string::size_type j = s.find(c);
    
    while (j != string::npos) {
        v.push_back(atof(s.substr(i, j-i).c_str()));
        i = ++j;
        j = s.find(c, j);
        
        if (j == string::npos)
            v.push_back(atof(s.substr(i, s.length()).c_str()));
    }
}

inline float splitToFloat(const string& s, char c)
{
	return atof(s.c_str());
}



inline void importDataFromFile(std::string fileName, std::vector<float>& result, const char splitter = ' ')
{
	ifstream inputFile(fileName);
	string line;
	int currentFrame = 0;
	
	while (getline(inputFile, line))
	{
		result.push_back(splitToFloat(line, splitter));
		currentFrame++;
	}
}

inline void importDataFromFile(std::string fileName, std::deque<std::vector<float>>& result, const char splitter = ' ')
{
  ifstream inputFile(fileName);
  string line;
	
	while (getline(inputFile, line)) {
		vector<float> values;
		splitToFloats(line, splitter, values);
    assert(values.size() > 0);
		result.emplace_back(values);
	}
}


inline void saveDataToFile(std::string filename, std::vector<std::vector<float>> data, std::string elementSeparator = "\n", bool separateRows = false){
  std::string directory = filename;
  std::ofstream file_write(directory.c_str());
  for(int i=0; i < data.size(); i++)
  {
      for(int j = 0; j < data[i].size(); j++) // Number of features
        file_write << data[i][j] << elementSeparator;
      if (separateRows)
        file_write << std::endl;
  }
  file_write.close();
}
inline void saveDataToFile(std::string filename, std::deque<std::vector<float>> data, std::string elementSeparator = "\n", bool separateRows = false){
  std::string directory = filename;
  std::ofstream file_write(directory.c_str());
  for(int i=0; i < data.size(); i++)
  {
      int j = 0;
      for(; j < data[i].size() - 1; j++) // Number of features
        file_write << data[i][j] << elementSeparator;
      file_write << data[i][j];
      if (separateRows)
        file_write << std::endl;
  }
  file_write.close();
}
template <class ContainerT>
inline void saveDataToFileForMatlab(std::string filename, const ContainerT& data){
  std::string directory = filename;
  std::ofstream file_write(directory.c_str());
  for(int i=0; i < data.size(); i++)
  {
    file_write << "[";
    for(int j = 0; j < data[0].size(); j++) {
       file_write << data[i][j];
       if (j != data[0].size() - 1) {
        file_write << ", ";
       }
    }
    file_write << "]";
    file_write << std::endl;
  }
  file_write.close();
}

inline void createDirectory(std::string path, bool emptyIfExists = true) {
  if (!fileExists(path)) {
    mkdir(path.c_str(), 0775);
  } else if (emptyIfExists) {
    system(std::string("exec rm -r " + path + "/*").c_str());
  }
}

inline bool is_not_digit(char c)
{
    return !std::isdigit(c);
}

inline bool numeric_string_compare(const std::string& s1, const std::string& s2)
{
  std::string::const_iterator it1 = s1.begin(), it2 = s2.begin();

  if (std::isdigit(s1[0]) && std::isdigit(s2[0])) {
    int n1, n2;
    std::stringstream ss(s1);
    ss >> n1;
    ss.clear();
    ss.str(s2);
    ss >> n2;

    if (n1 != n2) return n1 < n2;

    it1 = std::find_if(s1.begin(), s1.end(), is_not_digit);
    it2 = std::find_if(s2.begin(), s2.end(), is_not_digit);
  }

  return std::lexicographical_compare(it1, s1.end(), it2, s2.end());
}

// maskLength is the number of digits that represent a number with file names like file_001.wav, file_002.wav

typedef struct numeric_file_compare {
  numeric_file_compare(int maskLength = -1) : mMaskLength(maskLength) {}
  inline bool operator()(const std::string& string1, const std::string& string2);
  int mMaskLength;
} numeric_file_compare;

inline bool numeric_file_compare::operator()(const std::string& string1, const std::string& string2)
{
  auto tmp1(getFilenameFromPath(string1, true));
  auto tmp2(getFilenameFromPath(string2, true));
  if (mMaskLength > 0) {
    tmp1 = tmp1.substr(tmp1.length() - mMaskLength - 1);
    tmp2 = tmp2.substr(tmp2.length() - mMaskLength - 1);
  }
  const string s1(tmp1);
  const string s2(tmp2);

  std::string::const_iterator it1 = s1.begin(), it2 = s2.begin();

  if (std::isdigit(s1[0]) && std::isdigit(s2[0])) {
    int n1, n2;
    std::stringstream ss(s1);
    ss >> n1;
    ss.clear();
    ss.str(s2);
    ss >> n2;

    if (n1 != n2) return n1 < n2;

    it1 = std::find_if(s1.begin(), s1.end(), is_not_digit);
    it2 = std::find_if(s2.begin(), s2.end(), is_not_digit);
  }

  return std::lexicographical_compare(it1, s1.end(), it2, s2.end());
}

// https://stackoverflow.com/questions/1798112/removing-leading-and-trailing-spaces-from-a-string

// trim from left
inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from right
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from left & right
inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
    return ltrim(rtrim(s, t), t);
}

// copying versions

inline std::string ltrim_copy(std::string s, const char* t = " \t\n\r\f\v")
{
    return ltrim(s, t);
}

inline std::string rtrim_copy(std::string s, const char* t = " \t\n\r\f\v")
{
    return rtrim(s, t);
}

inline std::string trim_copy(std::string s, const char* t = " \t\n\r\f\v")
{
    return trim(s, t);
}

}}

#endif
