#include <util/singleton.h>
#include <util/scheduler.h>
#include <util/timer.h>

#include <map>
#include <boost/thread.hpp>

using namespace std;

namespace izenelib {
namespace util {

class QueueTimer : public Timer {
public:
    QueueTimer(void (*callback)(void *),
             void *arg,
             uint32_t due_time,
             uint32_t period)
    : callback_(callback),
      arg_(arg),
      due_time_(due_time),
      period_(period) {
    }

    bool start() {
      return Timer::start(due_time_, period_);
    }

    virtual void signaled() {
      callback_(arg_);
    }

private:
    void (*callback_)(void *);
    void *arg_;
    uint32_t due_time_;
    uint32_t period_;
};

struct Job {
    uint32_t default_interval;
    uint32_t delay_start;
    boost::function<void (void)> callback;
    QueueTimer* timer;
    bool running;
};

class SchedulerImpl {
 public:
  SchedulerImpl() {
  }

  virtual ~SchedulerImpl() {
    removeAllJobs();
  }

  void removeAllJobs() {
    boost::mutex::scoped_lock l(mutex_);
    for (std::map<std::string, Job>::iterator itr = jobs_.begin();
         itr != jobs_.end();
         ++itr) {
      Job *job = &itr->second;
      if (job->timer) {
        job->timer->stop();
        delete job->timer;
      }
    }
    jobs_.clear();
  }

  bool addJob(const string &name, uint32_t default_interval, 
              uint32_t delay_start, const boost::function<void (void)>& func) {
    boost::mutex::scoped_lock l(mutex_);
    std::map<string, Job>::iterator find_itr = jobs_.find(name);
    if (find_itr != jobs_.end()) {
      return false;
    }

    Job newjob;
    newjob.default_interval = default_interval;
    newjob.delay_start = delay_start;
    newjob.callback = func;
    newjob.timer = NULL;
    newjob.running = false;

    std::pair<std::map<std::string, Job>::iterator, bool> result
      = jobs_.insert(make_pair(name, newjob));
    if (!result.second) {
      return false;
    }
    Job *job = &result.first->second;

    uint32_t delay = job->delay_start;
    job->timer = new QueueTimer(&timerCallback, job, delay,
                                job->default_interval);
    if (job->timer == NULL) {
        return false;
    }
    const bool started =  job->timer->start();
    if (started) {
        return true;
    } else {
        delete job->timer;
        return false;
    }
  }

  bool removeJob(const std::string &name) {
      boost::mutex::scoped_lock l(mutex_);
      std::map<std::string, Job>::iterator itr = jobs_.find(name);
      if (itr == jobs_.end()) {
        return false;
    } else {
        Job *job = &itr->second;
        if (job->timer != NULL) {
          job->timer->stop();
          delete job->timer;
        }
        jobs_.erase(itr);
        return true;
    }
  }

 private:
  static void timerCallback(void *param) {
      Job *job = reinterpret_cast<Job *>(param);
      if (job->running) {
          return;
      }
      job->running = true;
      job->callback();
      job->running = false;
    }

    std::map<std::string, Job> jobs_;
    boost::mutex mutex_;
};

bool Scheduler::addJob(const string &name, uint32_t default_interval,
                       uint32_t delay_start, const boost::function<void (void)>& func) {
    return Singleton<SchedulerImpl>::get()->addJob(name, default_interval, delay_start, func);
}

bool Scheduler::removeJob(const string &name) {
    return Singleton<SchedulerImpl>::get()->removeJob(name);
}

void Scheduler::removeAllJobs() {
    Singleton<SchedulerImpl>::get()->removeAllJobs();
}

}}