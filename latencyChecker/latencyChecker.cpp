#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <time.h>

#define fileName "latencyLog.txt"

// Attempts to execute given command
std::string exec(const char *cmd) {
  char buffer[128];
  std::string result = "";
  FILE *pipe = popen(cmd, "r");
  if (!pipe) throw std::runtime_error("popen() failed!");
  try {
    while (fgets(buffer, sizeof buffer, pipe) != NULL) {
      result += buffer;
    }
  }
  catch (...) {
    pclose(pipe);
    throw;
  }
  pclose(pipe);
  return result;
}

// Extracts from returned output the statistics
std::string extractTime(std::string raw) {
  int statsPos = raw.find("--- google.com ping statistics ---\n");  // Search string is 35 chars ('\n' counts as one)
  if (statsPos == -1) return "Ping failed!";
  std::string lastLines = raw.substr(statsPos + 35, 100);
  return lastLines.substr(0, lastLines.find('\n'));
}

// Formats string str with date and time info (appends \n at end)
std::string loggify(std::string str) {
  // From here: http://www.cplusplus.com/reference/ctime/localtime/
  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  char buffer [128] = "";
  sprintf(buffer, "%s: %s", str.c_str(), asctime(timeinfo));
  return std::string(buffer);
}

int main() {
  FILE *pFile;
  pFile = fopen(fileName, "a+");
  if (pFile != NULL) {
    fputs(loggify(extractTime(exec("ping -c 4 google.com"))).c_str(), pFile);
    fclose(pFile);
  }
  else std::cerr << "Failed to open " << fileName << '\n';

  return 0;
}
