#include "async_log.hpp"

int main(int argc, char** argv) {
  AsyncLog::AsyncLog::instance().AsyncWrite(AsyncLog::LogLv::DEBUG, __FILE__,
                                            __LINE__, "Log data: {} {}", 42,
                                            "Hello");
  AsyncLog::AsyncLog::instance().AsyncWrite(AsyncLog::LogLv::ERROR, __FILE__,
                                            __LINE__, "Error occurred: {}",
                                            "File not found");
  std::this_thread::sleep_for(std::chrono::seconds(1));
  AsyncLog::AsyncLog::instance().Stop();
  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::cout << "exit main ..." << std::endl;
}