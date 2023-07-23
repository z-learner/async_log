#ifndef __ASYNC_LOG_HPP__
#define __ASYNC_LOG_HPP__

#include <any>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <variant>

#include "fmt/format.h"

namespace AsyncLog {

enum LogLv { DEBUG = 0, INFO = 1, WARN = 2, ERROR = 3, FATAL = 4 };

class LogTaskBase {
 public:
  virtual ~LogTaskBase() = default;
  virtual void ProcessLogTask() = 0;
};

template <typename... Args>
class LogTask : public LogTaskBase {
 public:
  template <typename... TArgs>
  LogTask(LogLv level, const char* file, int line, std::string format_data,
          TArgs&&... log_data)
      : _level(level),
        _file_name(file),
        _line(line),
        _format_data(format_data),
        _log_data(std::forward<TArgs>(log_data)...) {}

  // Optionally, you can provide getter functions for the level and log_data.
  void ProcessLogTask() {
    std::string log_msg = std::apply(
        [&](auto&&... args) {
          return fmt::format(_format_data,
                             args...);  // Formatting the log data
        },
        _log_data);

    size_t lastSlash = _file_name.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
      _file_name = _file_name.substr(lastSlash + 1);
    }
    std::cout << "[" << _file_name << ":" << _line << "] " << log_msg
              << std::endl;
  }

 private:
  LogLv _level;
  std::string _format_data;
  std::string _file_name;
  int _line;
  std::tuple<Args...> _log_data;
};

class AsyncLog {
 public:
  static AsyncLog& instance() {
    static AsyncLog instance;
    return instance;
  }

  ~AsyncLog() {
    Stop();
    _work_thread->join();
  }

  void Stop() {
    _stop.store(true, std::memory_order_relaxed);
    _empty_cond.notify_one();
  }

  template <typename... Args>
  void AsyncWrite(LogLv level, const char* file, int line,
                  const std::string& format_data, Args&&... args) {
    std::unique_ptr<LogTaskBase> task = std::make_unique<LogTask<Args...>>(
        level, file, line, format_data, std::forward<Args>(args)...);
    bool notify = false;
    {
      std::unique_lock<std::mutex> lock(_log_task_mtx);
      _log_tasks.push(std::move(task));
      notify = _log_tasks.size() == 1;
    }
    if (notify) {
      _empty_cond.notify_one();
    }
  }

 private:
  AsyncLog() : _stop(false), _work_thread{nullptr} {
    _work_thread = std::make_unique<std::thread>([this]() {
      for (;;) {
        std::unique_lock<std::mutex> lock(_log_task_mtx);
        _empty_cond.wait(lock, [this]() {
          return _stop.load(std::memory_order_acq_rel) || !_log_tasks.empty();
        });

        if (_stop.load(std::memory_order_acq_rel)) {
          break;
        }

        std::unique_ptr<LogTaskBase> log_task = std::move(_log_tasks.front());
        _log_tasks.pop();
        lock.unlock();

        log_task->ProcessLogTask();
      }
      std::cout << "exit log work thread..." << std::endl;
    });
  }

  AsyncLog(const AsyncLog&) = delete;
  AsyncLog& operator=(const AsyncLog&) = delete;

 private:
  std::atomic_bool _stop;
  std::unique_ptr<std::thread> _work_thread;
  std::queue<std::unique_ptr<LogTaskBase>> _log_tasks;
  std::mutex _log_task_mtx;
  std::condition_variable _empty_cond;
};

}  // namespace AsyncLog

#endif