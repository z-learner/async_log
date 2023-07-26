#include "async_log.hpp"

int main(int argc, char** argv) {
  ASYNC_SET_FILE("test.log");

  ASYNC_LOG_DEBUG("{} + {} = {}", 1, 1, 2);
  ASYNC_LOG_INFO("{} + {} = {}", 2, 1, 3);
  ASYNC_LOG_WARN("{} + {} = {}", 2, 1, 3);
  ASYNC_LOG_ERROR("{} + {} = {}", 2, 1, 3);
  ASYNC_LOG_FATAL("{} + {} = {}", 2, 1, 3);

  std::this_thread::sleep_for(std::chrono::seconds(1));
  AsyncLog::AsyncLog::instance().Stop();
  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::cout << "exit main ..." << std::endl;
}